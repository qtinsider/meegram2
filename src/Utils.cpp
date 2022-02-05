#include "Utils.hpp"

#include "Common.hpp"
#include "TdApi.hpp"

#include <fnv-cpp/fnv.h>

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

bool isBotUser(qint64 userId) noexcept
{
    auto user = TdApi::getInstance().getUser(userId);

    auto userType = user.value("type").toMap();
    return userType.value("@type").toByteArray() == "userTypeBot";
}

bool isMeChat(const QVariantMap &chat) noexcept
{
    auto type = chat.value("type").toMap();

    auto chatType = type.value("@type").toByteArray();
    if (chatType == "chatTypeSecret" || chatType == "chatTypePrivate")
    {
        return TdApi::getInstance().userStore->getMyId() == type.value("user_id").toLongLong();
    }

    return false;
}

bool isMeUser(qint64 userId) noexcept
{
    return TdApi::getInstance().userStore->getMyId() == userId;
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

QString formatCallDuration(int duration) noexcept
{
    if (duration > 3600)
    {
        auto result = QObject::tr("CallDurationHours", "", std::floor(duration / 3600));
        auto minutes = std::floor(duration % 3600 / 3600);
        if (minutes > 0)
        {
            result += ", " % QObject::tr("CallDurationMinutes", "", minutes);
        }

        return result;
    }

    if (duration > 60)
    {
        return QObject::tr("CallDurationMinutes", "", std::floor(duration / 60));
    }

    return QObject::tr("CallDurationSeconds", "", std::floor(duration));
}

QString getTTLString(int ttl) noexcept
{
    if (ttl < 60)
    {
        return QObject::tr("TTLStringSeconds", "", std::floor(ttl));
    }
    if (ttl < 60 * 60)
    {
        return QObject::tr("TTLStringMinutes", "", std::floor(ttl / 60));
    }
    if (ttl < 24 * 60 * 60)
    {
        return QObject::tr("TTLStringHours", "", std::floor(ttl / 60 / 60));
    }

    auto days = ttl / 60 / 60 / 24;
    if (ttl % 7 == 0)
    {
        return QObject::tr("TTLStringWeeks", "", std::floor(days / 7));
    }

    return QObject::tr("TTLStringWeeks", "", std::floor(days / 7)) % QObject::tr("TTLStringDays", "", std::floor(days % 7));
}

QString getMessageAuthor(const QVariantMap &message, bool openUser) noexcept
{
    auto sender = message.value("sender_id").toMap();

    if (sender.value("@type").toByteArray() == "messageSenderUser")
        return openUser
                   ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"userId://" +
                         sender.value("user_id").toString() + "\">" + Utils::getUserShortName(sender.value("user_id").toLongLong()) + "</a>"
                   : Utils::getUserShortName(sender.value("user_id").toLongLong());

    auto chat = TdApi::getInstance().getChat(message.value("chat_id").toLongLong());

    return openUser ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"chatId://" +
                          sender.value("chat_id").toString() + "\">" + chat.value("title").toString() + "</a>"
                    : chat.value("title").toString();
}

QString getUserFullName(qint64 userId) noexcept
{
    auto user = TdApi::getInstance().getUser(userId);

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
            return QObject::tr("HiddenName");
        }
    }

    return QString();
}

QString getCallContent(const QVariantMap &sender, const QVariantMap &content) noexcept
{
    auto isVideo = content.value("is_video").toBool();
    auto discardReason = content.value("discard_reason").toMap();
    auto isMissed = discardReason.value("@type").toByteArray() == "callDiscardReasonMissed";
    auto isBusy = discardReason.value("@type").toByteArray() == "callDiscardReasonDeclined";

    if (isMeUser(sender.value("user_id").toInt()))
    {
        if (isMissed)
        {
            if (isVideo)
            {
                return QObject::tr("CallMessageVideoOutgoingMissed");
            }

            return QObject::tr("CallMessageOutgoingMissed");
        }

        if (isVideo)
        {
            return QObject::tr("CallMessageVideoOutgoing");
        }

        return QObject::tr("CallMessageOutgoing");
    }

    if (isMissed)
    {
        if (isVideo)
        {
            return QObject::tr("CallMessageVideoIncomingMissed");
        }

        return QObject::tr("CallMessageIncomingMissed");
    }
    else if (isBusy)
    {
        if (isVideo)
        {
            return QObject::tr("CallMessageVideoIncomingDeclined");
        }

        return QObject::tr("CallMessageIncomingDeclined");
    }

    if (isVideo)
    {
        return QObject::tr("CallMessageVideoIncoming");
    }

    return QObject::tr("CallMessageIncoming");
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

bool isUserBlocked(qint64 userId) noexcept
{
    auto fullInfo = TdApi::getInstance().getUserFullInfo(userId);

    if (fullInfo.isEmpty())
        return false;

    return fullInfo.value("is_blocked").toBool();
}

bool isDeletedUser(qint64 userId)
{
    auto user = TdApi::getInstance().getUser(userId);

    auto userType = user.value("type").toMap();
    return userType.value("@type").toByteArray() == "userTypeDeleted";
}

void appendDuration(int count, QChar &&order, QString &outString)
{
    outString.append(QString::number(count));
    outString.append(order);
}
}  // namespace detail

