#include "Utils.hpp"

#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "TdApi.hpp"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QDateTime>
#include <QDebug>
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
    const auto sender = message.value("sender_id").toMap();
    const auto senderType = sender.value("@type").toByteArray();
    const QString linkStyle = "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"";
    const QString linkClose = "</a>";

    if (senderType == "messageSenderUser")
    {
        const auto userIdStr = sender.value("user_id").toString();
        const auto userName = Utils::getUserShortName(sender.value("user_id").toLongLong(), store, locale);

        if (openUser)
        {
            return linkStyle + "userId://" + userIdStr + "\">" + userName + linkClose;
        }
        return userName;
    }
    else
    {
        const auto chat = store->getChat(message.value("chat_id").toLongLong());
        const auto chatTitle = chat.value("title").toString();

        if (openUser)
        {
            return linkStyle + "chatId://" + sender.value("chat_id").toString() + "\">" + chatTitle + linkClose;
        }
        return chatTitle;
    }
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
    const bool isVideo = content.value("is_video").toBool();
    const auto discardReason = content.value("discard_reason").toMap();
    const auto discardType = discardReason.value("@type").toByteArray();
    const bool isMissed = discardType == "callDiscardReasonMissed";
    const bool isBusy = discardType == "callDiscardReasonDeclined";
    const bool isOutgoing = isMeUser(sender.value("user_id").toInt(), store);

    if (isMissed)
    {
        if (isOutgoing)
        {
            return isVideo ? locale->getString("CallMessageVideoOutgoingMissed") : locale->getString("CallMessageOutgoingMissed");
        }
        return isVideo ? locale->getString("CallMessageVideoIncomingMissed") : locale->getString("CallMessageIncomingMissed");
    }

    if (isBusy)
    {
        return isVideo ? locale->getString("CallMessageVideoIncomingDeclined") : locale->getString("CallMessageIncomingDeclined");
    }

    return isVideo ? (isOutgoing ? locale->getString("CallMessageVideoOutgoing") : locale->getString("CallMessageVideoIncoming"))
                   : (isOutgoing ? locale->getString("CallMessageOutgoing") : locale->getString("CallMessageIncoming"));
}

QString getAudioTitle(const QVariantMap &audio) noexcept
{
    const auto fileName = audio.value("file_name").toString();
    const auto title = audio.value("title").toString().trimmed();
    const auto performer = audio.value("performer").toString().trimmed();

    if (title.isEmpty() && performer.isEmpty())
        return fileName;

    const auto artist = performer.isEmpty() ? "Unknown Artist" : performer;
    const auto track = title.isEmpty() ? "Unknown Track" : title;

    return artist + " - " + track;
}

bool isUserBlocked(qint64 userId, StorageManager *store) noexcept
{
    const auto fullInfo = store->getUserFullInfo(userId);

    return !fullInfo.isEmpty() && fullInfo.value("is_blocked").toBool();
}

bool isDeletedUser(qint64 userId, StorageManager *store)
{
    const auto user = store->getUser(userId);
    const auto userType = user.value("type").toMap();

    return userType.value("@type").toByteArray() == "userTypeDeleted";
}

void appendDuration(int count, QChar &&order, QString &outString)
{
    outString.append(QString::number(count));
    outString.append(order);
}
}  // namespace detail

