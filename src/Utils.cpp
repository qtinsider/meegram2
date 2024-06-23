#include "Utils.hpp"

#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "TdApi.hpp"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QDateTime>
#include <QImageReader>
#include <QLocale>
#include <QScopedPointer>
#include <QStringBuilder>
#include <QStringList>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

#include <algorithm>
#include <cmath>
#include <unordered_map>

namespace detail {

bool isBotUser(const QVariantMap &user) noexcept
{
    auto userType = user.value("type").toMap();
    return userType.value("@type").toByteArray() == "userTypeBot";
}

bool isMeChat(const QVariantMap &chat, StorageManager *store) noexcept
{
    auto type = chat.value("type").toMap();

    auto chatType = type.value("@type").toByteArray();
    if (chatType == "chatTypeSecret" || chatType == "chatTypePrivate")
    {
        return store->getMyId() == type.value("user_id").toLongLong();
    }

    return false;
}

bool isMeUser(qint64 userId, StorageManager *store) noexcept
{
    return store->getMyId() == userId;
}

bool isUserOnline(const QVariantMap &user) noexcept
{
    auto status = user.value("status").toMap();
    auto type = user.value("type").toMap();

    if (std::ranges::any_of(ServiceNotificationsUserIds, [user](qint64 userId) { return userId == user.value("id").toLongLong(); }))
    {
        return false;
    }

    return status.value("@type").toByteArray() == "userStatusOnline" && type.value("@type").toByteArray() != "userTypeBot";
}

bool isChannelChat(const QVariantMap &chat) noexcept
{
    auto type = chat.value("type").toMap();

    auto chatType = type.value("@type").toByteArray();
    if (chatType == "chatTypeSupergroup")
        return type.value("is_channel").toBool();

    return false;
}

bool isSupergroup(const QVariantMap &chat) noexcept
{
    auto type = chat.value("type").toMap();

    return type.value("@type").toByteArray() == "chatTypeSupergroup";
}

QString getMessageAuthor(const QVariantMap &message, StorageManager *store, Locale *locale, bool openUser) noexcept
{
    auto sender = message.value("sender_id").toMap();

    if (sender.value("@type").toByteArray() == "messageSenderUser")
        return openUser
                   ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"userId://" +
                         sender.value("user_id").toString() + "\">" +  Utils::getUserShortName(sender.value("user_id").toLongLong(), store, locale) + "</a>"
                   :  Utils::getUserShortName(sender.value("user_id").toLongLong(), store, locale);

    auto chat = store->getChat(message.value("chat_id").toLongLong());

    return openUser ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"chatId://" +
                          sender.value("chat_id").toString() + "\">" + chat.value("title").toString() + "</a>"
                    : chat.value("title").toString();
}

QString getUserFullName(qint64 userId, StorageManager *store, Locale *locale) noexcept
{
    auto user = store->getUser(userId);

    auto type = user.value("type").toMap();
    auto firstName = user.value("first_name").toString();
    auto lastName = user.value("last_name").toString();

    auto userType = type.value("@type").toByteArray();

    switch (fnv::hashRuntime(userType.constData()))
    {
        case fnv::hash("userTypeBot"):
        case fnv::hash("userTypeRegular"): {
            return QString(firstName % " " % lastName).trimmed();
        }
        case fnv::hash("userTypeDeleted"):
        case fnv::hash("userTypeUnknown"): {
            return locale->getString("HiddenName");
        }
    }

    return QString();
}

QString getCallContent(const QVariantMap &sender, const QVariantMap &content, StorageManager *store, Locale *locale) noexcept
{
    auto isVideo = content.value("is_video").toBool();
    auto discardReason = content.value("discard_reason").toMap();
    auto isMissed = discardReason.value("@type").toByteArray() == "callDiscardReasonMissed";
    auto isBusy = discardReason.value("@type").toByteArray() == "callDiscardReasonDeclined";

    if (isMeUser(sender.value("user_id").toInt(), store))
    {
        if (isMissed)
        {
            if (isVideo)
            {
                return locale->getString("CallMessageVideoOutgoingMissed");
            }

            return locale->getString("CallMessageOutgoingMissed");
        }

        if (isVideo)
        {
            return locale->getString("CallMessageVideoOutgoing");
        }

        return locale->getString("CallMessageOutgoing");
    }

    if (isMissed)
    {
        if (isVideo)
        {
            return locale->getString("CallMessageVideoIncomingMissed");
        }

        return locale->getString("CallMessageIncomingMissed");
    }
    else if (isBusy)
    {
        if (isVideo)
        {
            return locale->getString("CallMessageVideoIncomingDeclined");
        }

        return locale->getString("CallMessageIncomingDeclined");
    }

    if (isVideo)
    {
        return locale->getString("CallMessageVideoIncoming");
    }

    return locale->getString("CallMessageIncoming");
}

QString getAudioTitle(const QVariantMap &audio) noexcept
{
    auto fileName = audio.value("file_name").toString();
    auto title = audio.value("title").toString();
    auto performer = audio.value("performer").toString();

    if (title.isEmpty() and performer.isEmpty())
        return fileName;

    QString result;
    result += performer.trimmed().isEmpty() ? "Unknown Artist" : performer;
    result += " - ";
    result += title.trimmed().isEmpty() ? "Unknown Track" : title;

    return result;
}

bool isUserBlocked(qint64 userId, StorageManager *store) noexcept
{
    auto fullInfo = store->getUserFullInfo(userId);

    if (fullInfo.isEmpty())
        return false;

    return fullInfo.value("is_blocked").toBool();
}

bool isDeletedUser(qint64 userId, StorageManager *store)
{
    auto user = store->getUser(userId);

    auto userType = user.value("type").toMap();
    return userType.value("@type").toByteArray() == "userTypeDeleted";
}

void appendDuration(int count, QChar &&order, QString &outString)
{
    outString.append(QString::number(count));
    outString.append(order);
}
}  // namespace detail

