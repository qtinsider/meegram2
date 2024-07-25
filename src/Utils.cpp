#include "Utils.hpp"

#include "Chat.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "Message.hpp"
#include "Serialize.hpp"
#include "StorageManager.hpp"
#include "User.hpp"
#include "UserFullInfo.hpp"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QDateTime>
#include <QImageReader>
#include <QLocale>
#include <QScopedPointer>
#include <QStringList>
#include <QStringRef>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>
#include <QTextStream>

#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace detail {

bool isBotUser(const User *user) noexcept
{
    return user->type().value("@type").toString() == QLatin1String("userTypeBot");
}

bool isMeUser(qint64 userId, StorageManager *store) noexcept
{
    return store->getMyId() == userId;
}

bool isUserOnline(const User *user) noexcept
{
    if (std::ranges::any_of(ServiceNotificationsUserIds, [user](auto id) { return id == user->id(); }))
    {
        return false;
    }

    const auto statusType = user->status().value("@type").toByteArray();
    const auto userType = user->type().value("@type").toByteArray();

    return statusType == "userStatusOnline" && userType != "userTypeBot";
}

bool isChannelChat(const Chat *chat) noexcept
{
    const auto type = chat->type();
    const auto chatType = type.value("@type").toByteArray();

    if (chatType == "chatTypeSupergroup")
    {
        return type.value("is_channel").toBool();
    }

    return false;
}

bool isSupergroup(const Chat *chat) noexcept
{
    return chat->type().value("@type").toByteArray() == "chatTypeSupergroup";
}

QString getMessageAuthor(const Message *message, StorageManager *store, Locale *locale, bool openUser) noexcept
{
    const auto sender = message->senderId();
    const auto senderType = sender.value("@type").toByteArray();
    const QString linkStyle = "<a style=\"text-decoration: none; font-weight: "
                              "bold; color: grey \" href=\"";
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
        const auto chatTitle = store->getChat(message->chatId())->title();

        if (openUser)
        {
            return linkStyle + "chatId://" + sender.value("chat_id").toString() + "\">" + chatTitle + linkClose;
        }
        return chatTitle;
    }
}

QString getUserFullName(qint64 userId, StorageManager *store, Locale *locale) noexcept
{
    const auto user = store->getUser(userId);
    const auto userType = user->type().value("@type").toByteArray();

    if (userType == "userTypeBot" || userType == "userTypeRegular")
    {
        return QString(user->firstName() + " " + user->lastName()).trimmed();
    }
    else if (userType == "userTypeDeleted" || userType == "userTypeUnknown")
    {
        return locale->getString("HiddenName");
    }

    return QString();
}

QString getCallContent(const QVariantMap &sender, const QVariantMap &content, StorageManager *store, Locale *locale) noexcept
{
    const auto isVideo = content.value("is_video").toBool();
    const auto discardReason = content.value("discard_reason").toMap();
    const auto discardType = discardReason.value("@type").toByteArray();
    const auto isMissed = discardType == "callDiscardReasonMissed";
    const auto isBusy = discardType == "callDiscardReasonDeclined";
    const auto isOutgoing = isMeUser(sender.value("user_id").toInt(), store);

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

    return !fullInfo /* && fullInfo.value("is_blocked").toBool()*/;
}

bool isDeletedUser(qint64 userId, StorageManager *store)
{
    const auto userType = store->getUser(userId)->type();

    return userType.value("@type").toByteArray() == "userTypeDeleted";
}

}  // namespace detail

struct ChatMessage
{
    QVariantMap sender;
    QVariantMap content;
    QString author;
    Chat *chatDetails;
    Locale *locale = nullptr;
    StorageManager *storageManager = nullptr;
    bool isOutgoing = false;
    bool isChannel = false;

    ChatMessage(QVariantMap sender_, QVariantMap content_, QString author_, Chat *chatDetails_ = nullptr, Locale *locale_ = nullptr,
                StorageManager *storageManager_ = nullptr, bool isOutgoing_ = false, bool isChannel_ = false)
        : sender(std::move(sender_))
        , content(std::move(content_))
        , author(std::move(author_))
        , chatDetails(chatDetails_)
        , locale(locale_)
        , storageManager(storageManager_)
        , isOutgoing(isOutgoing_)
        , isChannel(isChannel_)
    {
    }
};