Utils::Utils(QObject *parent)
    : QObject(parent)
{
}

QVariantMap Utils::getChatPosition(qint64 chatId, const QVariantMap &chatList)
{
    auto chat = TdApi::getInstance().getChat(chatId);
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
        }
        case fnv::hash("chatListArchive"): {
            if (auto it = std::ranges::find_if(positions,
                                               [](const auto &position) {
                                                   auto list = position.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListArchive";
                                               });
                it != positions.end())
                return it->toMap();
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
        }
    }

    return {};
}

bool Utils::isChatPinned(qint64 chatId, const QVariantMap &chatList)
{
    auto position = getChatPosition(chatId, chatList);

    return position.value("is_pinned").toBool();
}

qint64 Utils::getChatOrder(qint64 chatId, const QVariantMap &chatList)
{
    auto position = getChatPosition(chatId, chatList);

    return position.value("order").toLongLong();
}

bool Utils::chatListEquals(const QVariantMap &list1, const QVariantMap &list2)
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

QString Utils::getChatTitle(qint64 chatId, bool showSavedMessages)
{
    auto chat = TdApi::getInstance().getChat(chatId);

    if (detail::isMeChat(chat) && showSavedMessages)
        return QObject::tr("SavedMessages");

    auto title = chat.value("title").toString();

    return title.isEmpty() ? QObject::tr("HiddenName") : title;
}

bool Utils::isChatMuted(qint64 chatId)
{
    return getChatMuteFor(chatId) > 0;
}

int Utils::getChatMuteFor(qint64 chatId)
{
    auto chat = TdApi::getInstance().getChat(chatId);

    auto notificationSettings = chat.value("notification_settings").toMap();

    return notificationSettings.value("mute_for").toInt();
}

