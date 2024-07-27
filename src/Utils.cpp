#include "Utils.hpp"

#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QDateTime>
#include <QImageReader>
#include <QLocale>
#include <QScopedPointer>
#include <QStringList>
#include <QStringRef>
#include <QTextStream>

#include <algorithm>
#include <cmath>
#include <functional>
#include <unordered_map>
#include <unordered_set>

namespace {

bool isBotUser(const td::td_api::user *user) noexcept
{
    return user->type_->get_id() == td::td_api::userTypeBot::ID;
}

bool isMeUser(qint64 userId, StorageManager *store) noexcept
{
    return store->getMyId() == userId;
}

bool isUserOnline(const td::td_api::user *user) noexcept
{
    if (std::ranges::any_of(ServiceNotificationsUserIds, [user](auto id) { return id == user->id_; }))
    {
        return false;
    }

    return user->status_->get_id() == td::td_api::userStatusOnline::ID && user->type_->get_id() != td::td_api::userTypeBot::ID;
}

bool isChannelChat(const td::td_api::chat *chat) noexcept
{
    if (chat->type_->get_id() == td::td_api::chatTypeSupergroup::ID)
    {
        return static_cast<const td::td_api::chatTypeSupergroup *>(chat->type_.get())->is_channel_;
    }

    return false;
}

bool isSupergroup(const td::td_api::chat *chat) noexcept
{
    return chat->type_->get_id() == td::td_api::chatTypeSupergroup::ID;
}

QString getMessageAuthor(const td::td_api::message &message, StorageManager *store, Locale *locale, bool openUser) noexcept
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());
    const QString linkStyle = "<a style=\"text-decoration: none; font-weight: bold; color: grey\" href=\"";
    const QString linkClose = "</a>";

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        const auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
        const auto userName = Utils::getUserShortName(userId, store, locale);

        if (openUser)
        {
            return linkStyle + "userId://" + QString::number(userId) + "\">" + userName + linkClose;
        }
        return userName;
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        const auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
        const auto chatTitle = QString::fromStdString(store->getChat(chatId)->title_);

        if (openUser)
        {
            return linkStyle + "chatId://" + QString::number(chatId) + "\">" + chatTitle + linkClose;
        }
        return chatTitle;
    }

    return {};
}

QString getUserFullName(qint64 userId, StorageManager *store, Locale *locale) noexcept
{
    const auto user = store->getUser(userId);

    switch (user->type_->get_id())
    {
        case td::td_api::userTypeBot::ID:
        case td::td_api::userTypeRegular::ID: {
            return QString::fromStdString(user->first_name_ + " " + user->last_name_).trimmed();
        }
        case td::td_api::userTypeDeleted::ID:
        case td::td_api::userTypeUnknown::ID: {
            return locale->getString("HiddenName");
        }

        default:
            return QString();
    }
}

QString getUserName(auto userId, StorageManager *store, Locale *locale, bool openUser)
{
    const auto userName = Utils::getUserShortName(userId, store, locale);
    if (userName.isEmpty())
    {
        return QString();
    }

    QString result;
    if (openUser)
    {
        result = QString("<a style=\"text-decoration: none; font-weight: bold; color: grey\" href=\"userId://%1\">%2</a>").arg(userId).arg(userName);
    }
    else
    {
        result = QString("<span style=\"color: grey\">%1</span>").arg(userName);
    }

    return result;
}

QString getCallContent(qint64 userId, const td::td_api::messageCall &content, StorageManager *store, Locale *locale) noexcept
{
    const auto isVideo = content.is_video_;
    const auto &discardReason = content.discard_reason_;
    const auto isMissed = discardReason->get_id() == td::td_api::callDiscardReasonMissed::ID;
    const auto isBusy = discardReason->get_id() == td::td_api::callDiscardReasonDeclined::ID;
    const auto isOutgoing = isMeUser(userId, store);

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

QString getAudioTitle(const td::td_api::audio &audio) noexcept
{
    const auto fileName = QString::fromStdString(audio.file_name_);
    const auto title = QString::fromStdString(audio.title_).trimmed();
    const auto performer = QString::fromStdString(audio.performer_).trimmed();

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
    return store->getUser(userId)->type_->get_id() == td::td_api::userTypeDeleted::ID;
}

}  // namespace

td::td_api::object_ptr<td::td_api::ChatList> Utils::toChatList(const ChatList &list)
{
    switch (list.type)
    {
        case TdApi::ChatListArchive:
            return td::td_api::make_object<td::td_api::chatListArchive>();
        case TdApi::ChatListFolder:
            return td::td_api::make_object<td::td_api::chatListFolder>(list.folderId);
        default:
            return td::td_api::make_object<td::td_api::chatListMain>();
    }
}