namespace Utils {

QVariantMap getChatPosition(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    const auto chat = store->getChat(chatId);
    const auto positions = chat.value("positions").toList();
    const auto chatListType = chatList.value("@type").toString().toStdString();

    const auto findPosition = [&](const QByteArray &type, const std::function<bool(const QVariantMap &)> &filter = nullptr) -> QVariantMap {
        for (const auto &position : positions)
        {
            const auto list = position.toMap().value("list").toMap();
            if (list.value("@type").toByteArray() == type && (!filter || filter(list)))
            {
                return position.toMap();
            }
        }
        return {};
    };

    std::unordered_map<std::string, std::function<QVariantMap()>> handler = {
        {"chatListMain", [&]() { return findPosition("chatListMain"); }},
        {"chatListArchive", [&]() { return findPosition("chatListArchive"); }},
        {"chatListFilter", [&]() {
             const auto filter = [&chatList](const QVariantMap &list) {
                 return list.value("chat_filter_id").toInt() == chatList.value("chat_filter_id").toInt();
             };
             return findPosition("chatListFilter", filter);
         }}};

    if (auto it = handler.find(chatListType); it != handler.end())
    {
        return it->second();
    }

    return {};
}

bool isChatPinned(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    const auto position = getChatPosition(chatId, chatList, store);
    return position.value("is_pinned").toBool();
}

qint64 getChatOrder(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    const auto position = getChatPosition(chatId, chatList, store);
    return position.value("order").toLongLong();
}

bool chatListEquals(const QVariantMap &list1, const QVariantMap &list2)
{
    const auto type1 = list1.value("@type").toByteArray();
    const auto type2 = list2.value("@type").toByteArray();

    if (type1 != type2)
        return false;

    switch (fnv::hashRuntime(type1.constData()))
    {
        case fnv::hash("chatListMain"):
        case fnv::hash("chatListArchive"):
            return true;
        case fnv::hash("chatListFilter"):
            return list1.value("chat_filter_id") == list2.value("chat_filter_id");
        default:
            return false;
    }
}

QString getChatTitle(qint64 chatId, StorageManager *store, Locale *locale, bool showSavedMessages)
{
    const auto chat = store->getChat(chatId);

    if (detail::isMeChat(chat, store) && showSavedMessages)
    {
        return locale->getString("SavedMessages");
    }

    const QString title = chat.value("title").toString().trimmed();

    return !title.isEmpty() ? title : locale->getString("HiddenName");
}

bool isChatMuted(qint64 chatId, StorageManager *store)
{
    return getChatMuteFor(chatId, store) > 0;
}

int getChatMuteFor(qint64 chatId, StorageManager *store)
{
    const auto chat = store->getChat(chatId);
    const auto notificationSettings = chat.value("notification_settings").toMap();

    return notificationSettings.value("mute_for").toInt();
}

QString getServiceMessageContent(const QVariantMap &message, StorageManager *store, Locale *locale, bool openUser)
{
    const auto sender = message.value("sender_id").toMap();
    const auto content = message.value("content").toMap();
    const auto isOutgoing = message.value("is_outgoing").toBool();
    const auto chat = store->getChat(message.value("chat_id").toLongLong());
    const auto isChannel = detail::isChannelChat(chat);
    const auto author = detail::getMessageAuthor(message, store, locale, openUser);

    auto getUserName = [openUser, store, locale](qlonglong userId) {
        const QString userName = Utils::getUserShortName(userId, store, locale);
        return openUser ? QString("<a style=\"text-decoration: none; font-weight: bold; color: grey\" href=\"userId://%1\">%2</a>")
                              .arg(userId)
                              .arg(userName)
                        : userName;
    };

    const auto contentType = content.value("@type").toString().toStdString();

    static const std::unordered_map<std::string, std::function<QString()>> handlers = {
        {"messagePhoto",
         [&]() {
             return isOutgoing ? locale->getString("ActionYouSendTTLPhoto")
                               : locale->getString("ActionSendTTLPhoto").replace("un1", author);
         }},
        {"messageVideo",
         [&]() {
             return isOutgoing ? locale->getString("ActionYouSendTTLVideo")
                               : locale->getString("ActionSendTTLVideo").replace("un1", author);
         }},
        {"messageExpiredPhoto", [&]() { return locale->getString("AttachPhotoExpired"); }},
        {"messageExpiredVideo", [&]() { return locale->getString("AttachVideoExpired"); }},
        {"messageBasicGroupChatCreate",
         [&]() {
             return isOutgoing ? locale->getString("ActionYouCreateGroup") : locale->getString("ActionCreateGroup").replace("un1", author);
         }},
        {"messageSupergroupChatCreate",
         [&]() { return isChannel ? locale->getString("ActionCreateChannel") : locale->getString("ActionCreateMega"); }},
        {"messageChatChangeTitle",
         [&]() {
             const QString title = content.value("title").toString();
             return isChannel    ? locale->getString("ActionChannelChangedTitle").replace("un2", title)
                    : isOutgoing ? locale->getString("ActionYouChangedTitle").replace("un2", title)
                                 : locale->getString("ActionChangedTitle").replace("un1", author).replace("un2", title);
         }},
        {"messageChatChangePhoto",
         [&]() {
             return isChannel    ? locale->getString("ActionChannelChangedPhoto")
                    : isOutgoing ? locale->getString("ActionYouChangedPhoto")
                                 : locale->getString("ActionChangedPhoto").replace("un1", author);
         }},
        {"messageChatDeletePhoto",
         [&]() {
             return isChannel    ? locale->getString("ActionChannelRemovedPhoto")
                    : isOutgoing ? locale->getString("ActionYouRemovedPhoto")
                                 : locale->getString("ActionRemovedPhoto").replace("un1", author);
         }},
        {"messageChatAddMembers",
         [&]() {
             const QList<QVariant> memberIds = content.value("member_user_ids").toList();
             const int memberCount = memberIds.size();
             if (memberCount == 1)
             {
                 const qint64 memberUserId = memberIds.first().toLongLong();
                 if (sender.value("user_id") == memberUserId)
                 {
                     if (detail::isSupergroup(chat))
                     {
                         if (isChannel)
                         {
                             return locale->getString("ChannelJoined");
                         }
                         else if (detail::isMeUser(memberUserId, store))
                         {
                             return locale->getString("ChannelMegaJoined");
                         }
                         else if (isOutgoing)
                         {
                             return locale->getString("ActionAddUserSelfYou");
                         }
                         else
                         {
                             return locale->getString("ActionAddUserSelf").replace("un1", author);
                         }
                     }
                     else if (isOutgoing)
                     {
                         return locale->getString("ActionAddUserSelfYou");
                     }
                     else
                     {
                         return locale->getString("ActionAddUserSelf").replace("un1", author);
                     }
                 }
                 else
                 {
                     if (isOutgoing)
                     {
                         return locale->getString("ActionYouAddUser").replace("un2", getUserName(memberUserId));
                     }
                     else if (detail::isMeUser(memberUserId, store))
                     {
                         if (detail::isSupergroup(chat))
                         {
                             return isChannel ? locale->getString("ChannelAddedBy").replace("un1", author)
                                              : locale->getString("MegaAddedBy").replace("un1", author);
                         }
                         else
                         {
                             return locale->getString("ActionAddUserYou").replace("un1", author);
                         }
                     }
                     else
                     {
                         return locale->getString("ActionAddUser").replace("un1", author).replace("un2", getUserName(memberUserId));
                     }
                 }
             }
             else
             {
                 QStringList members;
                 members.reserve(memberCount);
                 for (const auto &userId : memberIds)
                 {
                     members << getUserName(userId.toLongLong());
                 }
                 const QString users = members.join(", ");

                 return isOutgoing ? locale->getString("ActionYouAddUser").arg(users)
                                   : locale->getString("ActionAddUser").replace("un1", author).replace("un2", users);
             }
         }},
        {"messageChatJoinByLink",
         [&]() {
             return isOutgoing ? locale->getString("ActionInviteYou") : locale->getString("ActionInviteUser").replace("un1", author);
         }},
        {"messageChatDeleteMember",
         [&]() {
             const qint64 userId = content.value("user_id").toLongLong();
             if (userId == sender.value("user_id").toLongLong())
             {
                 return isOutgoing ? locale->getString("ActionYouLeftUser") : locale->getString("ActionLeftUser").replace("un1", author);
             }
             else if (isOutgoing)
             {
                 return locale->getString("ActionYouKickUser").replace("un2", getUserName(userId));
             }
             else if (detail::isMeUser(userId, store))
             {
                 return locale->getString("ActionKickUserYou").replace("un1", author);
             }
             else
             {
                 return locale->getString("ActionKickUser").replace("un1", author).replace("un2", getUserName(userId));
             }
         }},
        {"messageChatUpgradeTo", [&]() { return locale->getString("ActionMigrateFromGroup"); }},
        {"messageChatUpgradeFrom", [&]() { return locale->getString("ActionMigrateFromGroup"); }},
        {"messagePinMessage", [&]() { return locale->getString("ActionPinned").replace("un1", author); }},
        {"messageScreenshotTaken",
         [&]() {
             return isOutgoing ? locale->getString("ActionTakeScreenshootYou")
                               : locale->getString("ActionTakeScreenshoot").replace("un1", author);
         }},
        {"messageChatSetTtl",
         [&]() {
             const int ttlValue = content.value("ttl").toInt();
             const QString ttlString = locale->formatTtl(ttlValue);

             if (ttlValue <= 0)
             {
                 return isOutgoing ? locale->getString("MessageLifetimeYouRemoved")
                                   : locale->getString("MessageLifetimeRemoved").arg(getUserName(sender.value("user_id").toLongLong()));
             }
             else
             {
                 return isOutgoing ? locale->getString("MessageLifetimeChangedOutgoing").arg(ttlString)
                                   : locale->getString("MessageLifetimeChanged")
                                         .arg(getUserName(sender.value("user_id").toLongLong()))
                                         .arg(ttlString);
             }
         }},
        {"messageCustomServiceAction", [&]() { return content.value("text").toString(); }},
        {"messageContactRegistered",
         [&]() { return locale->getString("NotificationContactJoined").arg(getUserName(sender.value("user_id").toLongLong())); }},
        {"messageWebsiteConnected", [&]() { return locale->getString("ActionBotAllowed"); }},
        {"messageUnsupported", [&]() { return locale->getString("UnsupportedMedia"); }}};

    if (auto it = handlers.find(contentType); it != handlers.end())
    {
        return it->second();
    }
    else
    {
        return locale->getString("UnsupportedMedia");
    }
}

bool isServiceMessage(const QVariantMap &message)
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

QString getUserShortName(qint64 userId, StorageManager *store, Locale *locale) noexcept
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

QString getTitle(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept
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

QString getMessageDate(const QVariantMap &message, Locale *locale) noexcept
{
    auto date = QDateTime::fromMSecsSinceEpoch(message.value("date").toLongLong() * 1000);

    auto days = date.daysTo(QDateTime::currentDateTime());

    if (days == 0)
        return date.toString(locale->getString("formatterDay12H"));
    else if (days < 7)
        return date.toString(locale->getString("formatterWeek"));

    return date.toString(locale->getString("formatterYear"));
}

QString getContent(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept
{
    auto content = message.value("content").toMap();
    auto sender = message.value("sender_id").toMap();

    auto textOneLine = [](QString text) { return text.replace("\n", " ").replace("\r", " "); };

    QString caption;
    if (!content.value("caption").toMap().value("text").toString().isEmpty())
        caption.append(": ").append(textOneLine(content.value("caption").toMap().value("text").toString()));

    if (message.value("ttl").toInt() > 0)
        return getServiceMessageContent(message, store, locale);

    auto contentType = content.value("@type").toByteArray();
    switch (fnv::hashRuntime(contentType.constData()))
    {
        case fnv::hash("messageAnimation"): {
            return locale->getString("AttachGif").append(caption);
        }
        case fnv::hash("messageAudio"): {
            auto audio = content.value("audio").toMap();
            auto title = detail::getAudioTitle(audio).isEmpty() ? locale->getString("AttachMusic") : detail::getAudioTitle(audio);

            return title.append(caption);
        }
        case fnv::hash("messageBasicGroupChatCreate"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageCall"): {
            auto text = detail::getCallContent(sender, content, store, locale);

            auto duration = content.value("duration").toInt();
            if (duration > 0)
            {
                return locale->getString("CallMessageWithDuration").arg(text).arg(locale->formatCallDuration(duration));
            }

            return text;
        }
        case fnv::hash("messageChatAddMembers"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatChangePhoto"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatChangeTitle"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatDeleteMember"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatDeletePhoto"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatJoinByLink"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatSetTtl"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatUpgradeFrom"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageChatUpgradeTo"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageContact"): {
            return locale->getString("AttachContact").append(caption);
        }
        case fnv::hash("messageContactRegistered"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageCustomServiceAction"): {
            return getServiceMessageContent(message, store, locale);
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
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageInvoice"): {
            auto title = content.value("title").toString();
            return title.append(caption);
        }
        case fnv::hash("messageLocation"): {
            return locale->getString("AttachLocation").arg(caption);
        }
        case fnv::hash("messagePassportDataReceived"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messagePassportDataSent"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messagePaymentSuccessful"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messagePaymentSuccessfulBot"): {
            return getServiceMessageContent(message, store, locale);
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
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageScreenshotTaken"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageSticker"): {
            auto sticker = content.value("sticker").toMap();
            auto emoji = sticker.value("emoji").toString();

            return locale->getString("AttachSticker").append(": ").append(emoji);
        }
        case fnv::hash("messageSupergroupChatCreate"): {
            return getServiceMessageContent(message, store, locale);
        }
        case fnv::hash("messageText"): {
            return textOneLine(content.value("text").toMap().value("text").toString());
        }
        case fnv::hash("messageUnsupported"): {
            return getServiceMessageContent(message, store, locale);
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
            return getServiceMessageContent(message, store, locale);
        }
    }

    return locale->getString("UnsupportedAttachment");
}

bool isChatUnread(qint64 chatId, StorageManager *store) noexcept
{
    auto chat = store->getChat(chatId);

    auto isMarkedAsUnread = chat.value("is_marked_as_unread").toBool();
    auto unreadCount = chat.value("unread_count").toInt();

    return isMarkedAsUnread || unreadCount > 0;
}

QString getMessageSenderName(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept
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
                    if (detail::isMeUser(sender.value("user_id").toLongLong(), store))
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

QString getFormattedText(const QVariantMap &formattedText, StorageManager *store, Locale *locale, const QVariantMap &options) noexcept
{
    static const std::unordered_map<std::string, std::function<void(QTextCharFormat &, const QString &, const QVariantMap &)>> formatters =
        {{"textEntityTypeBold", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontWeight(QFont::Bold); }},
         {"textEntityTypeBotCommand",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("botCommand:" + entityText);
          }},
         {"textEntityTypeEmailAddress",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("mailto:" + entityText);
          }},
         {"textEntityTypeItalic", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontItalic(true); }},
         {"textEntityTypeMentionName",
          [store, locale](QTextCharFormat &format, const QString &, const QVariantMap &type) {
              auto userId = type.value("user_id").toLongLong();
              auto title = detail::getUserFullName(userId, store, locale);
              format.setAnchor(true);
              format.setAnchorHref("userId:" + title);
          }},
         {"textEntityTypeMention",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("username:" + entityText);
          }},
         {"textEntityTypePhoneNumber",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("tel:" + entityText);
          }},
         {"textEntityTypeCode", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypePre", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypePreCode", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypeStrikethrough",
          [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontStrikeOut(true); }},
         {"textEntityTypeTextUrl",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &type) {
              QString url = type.value("url").toString();
              if (url.isEmpty())
              {
                  url = entityText;
              }
              format.setAnchor(true);
              format.setAnchorHref(url);
              format.setFontUnderline(true);
          }},
         {"textEntityTypeUrl",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref(entityText);
              format.setFontUnderline(true);
          }},
         {"textEntityTypeUnderline", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontUnderline(true); }}};

    const auto text = formattedText.value("text").toString();
    const auto entities = formattedText.value("entities").toList();

    QFont font;
    font.setPixelSize(23);
    font.setWeight(QFont::Light);

    QScopedPointer<QTextDocument> doc(new QTextDocument);
    doc->setDefaultFont(font);
    doc->setPlainText(text);

    QTextCursor cursor(doc.data());

    for (const auto &entityVariant : entities)
    {
        const auto entity = entityVariant.toMap();
        const auto offset = entity.value("offset").toInt();
        const auto length = entity.value("length").toInt();
        const auto type = entity.value("type").toMap();
        const auto entityType = type.value("@type").toString().toStdString();
        const auto entityText = text.mid(offset, length);

        cursor.setPosition(offset);
        cursor.setPosition(offset + length, QTextCursor::KeepAnchor);

        QTextCharFormat format;

        if (auto it = formatters.find(entityType); it != formatters.end())
        {
            it->second(format, entityText, type);
            cursor.mergeCharFormat(format);
        }
    }

    // Return the formatted text as HTML
    return doc->toHtml();
}

void copyToClipboard(const QVariantMap &content) noexcept
{
    auto contentType = content.value("@type").toByteArray();
    if (contentType != "messageText")
        return;

    auto clipboard = QApplication::clipboard();

    clipboard->setText(content.value("text").toMap().value("text").toString());
}

QImage getThumb(const QVariantMap &thumbnail) noexcept
{
    QByteArray thumb("data:image/jpeg;base64, ");
    thumb += thumbnail.value("data").toByteArray();

    QBuffer buf;
    buf.setData(thumb);

    QImageReader reader(&buf);

    return reader.read();
}

bool isMessageUnread(qint64 chatId, const QVariantMap &message, StorageManager *store) noexcept
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

QString getViews(int views) noexcept
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

QString getFileSize(const QVariantMap &file) noexcept
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

QString formatTime(int totalSeconds) noexcept
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

}  // namespace Utils