QString Utils::getServiceMessageContent(const QVariantMap &message, bool openUser)
{
    auto ttl = message.value("ttl").toInt();
    auto sender = message.value("sender_id").toMap();
    auto content = message.value("content").toMap();
    auto isOutgoing = message.value("is_outgoing").toBool();

    auto chat = TdApi::getInstance().getChat(message.value("chat_id").toLongLong());

    auto isChannel = detail::isChannelChat(chat);

    auto author = detail::getMessageAuthor(message, openUser);

    auto getUserName = [openUser](qlonglong userId) {
        return openUser ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"userId://" + QString::number(userId) +
                              "\">" + Utils::getUserShortName(userId) + "</a>"
                        : Utils::getUserShortName(userId);
    };

    auto contentType = content.value("@type").toByteArray();
    if (ttl > 0)
    {
        switch (fnv::hashRuntime(contentType.constData()))
        {
            case fnv::hash("messagePhoto"): {
                if (isOutgoing)
                {
                    return QObject::tr("ActionYouSendTTLPhoto");
                }

                return QObject::tr("ActionSendTTLPhoto").arg(author);
            }
            case fnv::hash("messageVideo"): {
                if (isOutgoing)
                {
                    return QObject::tr("ActionYouSendTTLVideo");
                }

                return QObject::tr("ActionSendTTLVideo").arg(author);
            }
        }
    }

    switch (fnv::hashRuntime(contentType.constData()))
    {
        case fnv::hash("messageExpiredPhoto"): {
            return QObject::tr("AttachPhotoExpired");
        }
        case fnv::hash("messageExpiredVideo"): {
            return QObject::tr("AttachVideoExpired");
        }
        case fnv::hash("messageBasicGroupChatCreate"): {
            if (isOutgoing)
            {
                return QObject::tr("ActionYouCreateGroup");
            }

            return QObject::tr("ActionCreateGroup").arg(author);
        }
        case fnv::hash("messageSupergroupChatCreate"): {
            if (isChannel)
            {
                return QObject::tr("ActionCreateChannel");
            }

            return QObject::tr("ActionCreateMega");
        }
        case fnv::hash("messageChatChangeTitle"): {
            auto title = content.value("title").toString();

            if (isChannel)
            {
                return QObject::tr("ActionChannelChangedTitle").arg(title);
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouChangedTitle").arg(title);
            }

            return QObject::tr("ActionChangedTitle").arg(author).arg(title);
        }
        case fnv::hash("messageChatChangePhoto"): {
            if (isChannel)
            {
                return QObject::tr("ActionChannelChangedPhoto");
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouChangedPhoto");
            }

            return QObject::tr("ActionChangedPhoto").arg(author);
        }
        case fnv::hash("messageChatDeletePhoto"): {
            if (isChannel)
            {
                return QObject::tr("ActionChannelRemovedPhoto");
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouRemovedPhoto");
            }

            return QObject::tr("ActionRemovedPhoto").arg(author);
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
                        return QObject::tr("ChannelJoined");
                    }

                    if (detail::isSupergroup(chat) && !isChannel)
                    {
                        if (detail::isMeUser(memberUserId))
                        {
                            return QObject::tr("ChannelMegaJoined");
                        }

                        return QObject::tr("ActionAddUserSelfMega").arg(author);
                    }

                    if (isOutgoing)
                    {
                        return QObject::tr("ActionAddUserSelfYou");
                    }

                    return QObject::tr("ActionAddUserSelf").arg(author);
                }

                if (isOutgoing)
                {
                    return QObject::tr("ActionYouAddUser").arg(getUserName(memberUserId));
                }

                if (detail::isMeUser(memberUserId))
                {
                    if (detail::isSupergroup(chat))
                    {
                        if (!isChannel)
                        {
                            return QObject::tr("MegaAddedBy").arg(author);
                        }

                        return QObject::tr("ChannelAddedBy").arg(author);
                    }

                    return QObject::tr("ActionAddUserYou").arg(author);
                }

                return QObject::tr("ActionAddUser").arg(author).arg(getUserName(memberUserId));
            }

            QStringList result;
            for (const auto &userId : content.value("member_user_ids").toList())
            {
                result << getUserName(userId.toLongLong());
            }

            auto users = result.join(", ");

            if (isOutgoing)
            {
                return QObject::tr("ActionYouAddUser").arg(users);
            }

            return QObject::tr("ActionAddUser").arg(author).arg(users);
        }
        case fnv::hash("messageChatJoinByLink"): {
            if (isOutgoing)
            {
                return QObject::tr("ActionInviteYou");
            }

            return QObject::tr("ActionInviteUser").arg(author);
        }
        case fnv::hash("messageChatDeleteMember"): {
            if (content.value("user_id").toLongLong() == sender.value("user_id").toLongLong())
            {
                if (isOutgoing)
                {
                    return QObject::tr("ActionYouLeftUser");
                }

                return QObject::tr("ActionLeftUser").arg(author);
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouKickUser").arg(getUserName(content.value("user_id").toLongLong()));
            }
            else if (detail::isMeUser(content.value("user_id").toLongLong()))
            {
                return QObject::tr("ActionKickUserYou").arg(author);
            }

            return QObject::tr("ActionKickUser").arg(author).arg(getUserName(content.value("user_id").toLongLong()));
        }
        case fnv::hash("messageChatUpgradeTo"): {
            return QObject::tr("ActionMigrateFromGroup");
        }
        case fnv::hash("messageChatUpgradeFrom"): {
            return QObject::tr("ActionMigrateFromGroup");
        }
        case fnv::hash("messagePinMessage"): {
            return QObject::tr("ActionPinned").arg(author);
        }
        case fnv::hash("messageScreenshotTaken"): {
            if (isOutgoing)
            {
                return QObject::tr("ActionTakeScreenshootYou");
            }

            return QObject::tr("ActionTakeScreenshoot").arg(author);
        }
        case fnv::hash("messageChatSetTtl"): {
            auto ttlString = detail::getTTLString(content.value("ttl").toInt());

            if (content.value("ttl").toInt() <= 0)
            {
                if (isOutgoing)
                    return QObject::tr("MessageLifetimeYouRemoved");

                return QObject::tr("MessageLifetimeRemoved").arg(getUserName(sender.value("user_id").toLongLong()));
            }

            if (isOutgoing)
                return QObject::tr("MessageLifetimeChangedOutgoing").arg(ttlString);

            return QObject::tr("MessageLifetimeChanged").arg(getUserName(sender.value("user_id").toLongLong())).arg(ttlString);
        }
        case fnv::hash("messageCustomServiceAction"): {
            return content.value("text").toString();
        }
        case fnv::hash("messageContactRegistered"): {
            return QObject::tr("NotificationContactJoined").arg(getUserName(sender.value("user_id").toLongLong()));
        }
        case fnv::hash("messageWebsiteConnected"): {
            return QObject::tr("ActionBotAllowed");
        }
        case fnv::hash("messageUnsupported"): {
            return QObject::tr("UnsupportedMedia");
        }
    }

    return QObject::tr("UnsupportedMedia");
}