td::td_api::object_ptr<td::td_api::chatPosition> Utils::getChatPosition(const td::td_api::chat *chat, const ChatList &chatList)
{
    const auto &positions = chat->positions_;

    const auto findPosition = [&](int id,
                                  std::function<bool(const td::td_api::ChatList &)> &&folder = nullptr) -> td::td_api::object_ptr<td::td_api::chatPosition> {
        for (const auto &position : positions)
        {
            if (const auto &list = position->list_; list->get_id() == id && (!folder || folder(*list)))
            {
                auto result = td::td_api::make_object<td::td_api::chatPosition>();

                result->order_ = position->order_;
                result->is_pinned_ = position->is_pinned_;
                result->list_ = toChatList(chatList);

                return result;
            }
        }
        return nullptr;
    };

    switch (chatList.type)
    {
        case TdApi::ChatListMain:
            return findPosition(td::td_api::chatListMain::ID);
        case TdApi::ChatListArchive:
            return findPosition(td::td_api::chatListArchive::ID);
        case TdApi::ChatListFolder: {
            auto folder = [&chatList](const td::td_api::ChatList &list) {
                return static_cast<const td::td_api::chatListFolder &>(list).chat_folder_id_ == chatList.folderId;
            };
            return findPosition(td::td_api::chatListFolder::ID, folder);
        }
        default:
            return nullptr;
    }
}

bool Utils::isChatPinned(const td::td_api::chat *chat, const ChatList &chatList)
{
    return getChatPosition(chat, chatList)->is_pinned_;
}

qint64 Utils::getChatOrder(const td::td_api::chat *chat, const ChatList &chatList)
{
    return getChatPosition(chat, chatList)->order_;
}

bool Utils::isMeChat(const td::td_api::chat *chat, StorageManager *store) noexcept
{
    if (!chat || !chat->type_)  // Safety check for null pointers
        return false;

    const auto chatTypeId = chat->type_->get_id();
    const auto myId = store->getMyId();

    if (chatTypeId == td::td_api::chatTypeSecret::ID)
    {
        const auto userId = static_cast<const td::td_api::chatTypeSecret *>(chat->type_.get())->user_id_;
        return myId == userId;
    }
    else if (chatTypeId == td::td_api::chatTypePrivate::ID)
    {
        const auto userId = static_cast<const td::td_api::chatTypePrivate *>(chat->type_.get())->user_id_;
        return myId == userId;
    }

    return false;
}

QString Utils::getChatTitle(qint64 chatId, StorageManager *store, Locale *locale, bool showSavedMessages)
{
    const auto chat = store->getChat(chatId);

    if (isMeChat(chat, store) && showSavedMessages)
    {
        return locale->getString("SavedMessages");
    }

    const auto title = QString::fromStdString(chat->title_).trimmed();

    return !title.isEmpty() ? title : locale->getString("HiddenName");
}

bool Utils::isChatMuted(qint64 chatId, StorageManager *store)
{
    return getChatMuteFor(chatId, store) > 0;
}

int Utils::getChatMuteFor(qint64 chatId, StorageManager *store)
{
    if (auto chat = store->getChat(chatId))
    {
        if (chat->notification_settings_)
        {
            return chat->notification_settings_->mute_for_;
        }
    }

    return 0;
}

