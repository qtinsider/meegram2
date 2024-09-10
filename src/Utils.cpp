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

bool isMeUser(qlonglong userId, StorageManager *store) noexcept
{
    return store->myId() == userId;
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

QString getMessageAuthor(const td::td_api::message &message, StorageManager *store, bool openUser) noexcept
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());
    const QString linkStyle = "<a style=\"text-decoration: none; font-weight: bold; color: grey\" href=\"";
    const QString linkClose = "</a>";

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        const auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
        const auto userName = Utils::getUserShortName(userId, store);

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

QString getUserFullName(qlonglong userId, StorageManager *store) noexcept
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
            return QObject::tr("HiddenName");
        }

        default:
            return QString();
    }
}

QString getUserName(auto userId, StorageManager *store, bool openUser)
{
    const auto userName = Utils::getUserShortName(userId, store);
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

QString getCallContent(qlonglong userId, const td::td_api::messageCall &content, StorageManager *store) noexcept
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
            return isVideo ? QObject::tr("CallMessageVideoOutgoingMissed") : QObject::tr("CallMessageOutgoingMissed");
        }
        return isVideo ? QObject::tr("CallMessageVideoIncomingMissed") : QObject::tr("CallMessageIncomingMissed");
    }

    if (isBusy)
    {
        return isVideo ? QObject::tr("CallMessageVideoIncomingDeclined") : QObject::tr("CallMessageIncomingDeclined");
    }

    return isVideo ? (isOutgoing ? QObject::tr("CallMessageVideoOutgoing") : QObject::tr("CallMessageVideoIncoming"))
                   : (isOutgoing ? QObject::tr("CallMessageOutgoing") : QObject::tr("CallMessageIncoming"));
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

bool isUserBlocked(qlonglong userId, StorageManager *store) noexcept
{
    const auto fullInfo = store->getUserFullInfo(userId);

    return !fullInfo /* && fullInfo.value("is_blocked").toBool()*/;
}

bool isDeletedUser(qlonglong userId, StorageManager *store)
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

qlonglong Utils::getChatOrder(const td::td_api::chat *chat, const ChatList &chatList)
{
    return getChatPosition(chat, chatList)->order_;
}