namespace Utils {

bool isMeChat(const Chat *chat, StorageManager *store) noexcept
{
    const auto type = chat->type();
    const auto chatType = type.value("@type").toString();

    if (chatType == QLatin1String("chatTypeSecret") || chatType == QLatin1String("chatTypePrivate"))
    {
        return store->getMyId() == type.value("user_id").toLongLong();
    }

    return false;
}

QVariantMap getChatPosition(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    const auto positions = store->getChat(chatId)->positions();
    const auto chatListType = chatList.value("@type").toString();

    const auto findPosition = [&](const QByteArray &type, const std::function<bool(const QVariantMap &)> &folder = nullptr) -> QVariantMap {
        for (const auto &position : positions)
        {
            const auto list = position.toMap().value("list").toMap();
            if (list.value("@type").toByteArray() == type && (!folder || folder(list)))
            {
                return position.toMap();
            }
        }
        return {};
    };

    std::unordered_map<QString, std::function<QVariantMap()>> handler = {
        {"chatListMain", [&]() { return findPosition("chatListMain"); }},
        {"chatListArchive", [&]() { return findPosition("chatListArchive"); }},
        {"chatListFolder", [&]() {
             const auto folder = [&chatList](const QVariantMap &list) {
                 return list.value("chat_folder_id").toInt() == chatList.value("chat_folder_id").toInt();
             };
             return findPosition("chatListFolder", folder);
         }}};

    if (auto it = handler.find(chatListType); it != handler.end())
    {
        return it->second();
    }

    return {};
}

bool isChatPinned(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    return getChatPosition(chatId, chatList, store).value("is_pinned").toBool();
}

qint64 getChatOrder(qint64 chatId, const QVariantMap &chatList, StorageManager *store)
{
    return getChatPosition(chatId, chatList, store).value("order").toLongLong();
}

bool chatListEquals(const QVariantMap &list1, const QVariantMap &list2)
{
    const auto type1 = list1.value("@type").toString();
    const auto type2 = list2.value("@type").toString();

    if (type1 != type2)
        return false;

    static const std::unordered_map<QString, QString> typeMap = {
        {"chatListMain", "chatListMain"}, {"chatListArchive", "chatListArchive"}, {"chatListFolder", "chatListFolder"}};

    if (auto it = typeMap.find(type1); it != typeMap.end())
    {
        const QString &typeName = it->second;
        if (typeName == "chatListMain" || typeName == "chatListArchive")
        {
            return true;
        }
        else if (typeName == "chatListFolder")
        {
            return list1.value("chat_folder_id") == list2.value("chat_folder_id");
        }
    }

    return false;
}

QString getChatTitle(qint64 chatId, StorageManager *store, Locale *locale, bool showSavedMessages)
{
    const auto chat = store->getChat(chatId);

    if (isMeChat(chat, store) && showSavedMessages)
    {
        return locale->getString("SavedMessages");
    }

    const auto title = chat->title().trimmed();

    return !title.isEmpty() ? title : locale->getString("HiddenName");
}

bool isChatMuted(qint64 chatId, StorageManager *store)
{
    return getChatMuteFor(chatId, store) > 0;
}

int getChatMuteFor(qint64 chatId, StorageManager *store)
{
    const auto notificationSettings = store->getChat(chatId)->notificationSettings();

    return notificationSettings.value("mute_for").toInt();
}

QString getUserName(auto userId, StorageManager *store, Locale *locale, bool openUser)
{
    const auto userName = getUserShortName(userId, store, locale);
    if (userName.isEmpty())
    {
        return QString();
    }

    QString result;
    if (openUser)
    {
        result = QString("<a style=\"text-decoration: none; font-weight: bold; color: grey\" href=\"userId://%1\">%2</a>")
                     .arg(userId)
                     .arg(userName);
    }
    else
    {
        result = QString("<span style=\"color: grey\">%1</span>").arg(userName);
    }

    return result;
}

QString getServiceMessageContent(const Message *message, StorageManager *store, Locale *locale, bool openUser)
{
    const auto chat = store->getChat(message->chatId());

    ChatMessage messageDetail{
        message->senderId(),   message->content(),         detail::getMessageAuthor(message, store, locale, openUser), chat, locale, store,
        message->isOutgoing(), detail::isChannelChat(chat)};

    const auto contentType = messageDetail.content.value("@type").toString();

    static const std::unordered_map<QString, std::function<QString(const ChatMessage &)>> handlers = {
        {"messagePhoto",
         [](const ChatMessage &message) {
             return message.isOutgoing ? message.locale->getString("ActionYouSendTTLPhoto")
                                       : message.locale->getString("ActionSendTTLPhoto").replace("un1", message.author);
         }},
        {"messageVideo",
         [](const ChatMessage &message) {
             return message.isOutgoing ? message.locale->getString("ActionYouSendTTLVideo")
                                       : message.locale->getString("ActionSendTTLVideo").replace("un1", message.author);
         }},
        {"messageExpiredPhoto", [](const ChatMessage &message) { return message.locale->getString("AttachPhotoExpired"); }},
        {"messageExpiredVideo", [](const ChatMessage &message) { return message.locale->getString("AttachVideoExpired"); }},
        {"messageBasicGroupChatCreate",
         [](const ChatMessage &message) {
             return message.isOutgoing ? message.locale->getString("ActionYouCreateGroup")
                                       : message.locale->getString("ActionCreateGroup").replace("un1", message.author);
         }},
        {"messageSupergroupChatCreate",
         [](const ChatMessage &message) {
             return message.isChannel ? message.locale->getString("ActionCreateChannel") : message.locale->getString("ActionCreateMega");
         }},
        {"messageChatChangeTitle",
         [](const ChatMessage &message) {
             const auto title = message.content.value("title").toString();
             return message.isChannel ? message.locale->getString("ActionChannelChangedTitle").replace("un2", title)
                    : message.isOutgoing
                        ? message.locale->getString("ActionYouChangedTitle").replace("un2", title)
                        : message.locale->getString("ActionChangedTitle").replace("un1", message.author).replace("un2", title);
         }},
        {"messageChatChangePhoto",
         [](const ChatMessage &message) {
             return message.isChannel    ? message.locale->getString("ActionChannelChangedPhoto")
                    : message.isOutgoing ? message.locale->getString("ActionYouChangedPhoto")
                                         : message.locale->getString("ActionChangedPhoto").replace("un1", message.author);
         }},
        {"messageChatDeletePhoto",
         [](const ChatMessage &message) {
             return message.isChannel    ? message.locale->getString("ActionChannelRemovedPhoto")
                    : message.isOutgoing ? message.locale->getString("ActionYouRemovedPhoto")
                                         : message.locale->getString("ActionRemovedPhoto").replace("un1", message.author);
         }},
        {"messageChatAddMembers",
         [&](const ChatMessage &message) {
             const auto memberIds = message.content.value("member_user_ids").toList();
             const auto memberCount = memberIds.size();
             if (memberCount == 1)
             {
                 const auto memberUserId = memberIds.first().toLongLong();
                 if (message.sender.value("user_id") == memberUserId)
                 {
                     if (detail::isSupergroup(message.chatDetails))
                     {
                         if (message.isChannel)
                         {
                             return message.locale->getString("ChannelJoined");
                         }
                         else if (detail::isMeUser(memberUserId, message.storageManager))
                         {
                             return message.locale->getString("ChannelMegaJoined");
                         }
                         else if (message.isOutgoing)
                         {
                             return message.locale->getString("ActionAddUserSelfYou");
                         }
                         else
                         {
                             return message.locale->getString("ActionAddUserSelf").replace("un1", message.author);
                         }
                     }
                     else if (message.isOutgoing)
                     {
                         return message.locale->getString("ActionAddUserSelfYou");
                     }
                     else
                     {
                         return message.locale->getString("ActionAddUserSelf").replace("un1", message.author);
                     }
                 }
                 else
                 {
                     if (message.isOutgoing)
                     {
                         const auto userName = getUserName(memberUserId, message.storageManager, message.locale, openUser);
                         return message.locale->getString("ActionYouAddUser").replace("un2", userName);
                     }
                     else if (detail::isMeUser(memberUserId, message.storageManager))
                     {
                         if (detail::isSupergroup(message.chatDetails))
                         {
                             return message.isChannel ? message.locale->getString("ChannelAddedBy").replace("un1", message.author)
                                                      : message.locale->getString("MegaAddedBy").replace("un1", message.author);
                         }
                         else
                         {
                             return message.locale->getString("ActionAddUserYou").replace("un1", message.author);
                         }
                     }
                     else
                     {
                         const auto userName = getUserName(memberUserId, message.storageManager, message.locale, openUser);
                         return message.locale->getString("ActionAddUser").replace("un1", message.author).replace("un2", userName);
                     }
                 }
             }
             else
             {
                 QStringList members;
                 members.reserve(memberCount);
                 for (const auto &userId : memberIds)
                 {
                     members << getUserName(userId.toLongLong(), message.storageManager, message.locale, openUser);
                 }
                 const auto users = members.join(", ");

                 return message.isOutgoing
                            ? message.locale->getString("ActionYouAddUser").arg(users)
                            : message.locale->getString("ActionAddUser").replace("un1", message.author).replace("un2", users);
             }
         }},
        {"messageChatJoinByLink",
         [](const ChatMessage &message) {
             return message.isOutgoing ? message.locale->getString("ActionInviteYou")
                                       : message.locale->getString("ActionInviteUser").replace("un1", message.author);
         }},
        {"messageChatDeleteMember",
         [&](const ChatMessage &message) {
             const auto userId = message.content.value("user_id").toLongLong();
             const auto userName = getUserName(userId, message.storageManager, message.locale, openUser);

             if (userId == message.sender.value("user_id").toLongLong())
             {
                 return message.isOutgoing ? message.locale->getString("ActionYouLeftUser")
                                           : message.locale->getString("ActionLeftUser").replace("un1", message.author);
             }
             else if (message.isOutgoing)
             {
                 return message.locale->getString("ActionYouKickUser").replace("un2", userName);
             }
             else if (detail::isMeUser(userId, message.storageManager))
             {
                 return message.locale->getString("ActionKickUserYou").replace("un1", message.author);
             }
             else
             {
                 return message.locale->getString("ActionKickUser").replace("un1", message.author).replace("un2", userName);
             }
         }},
        {"messageChatUpgradeTo", [](const ChatMessage &message) { return message.locale->getString("ActionMigrateFromGroup"); }},
        {"messageChatUpgradeFrom", [](const ChatMessage &message) { return message.locale->getString("ActionMigrateFromGroup"); }},
        {"messagePinMessage",
         [](const ChatMessage &message) { return message.locale->getString("ActionPinned").replace("un1", message.author); }},
        {"messageScreenshotTaken",
         [](const ChatMessage &message) {
             return message.isOutgoing ? message.locale->getString("ActionTakeScreenshootYou")
                                       : message.locale->getString("ActionTakeScreenshoot").replace("un1", message.author);
         }},
        {"messageCustomServiceAction", [](const ChatMessage &message) { return message.content.value("text").toString(); }},
        {"messageContactRegistered",
         [&](const ChatMessage &message) {
             const auto userName =
                 getUserName(message.sender.value("user_id").toLongLong(), message.storageManager, message.locale, openUser);
             return message.locale->getString("NotificationContactJoined").arg(userName);
         }},
        {"messageWebsiteConnected", [](const ChatMessage &message) { return message.locale->getString("ActionBotAllowed"); }},
        {"messageUnsupported", [](const ChatMessage &message) { return message.locale->getString("UnsupportedMedia"); }}};

    if (const auto it = handlers.find(contentType); it != handlers.end())
    {
        return it->second(messageDetail);
    }

    return locale->getString("UnsupportedMedia");
}

bool isServiceMessage(const Message *message)
{
    const auto contentType = message->content().value("@type").toString();
    static const std::unordered_map<QString, bool> serviceMap = {
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

    if (const auto it = serviceMap.find(contentType); it != serviceMap.end())
    {
        return it->second;
    }

    return false;
}

QString getUserShortName(qint64 userId, StorageManager *store, Locale *locale) noexcept
{
    const auto user = store->getUser(userId);

    const auto firstName = user->firstName();
    const auto lastName = user->lastName();
    const auto userType = user->type().value("@type").toString();

    if (userType == QLatin1String("userTypeBot") || userType == QLatin1String("userTypeRegular"))
    {
        if (!firstName.isEmpty())
        {
            return firstName;
        }

        if (!lastName.isEmpty())
        {
            return lastName;
        }
    }

    if (userType == QLatin1String("userTypeDeleted") || userType == QLatin1String("userTypeUnknown"))
    {
        return locale->getString("HiddenName");
    }

    return QString();
}

QString getTitle(const Message *message, StorageManager *store, Locale *locale) noexcept
{
    const auto sender = message->senderId();
    const auto senderType = sender.value("@type").toString();

    if (senderType == QLatin1String("messageSenderUser"))
    {
        return detail::getUserFullName(sender.value("user_id").toLongLong(), store, locale);
    }

    if (senderType == QLatin1String("messageSenderChat"))
    {
        return store->getChat(sender.value("chat_id").toLongLong())->title();
    }

    return QString();
}

QString getMessageDate(const Message *message, Locale *locale) noexcept
{
    const auto date = QDateTime::fromMSecsSinceEpoch(message->date() * 1000);
    const auto days = date.daysTo(QDateTime::currentDateTime());

    if (days == 0)
    {
        return date.toString(locale->getString("formatterDay12H"));
    }
    else if (days < 7)
    {
        return date.toString(locale->getString("formatterWeek"));
    }

    return date.toString(locale->getString("formatterYear"));
}

QString getContent(const Message *message, StorageManager *store, Locale *locale) noexcept
{
    const auto messageContent = message->content();
    const auto senderInfo = message->senderId();

    // Function to sanitize text
    auto sanitizeText = [](const QString &text) -> QString {
        QString result;
        result.reserve(text.size());  // Reserve memory in advance to avoid reallocations

        for (auto ch : text)
        {
            if (ch == '\n' || ch == '\r')
            {
                result.append(' ');
            }
            else
            {
                result.append(ch);
            }
        }

        return result;
    };

    QString attachmentCaption;
    if (const auto captionText = messageContent.value("caption").toString(); !captionText.isEmpty())
    {
        attachmentCaption.append(": ").append(sanitizeText(captionText));
    }

    const auto contentType = messageContent.value("@type").toString();

    const std::unordered_map<QString, std::function<QString()>> messageHandlers = {
        {"messageAnimation", [&]() { return locale->getString("AttachGif").append(attachmentCaption); }},
        {"messageAudio",
         [&]() {
             const auto audioInfo = messageContent.value("audio").toMap();
             auto title = detail::getAudioTitle(audioInfo).isEmpty() ? locale->getString("AttachMusic") : detail::getAudioTitle(audioInfo);
             return title.append(attachmentCaption);
         }},
        {"messageCall",
         [&]() {
             const auto callText = detail::getCallContent(senderInfo, messageContent, store, locale);
             if (const auto duration = messageContent.value("duration").toInt(); duration > 0)
             {
                 return locale->getString("CallMessageWithDuration").arg(callText).arg(locale->formatCallDuration(duration));
             }
             return callText;
         }},
        {"messageDocument",
         [&]() {
             const auto documentInfo = messageContent.value("document").toMap();
             if (auto fileName = documentInfo.value("file_name").toString(); !fileName.isEmpty())
             {
                 return fileName.append(attachmentCaption);
             }
             return locale->getString("AttachDocument").append(attachmentCaption);
         }},
        {"messageInvoice", [&]() { return messageContent.value("title").toString().append(attachmentCaption); }},
        {"messageLocation", [&]() { return locale->getString("AttachLocation").append(attachmentCaption); }},
        {"messagePhoto", [&]() { return locale->getString("AttachPhoto").append(attachmentCaption); }},
        {"messagePoll",
         [&]() {
             return QString::fromUtf8("\xf0\x9f\x93\x8a\x20").append(messageContent.value("poll").toMap().value("question").toString());
         }},
        {"messageSticker",
         [&]() {
             const auto stickerInfo = messageContent.value("sticker").toMap();
             const auto emoji = stickerInfo.value("emoji").toString();
             return locale->getString("AttachSticker").append(": ").append(emoji);
         }},
        {"messageText", [&]() { return sanitizeText(messageContent.value("text").toMap().value("text").toString()); }},
        {"messageVideo", [&]() { return locale->getString("AttachVideo").append(attachmentCaption); }},
        {"messageVideoNote", [&]() { return locale->getString("AttachRound").append(attachmentCaption); }},
        {"messageVoiceNote", [&]() { return locale->getString("AttachAudio").append(attachmentCaption); }}};

    if (const auto handler = messageHandlers.find(contentType); handler != messageHandlers.end())
    {
        return handler->second();
    }

    // Service messages that require getServiceMessageContent
    const std::unordered_set<QString> serviceMessageTypes = {
        "messageBasicGroupChatCreate", "messageChatAddMembers",      "messageChatChangePhoto",
        "messageChatChangeTitle",      "messageChatDeleteMember",    "messageChatDeletePhoto",
        "messageChatJoinByLink",       "messageChatUpgradeFrom",     "messageChatUpgradeTo",
        "messageContactRegistered",    "messageCustomServiceAction", "messageExpiredPhoto",
        "messageExpiredVideo",         "messageGameScore",           "messagePassportDataReceived",
        "messagePassportDataSent",     "messagePaymentSuccessful",   "messagePaymentSuccessfulBot",
        "messagePinMessage",           "messageScreenshotTaken",     "messageSupergroupChatCreate",
        "messageUnsupported",          "messageWebsiteConnected"};

    // Check if contentType is a service message type
    if (serviceMessageTypes.contains(contentType))
    {
        return getServiceMessageContent(message, store, locale);
    }

    return locale->getString("UnsupportedAttachment");
}

bool isChatUnread(qint64 chatId, StorageManager *store) noexcept
{
    const auto chat = store->getChat(chatId);

    return chat->isMarkedAsUnread() || chat->unreadCount() > 0;
}

QString getMessageSenderName(const Message *message, StorageManager *store, Locale *locale) noexcept
{
    if (isServiceMessage(message))
        return QString();

    const auto sender = message->senderId();
    const auto chat = store->getChat(message->chatId());
    const auto chatType = chat->type().value("@type").toString();

    static const std::unordered_map<QString, std::function<QString(const QVariantMap &, const Chat *, StorageManager *, Locale *)>>
        chatTypeHandlers = {
            {"chatTypeBasicGroup",
             [](const QVariantMap &sender, const Chat *chat, StorageManager *store, Locale *locale) {
                 if (detail::isChannelChat(chat))
                     return QString();

                 const auto senderType = sender.value("@type").toString();
                 static const std::unordered_map<QString,
                                                 std::function<QString(const QVariantMap &, const Chat *, StorageManager *, Locale *)>>
                     senderTypeHandlers = {{"messageSenderUser",
                                            [](const QVariantMap &sender, const Chat *, StorageManager *store, Locale *locale) {
                                                if (detail::isMeUser(sender.value("user_id").toLongLong(), store))
                                                    return locale->getString("FromYou");

                                                return getUserShortName(sender.value("user_id").toLongLong(), store, locale);
                                            }},
                                           {"messageSenderChat", [](const QVariantMap &, const Chat *chat, StorageManager *store,
                                                                    Locale *locale) { return getChatTitle(chat->id(), store, locale); }}};

                 if (auto it = senderTypeHandlers.find(senderType); it != senderTypeHandlers.end())
                 {
                     return it->second(sender, chat, store, locale);
                 }
                 return QString();
             }},
            {"chatTypeSupergroup", [](const QVariantMap &sender, const Chat *chat, StorageManager *store, Locale *locale) {
                 if (detail::isChannelChat(chat))
                     return QString();

                 const auto senderType = sender.value("@type").toString();
                 static const std::unordered_map<QString,
                                                 std::function<QString(const QVariantMap &, const Chat *, StorageManager *, Locale *)>>
                     senderTypeHandlers = {{"messageSenderUser",
                                            [](const QVariantMap &sender, const Chat *, StorageManager *store, Locale *locale) {
                                                if (detail::isMeUser(sender.value("user_id").toLongLong(), store))
                                                    return locale->getString("FromYou");

                                                return getUserShortName(sender.value("user_id").toLongLong(), store, locale);
                                            }},
                                           {"messageSenderChat", [](const QVariantMap &, const Chat *chat, StorageManager *store,
                                                                    Locale *locale) { return getChatTitle(chat->id(), store, locale); }}};

                 if (auto it = senderTypeHandlers.find(senderType); it != senderTypeHandlers.end())
                 {
                     return it->second(sender, chat, store, locale);
                 }
                 return QString();
             }}};

    if (auto it = chatTypeHandlers.find(chatType); it != chatTypeHandlers.end())
    {
        return it->second(sender, chat, store, locale);
    }

    return QString();
}

void copyToClipboard(const QVariantMap &content) noexcept
{
    const auto contentType = content.value("@type").toString();
    if (contentType != QLatin1String("messageText"))
        return;

    auto clipboard = QApplication::clipboard();
    const auto text = content.value("text").toMap().value("text").toString();

    clipboard->setText(text);
}

QImage getThumb(const QVariantMap &thumbnail) noexcept
{
    QByteArray thumb = "data:image/jpeg;base64, " + thumbnail.value("data").toByteArray();
    QBuffer buf(&thumb);
    buf.open(QIODevice::ReadOnly);

    QImageReader reader(&buf);
    return reader.read();
}

bool isMessageUnread(qint64 chatId, const Message *message, StorageManager *store) noexcept
{
    const auto chat = store->getChat(chatId);

    if (message->isOutgoing())
    {
        if (isMeChat(chat, store))
        {
            return false;
        }
        return message->id() > chat->lastReadOutboxMessageId();
    }

    return message->id() > chat->lastReadInboxMessageId();
}

QString getViews(int views) noexcept
{
    if (views < 1000)
    {
        return QString::number(views);
    }
    else if (views < 1000000)
    {
        auto value = views / 100;
        auto fractionDigits = (value % 10 == 0) ? 0 : 1;

        return QString("%1K").arg(static_cast<double>(value) / 10.0, 0, 'f', fractionDigits);
    }
    else
    {
        auto value = views / 100000;
        auto fractionDigits = (value % 10 == 0) ? 0 : 1;

        return QString("%1M").arg(static_cast<double>(value) / 10.0, 0, 'f', fractionDigits);
    }
}

QString getFileSize(const QVariantMap &file) noexcept
{
    const auto size = file.value("size").toInt();
    if (size <= 0)
        return QString();

    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;

    if (size < KB)
        return QString("%1 B").arg(size);
    else if (size < MB)
        return QString("%1 KB").arg(static_cast<double>(size) / KB, 0, 'f', 2);
    else if (size < GB)
        return QString("%1 MB").arg(static_cast<double>(size) / MB, 0, 'f', 2);
    else
        return QString("%1 GB").arg(static_cast<double>(size) / GB, 0, 'f', 2);
}

QString formatTime(int totalSeconds) noexcept
{
    QString result;
    QTextStream stream(&result);

    auto appendDuration = [&stream](int count, QChar order) {
        if (count > 0)
        {
            if (!stream.string()->isEmpty())
            {
                stream << ' ';
            }
            stream << count << order;
        }
    };

    const int seconds = totalSeconds % 60;
    const int totalMinutes = totalSeconds / 60;
    const int minutes = totalMinutes % 60;
    const int totalHours = totalMinutes / 60;
    const int hours = totalHours % 24;
    const int days = totalHours / 24;

    appendDuration(days, 'd');
    appendDuration(hours, 'h');
    appendDuration(minutes, 'm');
    appendDuration(seconds, 's');

    return result;
}

}  // namespace Utils