bool Utils::isServiceMessage(const QVariantMap &message)
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

QString Utils::getUserShortName(qint64 userId) noexcept
{
    auto user = TdApi::getInstance().getUser(userId);

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
        return QObject::tr("HiddenName");

    return QString();
}

QString Utils::getTitle(const QVariantMap &message) noexcept
{
    auto sender = message.value("sender_id").toMap();

    auto senderType = sender.value("@type").toByteArray();
    if (senderType == "messageSenderUser")
    {
        return detail::getUserFullName(sender.value("user_id").toLongLong());
    }

    if (senderType == "messageSenderChat")
        return TdApi::getInstance().getChat(sender.value("chat_id").toLongLong()).value("title").toString();

    return QString();
}

QString Utils::getMessageDate(const QVariantMap &message) noexcept
{
    auto date = QDateTime::fromMSecsSinceEpoch(message.value("date").toLongLong() * 1000);

    auto days = date.daysTo(QDateTime::currentDateTime());

    if (days == 0)
        return QLocale::system().toString(date.time(), QLocale::ShortFormat);
    else if (days < 2)
        return QObject::tr("Yesterday");
    else if (days < 7)
        return date.toString("dddd");

    return QLocale::system().toString(date.date(), QLocale::ShortFormat);
}

QString Utils::getContent(const QVariantMap &message) noexcept
{
    auto content = message.value("content").toMap();
    auto sender = message.value("sender_id").toMap();

    static auto textOneLine = [](const QString &text) {
        auto result = text;
        result.replace("\n", " ");
        result.replace("\r", " ");
        return result;
    };

    QString caption;
    if (!content.value("caption").toMap().value("text").toString().isEmpty())
    {
        caption += ": " % textOneLine(content.value("caption").toMap().value("text").toString());
    }

    if (message.value("ttl").toInt() > 0)
        return Utils::getServiceMessageContent(message);

    auto contentType = content.value("@type").toByteArray();
    switch (fnv::hashRuntime(contentType.constData()))
    {
        case fnv::hash("messageAnimation"): {
            return QObject::tr("AttachGif") % caption;
        }
        case fnv::hash("messageAudio"): {
            auto audio = content.value("audio").toMap();
            auto title = detail::getAudioTitle(audio).isEmpty() ? QObject::tr("AttachMusic") : detail::getAudioTitle(audio);

            return title % caption;
        }
        case fnv::hash("messageBasicGroupChatCreate"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageCall"): {
            auto text = detail::getCallContent(sender, content);

            auto duration = content.value("duration").toInt();
            if (duration > 0)
            {
                return QObject::tr("CallMessageWithDuration").arg(text).arg(detail::formatCallDuration(duration));
            }

            return text;
        }
        case fnv::hash("messageChatAddMembers"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatChangePhoto"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatChangeTitle"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatDeleteMember"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatDeletePhoto"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatJoinByLink"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatSetTtl"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatUpgradeFrom"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageChatUpgradeTo"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageContact"): {
            return QObject::tr("AttachContact") % caption;
        }
        case fnv::hash("messageContactRegistered"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageCustomServiceAction"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageDocument"): {
            auto document = content.value("document").toMap();
            auto fileName = document.value("file_name").toString();
            if (!fileName.isEmpty())
            {
                return fileName % caption;
            }

            return QObject::tr("AttachDocument") % caption;
        }
        case fnv::hash("messageExpiredPhoto"): {
            return QObject::tr("AttachPhoto") % caption;
        }
        case fnv::hash("messageExpiredVideo"): {
            return QObject::tr("AttachVideo") % caption;
        }
        case fnv::hash("messageGame"): {
            return QObject::tr("AttachGame") % caption;
        }
        case fnv::hash("messageGameScore"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageInvoice"): {
            auto title = content.value("title").toString();
            return title % caption;
        }
        case fnv::hash("messageLocation"): {
            return QObject::tr("AttachLocation").arg(caption);
        }
        case fnv::hash("messagePassportDataReceived"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messagePassportDataSent"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messagePaymentSuccessful"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messagePaymentSuccessfulBot"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messagePhoto"): {
            return QObject::tr("AttachPhoto") % caption;
        }
        case fnv::hash("messagePoll"): {
            auto poll = content.value("poll").toMap();
            auto question = poll.value("question").toString();

            return QString::fromUtf8("\xf0\x9f\x93\x8a\x20") % question;
        }
        case fnv::hash("messagePinMessage"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageScreenshotTaken"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageSticker"): {
            auto sticker = content.value("sticker").toMap();
            auto emoji = sticker.value("emoji").toString();

            return QObject::tr("AttachSticker") % ": " % emoji;
        }
        case fnv::hash("messageSupergroupChatCreate"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageText"): {
            return textOneLine(content.value("text").toMap().value("text").toString());
        }
        case fnv::hash("messageUnsupported"): {
            return Utils::getServiceMessageContent(message);
        }
        case fnv::hash("messageVenue"): {
            return QObject::tr("AttachLocation") % caption;
        }
        case fnv::hash("messageVideo"): {
            return QObject::tr("AttachVideo") % caption;
        }
        case fnv::hash("messageVideoNote"): {
            return QObject::tr("AttachRound") % caption;
        }
        case fnv::hash("messageVoiceNote"): {
            return QObject::tr("AttachAudio") % caption;
        }
        case fnv::hash("messageWebsiteConnected"): {
            return Utils::getServiceMessageContent(message);
        }
    }

    return QObject::tr("UnsupportedAttachment");
}