bool Utils::isMeChat(const td::td_api::chat *chat, StorageManager *store) noexcept
{
    if (!chat || !chat->type_)  // Safety check for null pointers
        return false;

    const auto chatTypeId = chat->type_->get_id();
    const auto myId = store->myId();

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

QString Utils::getChatTitle(qlonglong chatId, StorageManager *store, bool showSavedMessages)
{
    const auto chat = store->getChat(chatId);

    if (isMeChat(chat, store) && showSavedMessages)
    {
        return QObject::tr("SavedMessages");
    }

    const auto title = QString::fromStdString(chat->title_).trimmed();

    return !title.isEmpty() ? title : QObject::tr("HiddenName");
}

bool Utils::isChatMuted(qlonglong chatId, StorageManager *store)
{
    return getChatMuteFor(chatId, store) > 0;
}

int Utils::getChatMuteFor(qlonglong chatId, StorageManager *store)
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

QString Utils::getServiceMessageContent(const td::td_api::message &message, StorageManager *store, bool openUser)
{
    const auto chat = store->getChat(message.chat_id_);

    auto &sender = message.sender_id_;
    auto &content = message.content_;
    auto isOutgoing = message.is_outgoing_;

    auto isChannel = isChannelChat(chat);

    auto author = getMessageAuthor(message, store, openUser);

    switch (content->get_id())
    {
        case td::td_api::messageExpiredPhoto::ID: {
            return QObject::tr("AttachPhotoExpired");
        }
        case td::td_api::messageExpiredVideo::ID: {
            return QObject::tr("AttachVideoExpired");
        }
        case td::td_api::messageBasicGroupChatCreate::ID: {
            if (isOutgoing)
            {
                return QObject::tr("ActionYouCreateGroup");
            }

            return QObject::tr("ActionCreateGroup").replace("un1", author);
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            if (isChannel)
            {
                return QObject::tr("ActionCreateChannel");
            }

            return QObject::tr("ActionCreateMega");
        }
        case td::td_api::messageChatChangeTitle::ID: {
            const auto title = QString::fromStdString(static_cast<const td::td_api::messageChatChangeTitle *>(content.get())->title_);

            if (isChannel)
            {
                return QObject::tr("ActionChannelChangedTitle").replace("un2", title);
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouChangedTitle").replace("un2", title);
            }

            return QObject::tr("ActionChangedTitle").replace("un1", author).replace("un2", title);
        }
        case td::td_api::messageChatChangePhoto::ID: {
            if (isChannel)
            {
                return QObject::tr("ActionChannelChangedPhoto");
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouChangedPhoto");
            }

            return QObject::tr("ActionChangedPhoto").replace("un1", author);
        }
        case td::td_api::messageChatDeletePhoto::ID: {
            if (isChannel)
            {
                return QObject::tr("ActionChannelRemovedPhoto");
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouRemovedPhoto");
            }

            return QObject::tr("ActionRemovedPhoto").replace("un1", author);
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
                        return QObject::tr("ChannelJoined");
                    }

                    if (isSupergroup(chat) && !isChannel)
                    {
                        if (isMeUser(memberUserId, store))
                        {
                            return QObject::tr("ChannelMegaJoined");
                        }

                        return QObject::tr("ActionAddUserSelfMega").replace("un1", author);
                    }

                    if (isOutgoing)
                    {
                        return QObject::tr("ActionAddUserSelfYou");
                    }

                    return QObject::tr("ActionAddUserSelf").replace("un1", author);
                }

                if (isOutgoing)
                {
                    return QObject::tr("ActionYouAddUser").replace("un2", getUserName(memberUserId, store, openUser));
                }

                if (isMeUser(memberUserId, store))
                {
                    if (isSupergroup(chat))
                    {
                        if (!isChannel)
                        {
                            return QObject::tr("MegaAddedBy").replace("un1", author);
                        }

                        return QObject::tr("ChannelAddedBy").replace("un1", author);
                    }

                    return QObject::tr("ActionAddUserYou").replace("un1", author);
                }

                return QObject::tr("ActionAddUser").replace("un1", author).replace("un2", getUserName(memberUserId, store, openUser));
            }

            QStringList result;
            for (const auto &userId : memberUserIds)
            {
                result << getUserName(userId, store, openUser);
            }

            auto users = result.join(", ");

            if (isOutgoing)
            {
                return QObject::tr("ActionYouAddUser").arg(users);
            }

            return QObject::tr("ActionAddUser").replace("un1", author).replace("un2", users);
        }
        case td::td_api::messageChatJoinByLink::ID: {
            if (isOutgoing)
            {
                return QObject::tr("ActionInviteYou");
            }

            return QObject::tr("ActionInviteUser").replace("un1", author);
        }
        case td::td_api::messageChatDeleteMember::ID: {
            auto userId = static_cast<const td::td_api::messageChatDeleteMember *>(content.get())->user_id_;

            if (userId == static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_)
            {
                if (isOutgoing)
                {
                    return QObject::tr("ActionYouLeftUser");
                }

                return QObject::tr("ActionLeftUser").replace("un1", author);
            }

            if (isOutgoing)
            {
                return QObject::tr("ActionYouKickUser").replace("un2", getUserName(userId, store, openUser));
            }
            else if (isMeUser(userId, store))
            {
                return QObject::tr("ActionKickUserYou").replace("un1", author);
            }

            return QObject::tr("ActionKickUser").replace("un1", author).replace("un2", getUserName(userId, store, openUser));
        }
        case td::td_api::messageChatUpgradeTo::ID: {
            return QObject::tr("ActionMigrateFromGroup");
        }
        case td::td_api::messageChatUpgradeFrom::ID: {
            return QObject::tr("ActionMigrateFromGroup");
        }
        case td::td_api::messagePinMessage::ID: {
            return QObject::tr("ActionPinned").replace("un1", author);
        }
        case td::td_api::messageScreenshotTaken::ID: {
            if (isOutgoing)
            {
                return QObject::tr("ActionTakeScreenshootYou");
            }

            return QObject::tr("ActionTakeScreenshoot").replace("un1", author);
        }
        case td::td_api::messageCustomServiceAction::ID: {
            return QString::fromStdString(static_cast<const td::td_api::messageCustomServiceAction *>(content.get())->text_);
        }
        case td::td_api::messageContactRegistered::ID: {
            return QObject::tr("NotificationContactJoined")
                .arg(getUserName(static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_, store, openUser));
        }
        case td::td_api::messageUnsupported::ID: {
            return QObject::tr("UnsupportedMedia");
        }
    }

    return QObject::tr("UnsupportedMedia");
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

QString Utils::getUserShortName(qlonglong userId, StorageManager *store) noexcept
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
            return QObject::tr("HiddenName");
        }

        default:
            return QString();
    }
}

QString Utils::getTitle(const td::td_api::message &message, StorageManager *store) noexcept
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
        return getUserFullName(userId, store);
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
        return QString::fromStdString(store->getChat(chatId)->title_);
    }

    return {};
}

QString Utils::getMessageDate(const td::td_api::message &message) noexcept
{
    const auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(message.date_) * 1000);
    const auto days = date.daysTo(QDateTime::currentDateTime());

    if (days == 0)
    {
        return date.toString(QObject::tr("formatterDay12H"));
    }
    else if (days < 7)
    {
        return date.toString(QObject::tr("formatterWeek"));
    }

    return date.toString(QObject::tr("formatterYear"));
}