QString Utils::getServiceMessageContent(const td::td_api::message &message, StorageManager *store, Locale *locale, bool openUser)
{
    const auto chat = store->getChat(message.chat_id_);

    auto &sender = message.sender_id_;
    auto &content = message.content_;
    auto isOutgoing = message.is_outgoing_;

    auto isChannel = isChannelChat(chat);

    auto author = getMessageAuthor(message, store, locale, openUser);

    switch (content->get_id())
    {
        case td::td_api::messageExpiredPhoto::ID: {
            return locale->getString("AttachPhotoExpired");
        }
        case td::td_api::messageExpiredVideo::ID: {
            return locale->getString("AttachVideoExpired");
        }
        case td::td_api::messageBasicGroupChatCreate::ID: {
            if (isOutgoing)
            {
                return locale->getString("ActionYouCreateGroup");
            }

            return locale->getString("ActionCreateGroup").replace("un1", author);
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            if (isChannel)
            {
                return locale->getString("ActionCreateChannel");
            }

            return locale->getString("ActionCreateMega");
        }
        case td::td_api::messageChatChangeTitle::ID: {
            const auto title = QString::fromStdString(static_cast<const td::td_api::messageChatChangeTitle *>(content.get())->title_);

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
        case td::td_api::messageChatChangePhoto::ID: {
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
        case td::td_api::messageChatDeletePhoto::ID: {
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
        case td::td_api::messageChatAddMembers::ID: {
            const auto &memberUserIds = static_cast<const td::td_api::messageChatAddMembers *>(content.get())->member_user_ids_;

            if (memberUserIds.size() == 1)
            {
                auto memberUserId = memberUserIds[0];
                if (static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_ == memberUserId)
                {
                    if (isSupergroup(chat) && isChannel)
                    {
                        return locale->getString("ChannelJoined");
                    }

                    if (isSupergroup(chat) && !isChannel)
                    {
                        if (isMeUser(memberUserId, store))
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
                    return locale->getString("ActionYouAddUser").replace("un2", getUserName(memberUserId, store, locale, openUser));
                }

                if (isMeUser(memberUserId, store))
                {
                    if (isSupergroup(chat))
                    {
                        if (!isChannel)
                        {
                            return locale->getString("MegaAddedBy").replace("un1", author);
                        }

                        return locale->getString("ChannelAddedBy").replace("un1", author);
                    }

                    return locale->getString("ActionAddUserYou").replace("un1", author);
                }

                return locale->getString("ActionAddUser").replace("un1", author).replace("un2", getUserName(memberUserId, store, locale, openUser));
            }

            QStringList result;
            for (const auto &userId : memberUserIds)
            {
                result << getUserName(userId, store, locale, openUser);
            }

            auto users = result.join(", ");

            if (isOutgoing)
            {
                return locale->getString("ActionYouAddUser").arg(users);
            }

            return locale->getString("ActionAddUser").replace("un1", author).replace("un2", users);
        }
        case td::td_api::messageChatJoinByLink::ID: {
            if (isOutgoing)
            {
                return locale->getString("ActionInviteYou");
            }

            return locale->getString("ActionInviteUser").replace("un1", author);
        }
        case td::td_api::messageChatDeleteMember::ID: {
            auto userId = static_cast<const td::td_api::messageChatDeleteMember *>(content.get())->user_id_;

            if (userId == static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_)
            {
                if (isOutgoing)
                {
                    return locale->getString("ActionYouLeftUser");
                }

                return locale->getString("ActionLeftUser").replace("un1", author);
            }

            if (isOutgoing)
            {
                return locale->getString("ActionYouKickUser").replace("un2", getUserName(userId, store, locale, openUser));
            }
            else if (isMeUser(userId, store))
            {
                return locale->getString("ActionKickUserYou").replace("un1", author);
            }

            return locale->getString("ActionKickUser").replace("un1", author).replace("un2", getUserName(userId, store, locale, openUser));
        }
        case td::td_api::messageChatUpgradeTo::ID: {
            return locale->getString("ActionMigrateFromGroup");
        }
        case td::td_api::messageChatUpgradeFrom::ID: {
            return locale->getString("ActionMigrateFromGroup");
        }
        case td::td_api::messagePinMessage::ID: {
            return locale->getString("ActionPinned").replace("un1", author);
        }
        case td::td_api::messageScreenshotTaken::ID: {
            if (isOutgoing)
            {
                return locale->getString("ActionTakeScreenshootYou");
            }

            return locale->getString("ActionTakeScreenshoot").replace("un1", author);
        }
        case td::td_api::messageCustomServiceAction::ID: {
            return QString::fromStdString(static_cast<const td::td_api::messageCustomServiceAction *>(content.get())->text_);
        }
        case td::td_api::messageContactRegistered::ID: {
            return locale->getString("NotificationContactJoined")
                .arg(getUserName(static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_, store, locale, openUser));
        }
        case td::td_api::messageUnsupported::ID: {
            return locale->getString("UnsupportedMedia");
        }
    }

    return locale->getString("UnsupportedMedia");
}

bool Utils::isServiceMessage(const td::td_api::message &message)
{
    const std::unordered_set<int> serviceMessageTypes = {
        td::td_api::messageBasicGroupChatCreate::ID, td::td_api::messageChatAddMembers::ID,      td::td_api::messageChatChangePhoto::ID,
        td::td_api::messageChatChangeTitle::ID,      td::td_api::messageChatDeleteMember::ID,    td::td_api::messageChatDeletePhoto::ID,
        td::td_api::messageChatJoinByLink::ID,       td::td_api::messageChatUpgradeFrom::ID,     td::td_api::messageChatUpgradeTo::ID,
        td::td_api::messageContactRegistered::ID,    td::td_api::messageCustomServiceAction::ID, td::td_api::messageExpiredPhoto::ID,
        td::td_api::messageExpiredVideo::ID,         td::td_api::messageGameScore::ID,           td::td_api::messagePassportDataReceived::ID,
        td::td_api::messagePassportDataSent::ID,     td::td_api::messagePaymentSuccessful::ID,   td::td_api::messagePaymentSuccessfulBot::ID,
        td::td_api::messagePinMessage::ID,           td::td_api::messageScreenshotTaken::ID,     td::td_api::messageSupergroupChatCreate::ID,
        td::td_api::messageUnsupported::ID,
    };

    if (message.content_)
    {
        const int contentId = message.content_->get_id();
        return serviceMessageTypes.contains(contentId);
    }

    return false;
}

QString Utils::getUserShortName(qint64 userId, StorageManager *store, Locale *locale) noexcept
{
    const auto user = store->getUser(userId);

    const auto firstName = user->first_name_;
    const auto lastName = user->last_name_;

    switch (user->type_->get_id())
    {
        case td::td_api::userTypeBot::ID:
        case td::td_api::userTypeRegular::ID: {
            if (!firstName.empty())
            {
                return QString::fromStdString(firstName);
            }

            if (!lastName.empty())
            {
                return QString::fromStdString(lastName);
            }
        }
        case td::td_api::userTypeDeleted::ID:
        case td::td_api::userTypeUnknown::ID: {
            return locale->getString("HiddenName");
        }

        default:
            return QString();
    }
}

QString Utils::getTitle(const td::td_api::message &message, StorageManager *store, Locale *locale) noexcept
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
        return getUserFullName(userId, store, locale);
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
        return QString::fromStdString(store->getChat(chatId)->title_);
    }

    return {};
}

QString Utils::getMessageDate(const td::td_api::message &message, Locale *locale) noexcept
{
    const auto date = QDateTime::fromMSecsSinceEpoch(message.date_ * 1000);
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

QString Utils::getContent(const td::td_api::message &message, StorageManager *store, Locale *locale) noexcept
{
    const auto &content = message.content_;
    const auto &sender = message.sender_id_;

    auto sanitizeText = [](const std::string &text) -> QString {
        QString result;
        result.reserve(static_cast<int>(text.size()));  // Reserve memory in advance to avoid reallocations

        for (char ch : text)
        {
            // Append a space for newline or carriage return, otherwise append the character
            result.append((ch == '\n' || ch == '\r') ? ' ' : QChar(ch));
        }

        return result;
    };

    auto captionText = [&](const td::td_api::formattedText &caption) -> QString {
        QString result;

        result.append(": ").append(sanitizeText(caption.text_));

        return result;
    };

    const std::unordered_map<int, std::function<QString()>> messageHandlers = {
        {td::td_api::messageAnimation::ID,
         [&]() { return locale->getString("AttachGif").append(captionText(*static_cast<const td::td_api::messageAnimation *>(content.get())->caption_)); }},
        {td::td_api::messageAudio::ID,
         [&]() {
             const auto &audio = static_cast<const td::td_api::messageAudio *>(content.get())->audio_;

             auto title = getAudioTitle(*audio).isEmpty() ? locale->getString("AttachMusic") : getAudioTitle(*audio);
             return title.append(captionText(*static_cast<const td::td_api::messageAudio *>(content.get())->caption_));
         }},
        {td::td_api::messageCall::ID,
         [&]() {
             const auto &call = static_cast<const td::td_api::messageCall *>(content.get());

             const auto callText = getCallContent(static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_, *call, store, locale);
             if (const auto duration = call->duration_; duration > 0)
             {
                 return locale->getString("CallMessageWithDuration").arg(callText).arg(locale->formatCallDuration(duration));
             }
             return callText;
         }},
        {td::td_api::messageDocument::ID,
         [&]() {
             const auto &caption = static_cast<const td::td_api::messageDocument *>(content.get())->caption_;
             const auto &document = static_cast<const td::td_api::messageDocument *>(content.get())->document_;

             if (auto fileName = QString::fromStdString(document->file_name_); !fileName.isEmpty())
             {
                 return fileName.append(captionText(*caption));
             }
             return locale->getString("AttachDocument").append(captionText(*caption));
         }},
        {td::td_api::messageInvoice::ID,
         [&]() {
             const auto &productInfo = static_cast<const td::td_api::messageInvoice *>(content.get())->product_info_;

             return sanitizeText(productInfo->title_);
         }},
        {td::td_api::messageLocation::ID, [&]() { return locale->getString("AttachLocation"); }},
        {td::td_api::messagePhoto::ID,
         [&]() { return locale->getString("AttachPhoto").append(captionText(*static_cast<const td::td_api::messagePhoto *>(content.get())->caption_)); }},
        {td::td_api::messagePoll::ID,
         [&]() {
             const auto &poll = static_cast<const td::td_api::messagePoll *>(content.get())->poll_;

             return QString::fromUtf8("\xf0\x9f\x93\x8a\x20").append(captionText(*poll->question_));
         }},
        {td::td_api::messageSticker::ID,
         [&]() {
             const auto &sticker = static_cast<const td::td_api::messageSticker *>(content.get())->sticker_;
             return locale->getString("AttachSticker").append(": ").append(QString::fromStdString(sticker->emoji_));
         }},
        {td::td_api::messageText::ID,
         [&]() {
             const auto &text = static_cast<const td::td_api::messageText *>(content.get())->text_;

             return sanitizeText(text->text_);
         }},
        {td::td_api::messageVideo::ID,
         [&]() { return locale->getString("AttachVideo").append(captionText(*static_cast<const td::td_api::messageVideo *>(content.get())->caption_)); }},
        {td::td_api::messageVideoNote::ID, [&]() { return locale->getString("AttachRound"); }},
        {td::td_api::messageVoiceNote::ID,
         [&]() { return locale->getString("AttachAudio").append(captionText(*static_cast<const td::td_api::messageVoiceNote *>(content.get())->caption_)); }}};

    if (const auto handler = messageHandlers.find(content->get_id()); handler != messageHandlers.end())
    {
        return handler->second();
    }

    // Service messages that require getServiceMessageContent

    // Check if contentType is a service message type
    if (isServiceMessage(message))
    {
        return getServiceMessageContent(message, store, locale);
    }

    return locale->getString("UnsupportedAttachment");
}

bool Utils::isChatUnread(qint64 chatId, StorageManager *store) noexcept
{
    const auto chat = store->getChat(chatId);

    return chat->is_marked_as_unread_ || chat->unread_count_ > 0;
}

QString Utils::getMessageSenderName(const td::td_api::message &message, StorageManager *store, Locale *locale) noexcept
{
    if (isServiceMessage(message))
        return QString();

    const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());

    const auto chat = store->getChat(message.chat_id_);

    switch (chat->type_->get_id())
    {
        case td::td_api::chatTypePrivate::ID:
        case td::td_api::chatTypeSecret::ID:
            return QString();
        case td::td_api::chatTypeBasicGroup::ID:
        case td::td_api::chatTypeSupergroup::ID: {
            if (isChannelChat(chat))
            {
                return QString();
            }

            switch (sender->get_id())
            {
                case td::td_api::messageSenderUser::ID: {
                    auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
                    if (isMeUser(userId, store))
                        return locale->getString("FromYou");

                    return getUserShortName(userId, store, locale);
                }
                case td::td_api::messageSenderChat::ID: {
                    return getChatTitle(static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_, store, locale);
                }
            }
        }

        default:
            return QString();
    }
}

void Utils::copyToClipboard(const QVariantMap &content) noexcept
{
    const auto contentType = content.value("@type").toString();
    if (contentType != QLatin1String("messageText"))
        return;

    auto clipboard = QApplication::clipboard();
    const auto text = content.value("text").toMap().value("text").toString();

    clipboard->setText(text);
}

QImage Utils::getThumb(const QVariantMap &thumbnail) noexcept
{
    QByteArray thumb = "data:image/jpeg;base64, " + thumbnail.value("data").toByteArray();
    QBuffer buf(&thumb);
    buf.open(QIODevice::ReadOnly);

    QImageReader reader(&buf);
    return reader.read();
}

bool Utils::isMessageUnread(const td::td_api::message &message, StorageManager *store) noexcept
{
    const auto chat = store->getChat(message.chat_id_);

    if (message.is_outgoing_)
    {
        if (isMeChat(chat, store))
        {
            return false;
        }
        return message.id_ > chat->last_read_outbox_message_id_;
    }

    return message.id_ > chat->last_read_inbox_message_id_;
}

QString Utils::getViews(int views) noexcept
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

QString Utils::getFileSize(const QVariantMap &file) noexcept
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

QString Utils::formatTime(int totalSeconds) noexcept
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