namespace Utils {

QVariantMap  getChatPosition(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    auto chat = store->getChat(chatId);
    auto positions = chat.value("positions").toList();

    auto chatListType = chatList.value("@type").toByteArray();
    switch (fnv::hashRuntime(chatListType.constData()))
    {
        case fnv::hash("chatListMain"): {
            if (auto it = std::ranges::find_if(positions,
                                               [](const auto &position) {
                                                   auto list = position.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListMain";
                                               });
                it != positions.end())
                return it->toMap();

            break;
        }
        case fnv::hash("chatListArchive"): {
            if (auto it = std::ranges::find_if(positions,
                                               [](const auto &position) {
                                                   auto list = position.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListArchive";
                                               });
                it != positions.end())
                return it->toMap();

            break;
        }
        case fnv::hash("chatListFilter"): {
            if (auto it = std::ranges::find_if(positions,
                                               [chatList](const auto &position) {
                                                   auto list = position.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListFilter" &&
                                                          list.value("chat_filter_id").toInt() == chatList.value("chat_filter_id").toInt();
                                               });
                it != positions.end())
                return it->toMap();

            break;
        }
    }

    return {};
}

bool  isChatPinned(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    auto position = getChatPosition(chatId, chatList, store);

    return position.value("is_pinned").toBool();
}

qint64  getChatOrder(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    auto position = getChatPosition(chatId, chatList,store);

    return position.value("order").toLongLong();
}

bool  chatListEquals(const QVariantMap &list1, const QVariantMap &list2)
{
    if (list1.value("@type") != list2.value("@type"))
        return false;

    auto listType = list1.value("@type").toByteArray();

    switch (fnv::hashRuntime(listType.constData()))
    {
        case fnv::hash("chatListMain"):
            return true;
        case fnv::hash("chatListArchive"):
            return true;
        case fnv::hash("chatListFilter"): {
            return list1.value("chat_filter_id").toByteArray() == list2.value("chat_filter_id").toByteArray();
        }
    }

    return false;
}

QString  getChatTitle(qint64 chatId, StorageManager *store, Locale *locale, bool showSavedMessages)
{
    auto chat = store->getChat(chatId);

    if (detail::isMeChat(chat, store) && showSavedMessages)
        return locale->getString("SavedMessages");

    auto title = chat.value("title").toString();

    return title.isEmpty() ? locale->getString("HiddenName") : title;
}

bool  isChatMuted(qint64 chatId, StorageManager *store)
{
    return getChatMuteFor(chatId, store) > 0;
}

int  getChatMuteFor(qint64 chatId, StorageManager *store)
{
    auto chat = store->getChat(chatId);

    auto notificationSettings = chat.value("notification_settings").toMap();

    return notificationSettings.value("mute_for").toInt();
}

QString  getServiceMessageContent(const QVariantMap &message, StorageManager *store, Locale *locale, bool openUser)
{
    auto ttl = message.value("ttl").toInt();
    auto sender = message.value("sender_id").toMap();
    auto content = message.value("content").toMap();
    auto isOutgoing = message.value("is_outgoing").toBool();

    auto chat = store->getChat(message.value("chat_id").toLongLong());

    auto isChannel = detail::isChannelChat(chat);

    auto author = detail::getMessageAuthor(message, store, locale, openUser);

    auto getUserName = [openUser, store, locale](qlonglong userId) {
        return openUser ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"userId://" + QString::number(userId) +
                              "\">" +  getUserShortName(userId, store, locale) + "</a>"
                        :  getUserShortName(userId,store,locale);
    };

    auto contentType = content.value("@type").toByteArray();
    if (ttl > 0)
    {
        switch (fnv::hashRuntime(contentType.constData()))
        {
            case fnv::hash("messagePhoto"): {
                if (isOutgoing)
                {
                    return locale->getString("ActionYouSendTTLPhoto");
                }

                return locale->getString("ActionSendTTLPhoto").replace("un1", author);
            }
            case fnv::hash("messageVideo"): {
                if (isOutgoing)
                {
                    return locale->getString("ActionYouSendTTLVideo");
                }

                return locale->getString("ActionSendTTLVideo").replace("un1", author);
            }
        }
    }

    switch (fnv::hashRuntime(contentType.constData()))
    {
        case fnv::hash("messageExpiredPhoto"): {
            return locale->getString("AttachPhotoExpired");
        }
        case fnv::hash("messageExpiredVideo"): {
            return locale->getString("AttachVideoExpired");
        }
        case fnv::hash("messageBasicGroupChatCreate"): {
            if (isOutgoing)
            {
                return locale->getString("ActionYouCreateGroup");
            }

            return locale->getString("ActionCreateGroup").replace("un1", author);
        }
        case fnv::hash("messageSupergroupChatCreate"): {
            if (isChannel)
            {
                return locale->getString("ActionCreateChannel");
            }

            return locale->getString("ActionCreateMega");
        }
        case fnv::hash("messageChatChangeTitle"): {
            auto title = content.value("title").toString();

            if (isChannel)
            {
                return locale->getString("ActionChannelChangedTitle").replace("un2", title);
            }

            if (isOutgoing)
            {
                return locale->getString("ActionYouChangedTitle").replace("un2", title);
            }

            return locale->getString("ActionChangedTitle").replace("un1", author).replace("un2", title);
        }
        case fnv::hash("messageChatChangePhoto"): {
            if (isChannel)
            {
                return locale->getString("ActionChannelChangedPhoto");
            }

            if (isOutgoing)
            {
                return locale->getString("ActionYouChangedPhoto");
            }

            return locale->getString("ActionChangedPhoto").replace("un1", author);
        }
        case fnv::hash("messageChatDeletePhoto"): {
            if (isChannel)
            {
                return locale->getString("ActionChannelRemovedPhoto");
            }

            if (isOutgoing)
            {
                return locale->getString("ActionYouRemovedPhoto");
            }

            return locale->getString("ActionRemovedPhoto").replace("un1", author);
        }
        case fnv::hash("messageChatAddMembers"): {
            auto singleMember = content.value("member_user_ids").toList().length() == 1;

            if (singleMember)
            {
                auto memberUserId = content.value("member_user_ids").toList()[0].toLongLong();
                if (sender.value("user_id") == memberUserId)
                {
                    if (detail::isSupergroup(chat) && isChannel)
                    {
                        return locale->getString("ChannelJoined");
                    }

                    if (detail::isSupergroup(chat) && !isChannel)
                    {
                        if (detail::isMeUser(memberUserId, store))
                        {
                            return locale->getString("ChannelMegaJoined");
                        }

                        return locale->getString("ActionAddUserSelfMega").replace("un1", author);
                    }

                    if (isOutgoing)
                    {
                        return locale->getString("ActionAddUserSelfYou");
                    }

                    return locale->getString("ActionAddUserSelf").replace("un1", author);
                }

                if (isOutgoing)
                {
                    return locale->getString("ActionYouAddUser").replace("un2", getUserName(memberUserId));
                }

                if (detail::isMeUser(memberUserId, store))
                {
                    if (detail::isSupergroup(chat))
                    {
                        if (!isChannel)
                        {
                            return locale->getString("MegaAddedBy").replace("un1", author);
                        }

                        return locale->getString("ChannelAddedBy").replace("un1", author);
                    }

                    return locale->getString("ActionAddUserYou").replace("un1", author);
                }

                return locale->getString("ActionAddUser").replace("un1", author).replace("un2", getUserName(memberUserId));
            }

            QStringList result;
            for (const auto &userId : content.value("member_user_ids").toList())
            {
                result << getUserName(userId.toLongLong());
            }

            auto users = result.join(", ");

            if (isOutgoing)
            {
                return locale->getString("ActionYouAddUser").arg(users);
            }

            return locale->getString("ActionAddUser").replace("un1", author).replace("un2", users);
        }
        case fnv::hash("messageChatJoinByLink"): {
            if (isOutgoing)
            {
                return locale->getString("ActionInviteYou");
            }

            return locale->getString("ActionInviteUser").replace("un1", author);
        }
        case fnv::hash("messageChatDeleteMember"): {
            if (content.value("user_id").toLongLong() == sender.value("user_id").toLongLong())
            {
                if (isOutgoing)
                {
                    return locale->getString("ActionYouLeftUser");
                }

                return locale->getString("ActionLeftUser").replace("un1", author);
            }

            if (isOutgoing)
            {
                return locale->getString("ActionYouKickUser")
                    .replace("un2", getUserName(content.value("user_id").toLongLong()));
            }
            else if (detail::isMeUser(content.value("user_id").toLongLong(),store))
            {
                return locale->getString("ActionKickUserYou").replace("un1", author);
            }

            return locale->getString("ActionKickUser")
                .replace("un1", author)
                .replace("un2", getUserName(content.value("user_id").toLongLong()));
        }
        case fnv::hash("messageChatUpgradeTo"): {
            return locale->getString("ActionMigrateFromGroup");
        }
        case fnv::hash("messageChatUpgradeFrom"): {
            return locale->getString("ActionMigrateFromGroup");
        }
        case fnv::hash("messagePinMessage"): {
            return locale->getString("ActionPinned").replace("un1", author);
        }
        case fnv::hash("messageScreenshotTaken"): {
            if (isOutgoing)
            {
                return locale->getString("ActionTakeScreenshootYou");
            }

            return locale->getString("ActionTakeScreenshoot").replace("un1", author);
        }
        case fnv::hash("messageChatSetTtl"): {
            auto ttlString = locale->formatTtl(content.value("ttl").toInt());

            if (content.value("ttl").toInt() <= 0)
            {
                if (isOutgoing)
                    return locale->getString("MessageLifetimeYouRemoved");

                return locale->getString("MessageLifetimeRemoved")
                    .arg(getUserName(sender.value("user_id").toLongLong()));
            }

            if (isOutgoing)
                return locale->getString("MessageLifetimeChangedOutgoing").arg(ttlString);

            return locale->getString("MessageLifetimeChanged")
                .arg(getUserName(sender.value("user_id").toLongLong()))
                .arg(ttlString);
        }
        case fnv::hash("messageCustomServiceAction"): {
            return content.value("text").toString();
        }
        case fnv::hash("messageContactRegistered"): {
            return locale->getString("NotificationContactJoined")
                .arg(getUserName(sender.value("user_id").toLongLong()));
        }
        case fnv::hash("messageWebsiteConnected"): {
            return locale->getString("ActionBotAllowed");
        }
        case fnv::hash("messageUnsupported"): {
            return locale->getString("UnsupportedMedia");
        }
    }

    return locale->getString("UnsupportedMedia");
}

bool  isServiceMessage(const QVariantMap &message)
{
    auto contentType = message.value("content").toMap().value("@type").toByteArray();
    static const std::unordered_map<std::string, bool> serviceMap = {
        {"messageText", false},
        {"messageAnimation", false},
        {"messageAudio", false},
        {"messageDocument", false},
        {"messagePhoto", false},
        {"messageExpiredPhoto", true},
        {"messageSticker", false},
        {"messageVideo", false},
        {"messageExpiredVideo", true},
        {"messageVideoNote", false},
        {"messageVoiceNote", false},
        {"messageLocation", false},
        {"messageVenue", false},
        {"messageContact", false},
        {"messageDice", false},
        {"messageGame", false},
        {"messagePoll", false},
        {"messageInvoice", false},
        {"messageCall", false},
        {"messageBasicGroupChatCreate", true},
        {"messageSupergroupChatCreate", true},
        {"messageChatChangeTitle", true},
        {"messageChatChangePhoto", true},
        {"messageChatDeletePhoto", true},
        {"messageChatAddMembers", true},
        {"messageChatJoinByLink", true},
        {"messageChatDeleteMember", true},
        {"messageChatUpgradeTo", true},
        {"messageChatUpgradeFrom", true},
        {"messagePinMessage", true},
        {"messageScreenshotTaken", true},
        {"messageChatSetTtl", true},
        {"messageCustomServiceAction", true},
        {"messageGameScore", true},
        {"messagePaymentSuccessful", true},
        {"messagePaymentSuccessfulBot", true},
        {"messageContactRegistered", true},
        {"messageWebsiteConnected", true},
        {"messagePassportDataSent", true},
        {"messagePassportDataReceived", true},
        {"messageLiveLocationApproached", true},
        {"messageUnsupported", true},
    };

    if (auto it = serviceMap.find(contentType.constData()); it != serviceMap.end())
    {
        return it->second || message.value("ttl").toInt() > 0;
    }

    return {};
}

QString  getUserShortName(qint64 userId, StorageManager *store, Locale *locale) noexcept
{
    auto user = store->getUser(userId);

    auto type = user.value("type").toMap();
    auto firstName = user.value("first_name").toString();
    auto lastName = user.value("last_name").toString();

    auto userType = type.value("@type").toByteArray();
    if (userType == "userTypeBot" || userType == "userTypeRegular")
    {
        if (!firstName.isEmpty())
            return firstName;

        if (!lastName.isEmpty())
            return lastName;
    }
    if (userType == "userTypeDeleted" || userType == "userTypeUnknown")
        return locale->getString("HiddenName");

    return QString();
}

QString  getTitle(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept
{
    auto sender = message.value("sender_id").toMap();

    auto senderType = sender.value("@type").toByteArray();
    if (senderType == "messageSenderUser")
    {
        return detail::getUserFullName(sender.value("user_id").toLongLong(), store, locale);
    }

    if (senderType == "messageSenderChat")
        return store->getChat(sender.value("chat_id").toLongLong()).value("title").toString();

    return QString();
}

QString  getMessageDate(const QVariantMap &message, Locale *locale) noexcept
{
    auto date = QDateTime::fromMSecsSinceEpoch(message.value("date").toLongLong() * 1000);

    auto days = date.daysTo(QDateTime::currentDateTime());

    if (days == 0)
        return date.toString(locale->getString("formatterDay12H"));
    else if (days < 7)
        return date.toString(locale->getString("formatterWeek"));

    return date.toString(locale->getString("formatterYear"));
}

QString  getContent(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept
{
    auto content = message.value("content").toMap();
    auto sender = message.value("sender_id").toMap();

    auto textOneLine = [](QString text) { return text.replace("\n", " ").replace("\r", " "); };

    QString caption;
    if (!content.value("caption").toMap().value("text").toString().isEmpty())
        caption.append(": ").append(textOneLine(content.value("caption").toMap().value("text").toString()));

    if (message.value("ttl").toInt() > 0)
        return  getServiceMessageContent(message,store, locale);

    auto contentType = content.value("@type").toByteArray();
    switch (fnv::hashRuntime(contentType.constData()))
    {
        case fnv::hash("messageAnimation"): {
            return locale->getString("AttachGif").append(caption);
        }
        case fnv::hash("messageAudio"): {
            auto audio = content.value("audio").toMap();
            auto title = detail::getAudioTitle(audio).isEmpty() ? locale->getString("AttachMusic")
                                                                : detail::getAudioTitle(audio);

            return title.append(caption);
        }
        case fnv::hash("messageBasicGroupChatCreate"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageCall"): {
            auto text = detail::getCallContent(sender, content,store, locale);

            auto duration = content.value("duration").toInt();
            if (duration > 0)
            {
                return locale->getString("CallMessageWithDuration")
                    .arg(text)
                    .arg(locale->formatCallDuration(duration));
            }

            return text;
        }
        case fnv::hash("messageChatAddMembers"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatChangePhoto"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatChangeTitle"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatDeleteMember"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatDeletePhoto"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatJoinByLink"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatSetTtl"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatUpgradeFrom"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageChatUpgradeTo"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageContact"): {
            return locale->getString("AttachContact").append(caption);
        }
        case fnv::hash("messageContactRegistered"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageCustomServiceAction"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageDocument"): {
            auto document = content.value("document").toMap();
            auto fileName = document.value("file_name").toString();
            if (!fileName.isEmpty())
            {
                return fileName.append(caption);
            }

            return locale->getString("AttachDocument").append(caption);
        }
        case fnv::hash("messageExpiredPhoto"): {
            return locale->getString("AttachPhoto").append(caption);
        }
        case fnv::hash("messageExpiredVideo"): {
            return locale->getString("AttachVideo").append(caption);
        }
        case fnv::hash("messageGame"): {
            return locale->getString("AttachGame").append(caption);
        }
        case fnv::hash("messageGameScore"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageInvoice"): {
            auto title = content.value("title").toString();
            return title.append(caption);
        }
        case fnv::hash("messageLocation"): {
            return locale->getString("AttachLocation").arg(caption);
        }
        case fnv::hash("messagePassportDataReceived"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messagePassportDataSent"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messagePaymentSuccessful"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messagePaymentSuccessfulBot"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messagePhoto"): {
            return locale->getString("AttachPhoto").append(caption);
        }
        case fnv::hash("messagePoll"): {
            auto poll = content.value("poll").toMap();
            auto question = poll.value("question").toString();

            return QString::fromUtf8("\xf0\x9f\x93\x8a\x20").append(question);
        }
        case fnv::hash("messagePinMessage"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageScreenshotTaken"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageSticker"): {
            auto sticker = content.value("sticker").toMap();
            auto emoji = sticker.value("emoji").toString();

            return locale->getString("AttachSticker").append(": ").append(emoji);
        }
        case fnv::hash("messageSupergroupChatCreate"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageText"): {
            return textOneLine(content.value("text").toMap().value("text").toString());
        }
        case fnv::hash("messageUnsupported"): {
            return  getServiceMessageContent(message,store, locale);
        }
        case fnv::hash("messageVenue"): {
            return locale->getString("AttachLocation").append(caption);
        }
        case fnv::hash("messageVideo"): {
            return locale->getString("AttachVideo").append(caption);
        }
        case fnv::hash("messageVideoNote"): {
            return locale->getString("AttachRound").append(caption);
        }
        case fnv::hash("messageVoiceNote"): {
            return locale->getString("AttachAudio").append(caption);
        }
        case fnv::hash("messageWebsiteConnected"): {
            return  getServiceMessageContent(message,store, locale);
        }
    }

    return locale->getString("UnsupportedAttachment");
}

bool  isChatUnread(qint64 chatId, StorageManager *store) noexcept
{
    auto chat = store->getChat(chatId);

    auto isMarkedAsUnread = chat.value("is_marked_as_unread").toBool();
    auto unreadCount = chat.value("unread_count").toInt();

    return isMarkedAsUnread || unreadCount > 0;
}

QString  getMessageSenderName(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept
{
    if (isServiceMessage(message))
        return QString();

    auto sender = message.value("sender_id").toMap();

    auto chat = store->getChat(message.value("chat_id").toLongLong());

    auto chatType = chat.value("type").toMap().value("@type").toByteArray();
    switch (fnv::hashRuntime(chatType.constData()))
    {
        case fnv::hash("chatTypePrivate"):
        case fnv::hash("chatTypeSecret"): {
            return QString();
        }
        case fnv::hash("chatTypeBasicGroup"):
        case fnv::hash("chatTypeSupergroup"): {
            if (detail::isChannelChat(chat))
            {
                return QString();
            }

            auto senderType = sender.value("@type").toByteArray();
            switch (fnv::hashRuntime(senderType.constData()))
            {
                case fnv::hash("messageSenderUser"): {
                    if (detail::isMeUser(sender.value("user_id").toLongLong(),store))
                        return locale->getString("FromYou");

                    return getUserShortName(sender.value("user_id").toLongLong(), store, locale);
                }
                case fnv::hash("messageSenderChat"): {
                    return getChatTitle(chat.value("id").toLongLong(), store, locale);
                }
            }
        }
    }

    return QString();
}

// TODO(strawberry):
QString  getFormattedText(const QVariantMap &formattedText, StorageManager *store, Locale *locale, const QVariantMap &options) noexcept
{
    auto text = formattedText.value("text").toString();
    auto entities = formattedText.value("entities").toList();

    QFont font;
    font.setPixelSize(23);
    font.setWeight(QFont::Light);

    QScopedPointer<QTextDocument> doc(new QTextDocument);
    doc->setDefaultFont(font);
    doc->setPlainText(text);

QTextCursor cursor(doc.data());

    for ( const auto &entity : entities)
    {
        auto offset = entity.toMap().value("offset").toInt();
        auto length = entity.toMap().value("length").toInt();
        auto type = entity.toMap().value("type").toMap();

        cursor.setPosition(offset, QTextCursor::MoveAnchor);
        cursor.setPosition(offset + length, QTextCursor::KeepAnchor);

        auto entityText = text.mid(offset, length);

        auto entityType = type.value("@type").toByteArray();
        switch (fnv::hashRuntime(entityType.constData()))
        {
            case fnv::hash("textEntityTypeBold"): {
                QTextCharFormat format;
                format.setFontWeight(QFont::Bold);
                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeBotCommand"): {
                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref("botCommand:" % entityText);
                cursor.mergeCharFormat(format);

                break;
            }
            case fnv::hash("textEntityTypeCashtag"): {
                break;
            }
            case fnv::hash("textEntityTypeEmailAddress"): {
                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref("mailto:" % entityText);
                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeHashtag"): {
                break;
            }
            case fnv::hash("textEntityTypeItalic"): {
                QTextCharFormat format;
                format.setFontItalic(true);
                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeMentionName"): {
                auto userId = type.value("user_id").toByteArray();
                auto title = detail::getUserFullName(type.value("user_id").toLongLong(), store, locale);

                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref("userId:" % title);

                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeMention"): {
                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref("username:" % entityText);

                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypePhoneNumber"): {
                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref("tel:" % entityText);

                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeCode"):
            case fnv::hash("textEntityTypePre"):
            case fnv::hash("textEntityTypePreCode"): {
                QTextCharFormat format;
                format.setFontFixedPitch(true);
                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeStrikethrough"): {
                QTextCharFormat format;
                format.setFontStrikeOut(true);
                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeTextUrl"): {
                auto url = type.value("url").toString().isEmpty() ? entityText : type.value("url").toString();

                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref(url);
                format.setFontUnderline(true);

                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeUrl"): {
                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref(entityText);
                format.setFontUnderline(true);

                cursor.mergeCharFormat(format);
                break;
            }
            case fnv::hash("textEntityTypeUnderline"): {
                QTextCharFormat format;
                format.setFontUnderline(true);
                cursor.mergeCharFormat(format);
                break;
            }
        }
    }

    return doc->toHtml();
}

void  copyToClipboard(const QVariantMap &content) noexcept
{
    auto contentType = content.value("@type").toByteArray();
    if (contentType != "messageText")
        return;

    auto clipboard = QApplication::clipboard();

    clipboard->setText(content.value("text").toMap().value("text").toString());
}

QImage  getThumb(const QVariantMap &thumbnail)  noexcept
{
    QByteArray thumb("data:image/jpeg;base64, ");
    thumb += thumbnail.value("data").toByteArray();

    QBuffer buf;
    buf.setData(thumb);

    QImageReader reader(&buf);

    return reader.read();
}

bool  isMessageUnread(qint64 chatId, const QVariantMap &message, StorageManager *store) noexcept
{
    auto chat = store->getChat(chatId);

    auto lastReadInboxMessageId = chat.value("last_read_inbox_message_id").toLongLong();
    auto lastReadOutboxMessageId = chat.value("last_read_outbox_message_id").toLongLong();

    auto id = message.value("id").toLongLong();
    auto isOutgoing = message.value("is_outgoing").toBool();
    if (isOutgoing && detail::isMeChat(chat, store))
        return false;

    return isOutgoing ? id > lastReadOutboxMessageId : id > lastReadInboxMessageId;
}

QString  getViews(int views)  noexcept
{
    if (views < 1000)
        return QString::number(views);

    if (views < 1000000)
    {
        auto value = trunc(views / 100);
        auto fractionDigits = int(value) % 10 < 1 ? 0 : 1;

        return QString("%1K").arg(value / 10, 0, 'f', fractionDigits);
    }

    auto value = trunc(views / 100000);
    auto fractionDigits = int(value) % 10 < 1 ? 0 : 1;

    return QString("%1M").arg(value / 10, 0, 'f', fractionDigits);
}

QString  getFileSize(const QVariantMap &file) noexcept
{
    const auto size = file.value("size").toInt();
    if (size <= 0)
        return QString();

    if (size < 1024)
        return QString("%1 B").arg(size);

    if (size < 1024 * 1024)
        return QString("%1 KB").arg(size / 1024);

    if (size < 1024 * 1024 * 1024)
        return QString("%1 MB").arg(size / 1024 / 1024);

    return QString("%1 GB").arg(size / 1024 / 1024 / 1024);
}

QString  formatTime(int totalSeconds)  noexcept
{
    QString res;

    int seconds = totalSeconds % 60;
    int timeoutMinutes = totalSeconds / 60;
    int minutes = timeoutMinutes % 60;
    int timeoutHours = timeoutMinutes / 60;
    int hours = timeoutHours % 24;
    int days = timeoutHours / 24;
    if (days > 0)
    {
        detail::appendDuration(days, 'd', res);
    }
    if (hours > 0)
    {
        if (!res.isEmpty())
        {
            res.append(" ");
        }
        detail::appendDuration(hours, 'h', res);
    }
    if (minutes > 0)
    {
        if (!res.isEmpty())
        {
            res.append(" ");
        }
        detail::appendDuration(minutes, 'm', res);
    }
    if (seconds > 0)
    {
        if (!res.isEmpty())
        {
            res.append(" ");
        }
        detail::appendDuration(seconds, 's', res);
    }
    return res;
}

}