QString Utils::getContent(const td::td_api::message &message, StorageManager *storageManager) noexcept
{
    const auto *content = message.content_.get();
    const auto *sender = message.sender_id_.get();

    // Helper function to sanitize and convert std::string_view to QString
    const auto sanitizeText = [](std::string_view text) -> QString {
        QString result;
        result.reserve(static_cast<int>(text.size()));  // Reserve memory to avoid reallocations

        for (char ch : text)
        {
            result.append((ch == '\n' || ch == '\r') ? ' ' : QChar(ch));
        }
        return result;
    };

    // Helper function to create caption text from formattedText
    const auto captionText = [&](const td::td_api::formattedText &caption) -> QString { return QString(": ").append(sanitizeText(caption.text_)); };

    // Use structured bindings to handle each case efficiently
    switch (content->get_id())
    {
        case td::td_api::messageAnimation::ID: {
            const auto *animation = static_cast<const td::td_api::messageAnimation *>(content);
            return QObject::tr("AttachGif").append(captionText(*animation->caption_));
        }
        case td::td_api::messageAudio::ID: {
            const auto *audioMessage = static_cast<const td::td_api::messageAudio *>(content);
            QString title = getAudioTitle(*audioMessage->audio_);
            if (title.isEmpty())
            {
                title = QObject::tr("AttachMusic");
            }
            return title.append(captionText(*audioMessage->caption_));
        }
        case td::td_api::messageCall::ID: {
            const auto *call = static_cast<const td::td_api::messageCall *>(content);
            const auto callText = getCallContent(static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_, *call, storageManager);
            if (const auto duration = call->duration_; duration > 0)
            {
                return QObject::tr("CallMessageWithDuration").arg(callText).arg(Locale::instance().formatCallDuration(duration));
            }
            return callText;
        }
        case td::td_api::messageDocument::ID: {
            const auto *documentMessage = static_cast<const td::td_api::messageDocument *>(content);
            const auto &document = documentMessage->document_;
            const QString fileName = QString::fromStdString(document->file_name_);
            return QString(fileName.isEmpty() ? QObject::tr("AttachDocument") : fileName).append(captionText(*documentMessage->caption_));
        }
        case td::td_api::messageInvoice::ID: {
            const auto *invoice = static_cast<const td::td_api::messageInvoice *>(content);
            return sanitizeText(invoice->product_info_->title_);
        }
        case td::td_api::messageLocation::ID:
            return QObject::tr("AttachLocation");
        case td::td_api::messagePhoto::ID: {
            const auto *photoMessage = static_cast<const td::td_api::messagePhoto *>(content);
            return QObject::tr("AttachPhoto").append(captionText(*photoMessage->caption_));
        }
        case td::td_api::messagePoll::ID: {
            const auto *pollMessage = static_cast<const td::td_api::messagePoll *>(content);
            return QString::fromUtf8("\xf0\x9f\x93\x8a\x20").append(captionText(*pollMessage->poll_->question_));
        }
        case td::td_api::messageSticker::ID: {
            const auto *stickerMessage = static_cast<const td::td_api::messageSticker *>(content);
            return QObject::tr("AttachSticker").append(": ").append(QString::fromStdString(stickerMessage->sticker_->emoji_));
        }
        case td::td_api::messageText::ID: {
            const auto *textMessage = static_cast<const td::td_api::messageText *>(content);
            return sanitizeText(textMessage->text_->text_);
        }
        case td::td_api::messageVideo::ID: {
            const auto *videoMessage = static_cast<const td::td_api::messageVideo *>(content);
            return QObject::tr("AttachVideo").append(captionText(*videoMessage->caption_));
        }
        case td::td_api::messageVideoNote::ID:
            return QObject::tr("AttachRound");
        case td::td_api::messageVoiceNote::ID: {
            const auto *voiceNoteMessage = static_cast<const td::td_api::messageVoiceNote *>(content);
            return QObject::tr("AttachAudio").append(captionText(*voiceNoteMessage->caption_));
        }
        default:
            if (isServiceMessage(message))
            {
                return getServiceMessageContent(message, storageManager);
            }
            return QObject::tr("UnsupportedAttachment");
    }
}

bool Utils::isChatUnread(qlonglong chatId, StorageManager *store) noexcept
{
    const auto chat = store->getChat(chatId);

    return chat->is_marked_as_unread_ || chat->unread_count_ > 0;
}

QString Utils::getMessageSenderName(const td::td_api::message &message, StorageManager *store) noexcept
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
                        return QObject::tr("FromYou");

                    return getUserShortName(userId, store);
                }
                case td::td_api::messageSenderChat::ID: {
                    return getChatTitle(static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_, store);
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