bool Utils::isChatUnread(qint64 chatId) noexcept
{
    auto chat = TdApi::getInstance().getChat(chatId);

    auto isMarkedAsUnread = chat.value("is_marked_as_unread").toBool();
    auto unreadCount = chat.value("unread_count").toInt();

    return isMarkedAsUnread || unreadCount > 0;
}

QString Utils::getMessageSenderName(const QVariantMap &message) noexcept
{
    if (isServiceMessage(message))
        return QString();

    auto sender = message.value("sender_id").toMap();

    auto chat = TdApi::getInstance().getChat(message.value("chat_id").toLongLong());

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
                    if (detail::isMeUser(sender.value("user_id").toLongLong()))
                        return QObject::tr("FromYou");

                    return getUserShortName(sender.value("user_id").toLongLong());
                }
                case fnv::hash("messageSenderChat"): {
                    return getChatTitle(chat.value("id").toLongLong());
                }
            }
        }
    }

    return QString();
}

// TODO(strawberry):
QString Utils::getFormattedText(const QVariantMap &formattedText) noexcept
{
    auto text = formattedText.value("text").toString();
    auto entities = formattedText.value("entities").toList();

    QFont font;
    font.setPixelSize(23);
    font.setWeight(QFont::Light);

    QScopedPointer<QTextDocument> doc(new QTextDocument);
    doc->setDefaultFont(font);
    doc->setPlainText(text);

    for (QTextCursor cursor(doc.data()); const auto &entity : entities)
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
            case fnv::hash("textEntityTypeMentionName"):
            case fnv::hash("textEntityTypeMention"): {
                QTextCharFormat format;
                format.setAnchor(true);
                format.setAnchorHref(entityText);

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

bool Utils::copyToClipboard(const QVariantMap &content) const noexcept
{
    auto contentType = content.value("@type").toByteArray();
    if (contentType != "messageText")
        return false;

    auto clipboard = QApplication::clipboard();

    clipboard->setText(content.value("text").toMap().value("text").toString());

    return true;
}

QImage Utils::getThumb(const QVariantMap &thumbnail) const noexcept
{
    QByteArray thumb("data:image/jpeg;base64, ");
    thumb += thumbnail.value("data").toByteArray();

    QBuffer buf;
    buf.setData(thumb);

    QImageReader reader(&buf);

    return reader.read();
}

bool Utils::isMessageUnread(qint64 chatId, const QVariantMap &message) noexcept
{
    auto chat = TdApi::getInstance().getChat(chatId);

    auto lastReadInboxMessageId = chat.value("last_read_inbox_message_id").toLongLong();
    auto lastReadOutboxMessageId = chat.value("last_read_outbox_message_id").toLongLong();

    auto id = message.value("id").toLongLong();
    auto isOutgoing = message.value("is_outgoing").toBool();
    if (isOutgoing && detail::isMeChat(chat))
        return false;

    return isOutgoing ? id > lastReadOutboxMessageId : id > lastReadInboxMessageId;
}

QString Utils::getViews(int views) const noexcept
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

QString Utils::getFileSize(const QVariantMap &file) noexcept
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

QString Utils::formatTime(int totalSeconds) const noexcept
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
