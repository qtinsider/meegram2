#include "Message.hpp"

#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QStringList>

#include <algorithm>
#include <unordered_set>

namespace {

bool isMeUser(qlonglong userId, StorageManager *store) noexcept
{
    return store->myId() == userId;
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

QString getUserFullName(qlonglong userId, StorageManager *store, Locale *locale) noexcept
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

QString getCallContent(qlonglong userId, const td::td_api::messageCall &content, StorageManager *store, Locale *locale) noexcept
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

}  // namespace

Message::Message(QObject *parent)
    : QObject(parent)
    , m_locale(&Locale::instance())
    , m_storageManager(&StorageManager::instance())
{
    m_chat = m_storageManager->getChat(m_message->chat_id_);

    connect(m_storageManager->client(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

Message::Message(td::td_api::message *message, QObject *parent)
    : QObject(parent)
    , m_message(message)
    , m_locale(&Locale::instance())
    , m_storageManager(&StorageManager::instance())
{
    m_chat = m_storageManager->getChat(message->chat_id_);

    connect(m_storageManager->client(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

qlonglong Message::id() const
{
    return 0;
}

QVariantMap Message::senderId() const
{
    return {};
}

qlonglong Message::chatId() const
{
    return 0;
}

QVariantMap Message::sendingState() const
{
    return {};
}

QVariantMap Message::schedulingState() const
{
    return {};
}

bool Message::isOutgoing() const
{
    return false;
}

bool Message::isPinned() const
{
    return false;
}

bool Message::isFromOffline() const
{
    return false;
}

bool Message::canBeEdited() const
{
    return false;
}

bool Message::canBeForwarded() const
{
    return false;
}

bool Message::canBeRepliedInAnotherChat() const
{
    return false;
}

bool Message::canBeSaved() const
{
    return false;
}

bool Message::canBeDeletedOnlyForSelf() const
{
    return false;
}

bool Message::canBeDeletedForAllUsers() const
{
    return false;
}

bool Message::canGetAddedReactions() const
{
    return false;
}

bool Message::canGetStatistics() const
{
    return false;
}

bool Message::canGetMessageThread() const
{
    return false;
}

bool Message::canGetReadDate() const
{
    return false;
}

bool Message::canGetViewers() const
{
    return false;
}

bool Message::canGetMediaTimestampLinks() const
{
    return false;
}

bool Message::canReportReactions() const
{
    return false;
}

bool Message::hasTimestampedMedia() const
{
    return false;
}

bool Message::isChannelPost() const
{
    return false;
}

bool Message::isTopicMessage() const
{
    return false;
}

bool Message::containsUnreadMention() const
{
    return false;
}

qlonglong Message::date() const
{
    return 0;
}

qlonglong Message::editDate() const
{
    return 0;
}

QVariantMap Message::forwardInfo() const
{
    return {};
}

QVariantMap Message::importInfo() const
{
    return {};
}

QVariantMap Message::interactionInfo() const
{
    return {};
}

QVariantList Message::unreadReactions() const
{
    return {};
}

QVariantMap Message::factCheck() const
{
    return {};
}

QVariantMap Message::replyTo() const
{
    return {};
}

qlonglong Message::messageThreadId() const
{
    return 0;
}

qlonglong Message::savedMessagesTopicId() const
{
    return 0;
}

QVariantMap Message::selfDestructType() const
{
    return {};
}

double Message::selfDestructIn() const
{
    return 0.0;
}

double Message::autoDeleteIn() const
{
    return 0.0;
}

qlonglong Message::viaBotUserId() const
{
    return 0;
}

qlonglong Message::senderBusinessBotUserId() const
{
    return 0;
}

int Message::senderBoostCount() const
{
    return 0;
}

QString Message::authorSignature() const
{
    return {};
}

qlonglong Message::mediaAlbumId() const
{
    return 0;
}

qlonglong Message::effectId() const
{
    return 0;
}

QString Message::restrictionReason() const
{
    return {};
}

QVariantMap Message::content() const
{
    return {};
}

QVariantMap Message::replyMarkup() const
{
    return {};
}

QString Message::getServiceMessageContent()
{
    auto &sender = m_message->sender_id_;
    auto &content = m_message->content_;
    auto isOutgoing = m_message->is_outgoing_;

    auto isChannel = isChannelChat(m_chat);

    auto author = getMessageAuthor(*m_message, m_storageManager, m_locale, m_openUser);

    switch (content->get_id())
    {
        case td::td_api::messageExpiredPhoto::ID: {
            return m_locale->getString("AttachPhotoExpired");
        }
        case td::td_api::messageExpiredVideo::ID: {
            return m_locale->getString("AttachVideoExpired");
        }
        case td::td_api::messageBasicGroupChatCreate::ID: {
            if (isOutgoing)
            {
                return m_locale->getString("ActionYouCreateGroup");
            }

            return m_locale->getString("ActionCreateGroup").replace("un1", author);
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            if (isChannel)
            {
                return m_locale->getString("ActionCreateChannel");
            }

            return m_locale->getString("ActionCreateMega");
        }
        case td::td_api::messageChatChangeTitle::ID: {
            const auto title = QString::fromStdString(static_cast<const td::td_api::messageChatChangeTitle *>(content.get())->title_);

            if (isChannel)
            {
                return m_locale->getString("ActionChannelChangedTitle").replace("un2", title);
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouChangedTitle").replace("un2", title);
            }

            return m_locale->getString("ActionChangedTitle").replace("un1", author).replace("un2", title);
        }
        case td::td_api::messageChatChangePhoto::ID: {
            if (isChannel)
            {
                return m_locale->getString("ActionChannelChangedPhoto");
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouChangedPhoto");
            }

            return m_locale->getString("ActionChangedPhoto").replace("un1", author);
        }
        case td::td_api::messageChatDeletePhoto::ID: {
            if (isChannel)
            {
                return m_locale->getString("ActionChannelRemovedPhoto");
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouRemovedPhoto");
            }

            return m_locale->getString("ActionRemovedPhoto").replace("un1", author);
        }
        case td::td_api::messageChatAddMembers::ID: {
            const auto &memberUserIds = static_cast<const td::td_api::messageChatAddMembers *>(content.get())->member_user_ids_;

            if (memberUserIds.size() == 1)
            {
                auto memberUserId = memberUserIds[0];
                if (static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_ == memberUserId)
                {
                    if (isSupergroup(m_chat) && isChannel)
                    {
                        return m_locale->getString("ChannelJoined");
                    }

                    if (isSupergroup(m_chat) && !isChannel)
                    {
                        if (isMeUser(memberUserId, m_storageManager))
                        {
                            return m_locale->getString("ChannelMegaJoined");
                        }

                        return m_locale->getString("ActionAddUserSelfMega").replace("un1", author);
                    }

                    if (isOutgoing)
                    {
                        return m_locale->getString("ActionAddUserSelfYou");
                    }

                    return m_locale->getString("ActionAddUserSelf").replace("un1", author);
                }

                if (isOutgoing)
                {
                    return m_locale->getString("ActionYouAddUser").replace("un2", getUserName(memberUserId, m_storageManager, m_locale, m_openUser));
                }

                if (isMeUser(memberUserId, m_storageManager))
                {
                    if (isSupergroup(m_chat))
                    {
                        if (!isChannel)
                        {
                            return m_locale->getString("MegaAddedBy").replace("un1", author);
                        }

                        return m_locale->getString("ChannelAddedBy").replace("un1", author);
                    }

                    return m_locale->getString("ActionAddUserYou").replace("un1", author);
                }

                return m_locale->getString("ActionAddUser")
                    .replace("un1", author)
                    .replace("un2", getUserName(memberUserId, m_storageManager, m_locale, m_openUser));
            }

            QStringList result;
            for (const auto &userId : memberUserIds)
            {
                result << getUserName(userId, m_storageManager, m_locale, m_openUser);
            }

            auto users = result.join(", ");

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouAddUser").arg(users);
            }

            return m_locale->getString("ActionAddUser").replace("un1", author).replace("un2", users);
        }
        case td::td_api::messageChatJoinByLink::ID: {
            if (isOutgoing)
            {
                return m_locale->getString("ActionInviteYou");
            }

            return m_locale->getString("ActionInviteUser").replace("un1", author);
        }
        case td::td_api::messageChatDeleteMember::ID: {
            auto userId = static_cast<const td::td_api::messageChatDeleteMember *>(content.get())->user_id_;

            if (userId == static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_)
            {
                if (isOutgoing)
                {
                    return m_locale->getString("ActionYouLeftUser");
                }

                return m_locale->getString("ActionLeftUser").replace("un1", author);
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouKickUser").replace("un2", getUserName(userId, m_storageManager, m_locale, m_openUser));
            }
            else if (isMeUser(userId, m_storageManager))
            {
                return m_locale->getString("ActionKickUserYou").replace("un1", author);
            }

            return m_locale->getString("ActionKickUser").replace("un1", author).replace("un2", getUserName(userId, m_storageManager, m_locale, m_openUser));
        }
        case td::td_api::messageChatUpgradeTo::ID: {
            return m_locale->getString("ActionMigrateFromGroup");
        }
        case td::td_api::messageChatUpgradeFrom::ID: {
            return m_locale->getString("ActionMigrateFromGroup");
        }
        case td::td_api::messagePinMessage::ID: {
            return m_locale->getString("ActionPinned").replace("un1", author);
        }
        case td::td_api::messageScreenshotTaken::ID: {
            if (isOutgoing)
            {
                return m_locale->getString("ActionTakeScreenshootYou");
            }

            return m_locale->getString("ActionTakeScreenshoot").replace("un1", author);
        }
        case td::td_api::messageCustomServiceAction::ID: {
            return QString::fromStdString(static_cast<const td::td_api::messageCustomServiceAction *>(content.get())->text_);
        }
        case td::td_api::messageContactRegistered::ID: {
            return m_locale->getString("NotificationContactJoined")
                .arg(getUserName(static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_, m_storageManager, m_locale, m_openUser));
        }
        case td::td_api::messageUnsupported::ID: {
            return m_locale->getString("UnsupportedMedia");
        }
    }

    return m_locale->getString("UnsupportedMedia");
}

bool Message::isServiceMessage()
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

    if (m_message->content_)
    {
        const int contentId = m_message->content_->get_id();
        return serviceMessageTypes.contains(contentId);
    }

    return false;
}

QString Message::getTitle() noexcept
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
        return getUserFullName(userId, m_storageManager, m_locale);
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
        return QString::fromStdString(m_storageManager->getChat(chatId)->title_);
    }

    return {};
}

QString Message::getDate() noexcept
{
    const auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(m_message->date_) * 1000);
    const auto days = date.daysTo(QDateTime::currentDateTime());

    if (days == 0)
    {
        return date.toString(m_locale->getString("formatterDay12H"));
    }
    else if (days < 7)
    {
        return date.toString(m_locale->getString("formatterWeek"));
    }

    return date.toString(m_locale->getString("formatterYear"));
}

QString Message::getContent() noexcept
{
    const auto *content = m_message->content_.get();
    const auto *sender = m_message->sender_id_.get();

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
            return m_locale->getString("AttachGif").append(captionText(*animation->caption_));
        }
        case td::td_api::messageAudio::ID: {
            const auto *audioMessage = static_cast<const td::td_api::messageAudio *>(content);
            QString title = getAudioTitle(*audioMessage->audio_);
            if (title.isEmpty())
            {
                title = m_locale->getString("AttachMusic");
            }
            return title.append(captionText(*audioMessage->caption_));
        }
        case td::td_api::messageCall::ID: {
            const auto *call = static_cast<const td::td_api::messageCall *>(content);
            const auto callText = getCallContent(static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_, *call, m_storageManager, m_locale);
            if (const auto duration = call->duration_; duration > 0)
            {
                return m_locale->getString("CallMessageWithDuration").arg(callText).arg(m_locale->formatCallDuration(duration));
            }
            return callText;
        }
        case td::td_api::messageDocument::ID: {
            const auto *documentMessage = static_cast<const td::td_api::messageDocument *>(content);
            const auto &document = documentMessage->document_;
            const QString fileName = QString::fromStdString(document->file_name_);
            return QString(fileName.isEmpty() ? m_locale->getString("AttachDocument") : fileName).append(captionText(*documentMessage->caption_));
        }
        case td::td_api::messageInvoice::ID: {
            const auto *invoice = static_cast<const td::td_api::messageInvoice *>(content);
            return sanitizeText(invoice->product_info_->title_);
        }
        case td::td_api::messageLocation::ID:
            return m_locale->getString("AttachLocation");
        case td::td_api::messagePhoto::ID: {
            const auto *photoMessage = static_cast<const td::td_api::messagePhoto *>(content);
            return m_locale->getString("AttachPhoto").append(captionText(*photoMessage->caption_));
        }
        case td::td_api::messagePoll::ID: {
            const auto *pollMessage = static_cast<const td::td_api::messagePoll *>(content);
            return QString::fromUtf8("\xf0\x9f\x93\x8a\x20").append(captionText(*pollMessage->poll_->question_));
        }
        case td::td_api::messageSticker::ID: {
            const auto *stickerMessage = static_cast<const td::td_api::messageSticker *>(content);
            return m_locale->getString("AttachSticker").append(": ").append(QString::fromStdString(stickerMessage->sticker_->emoji_));
        }
        case td::td_api::messageText::ID: {
            const auto *textMessage = static_cast<const td::td_api::messageText *>(content);
            return sanitizeText(textMessage->text_->text_);
        }
        case td::td_api::messageVideo::ID: {
            const auto *videoMessage = static_cast<const td::td_api::messageVideo *>(content);
            return m_locale->getString("AttachVideo").append(captionText(*videoMessage->caption_));
        }
        case td::td_api::messageVideoNote::ID:
            return m_locale->getString("AttachRound");
        case td::td_api::messageVoiceNote::ID: {
            const auto *voiceNoteMessage = static_cast<const td::td_api::messageVoiceNote *>(content);
            return m_locale->getString("AttachAudio").append(captionText(*voiceNoteMessage->caption_));
        }
        default:
            if (isServiceMessage())
            {
                return getServiceMessageContent();
            }
            return m_locale->getString("UnsupportedAttachment");
    }
}

QString Message::getSenderName() noexcept
{
    if (isServiceMessage())
        return QString();

    const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

    switch (m_chat->type_->get_id())
    {
        case td::td_api::chatTypePrivate::ID:
        case td::td_api::chatTypeSecret::ID:
            return QString();

        case td::td_api::chatTypeBasicGroup::ID:
        case td::td_api::chatTypeSupergroup::ID: {
            if (isChannelChat(m_chat))
            {
                return QString();
            }

            switch (sender->get_id())
            {
                case td::td_api::messageSenderUser::ID: {
                    auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
                    if (isMeUser(userId, m_storageManager))
                        return m_locale->getString("FromYou");

                    return Utils::getUserShortName(userId, m_storageManager, m_locale);
                }
                case td::td_api::messageSenderChat::ID:
                    return Utils::getChatTitle(static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_, m_storageManager, m_locale);

                default:
                    return QString();
                    ;
            }
        }

        default:
            return QString();
    }
}

void Message::setMessage(td::td_api::message *message)
{
    m_message = message;

    m_chat = m_storageManager->getChat(m_message->chat_id_);

    emit dataChanged();
}

void Message::handleResult(td::td_api::Object *object)
{
    td::td_api::downcast_call(
        *object,
        detail::Overloaded{
            [this](td::td_api::updateMessageSendSucceeded &value) { handleMessageSendSucceeded(std::move(value.message_), value.old_message_id_); },
            [this](td::td_api::updateMessageSendFailed &value) {
                handleMessageSendFailed(std::move(value.message_), value.old_message_id_, std::move(value.error_));
            },
            [this](td::td_api::updateMessageContent &value) { handleMessageContent(value.chat_id_, value.message_id_, std::move(value.new_content_)); },
            [this](td::td_api::updateMessageEdited &value) {
                handleMessageEdited(value.chat_id_, value.message_id_, value.edit_date_, std::move(value.reply_markup_));
            },
            [this](td::td_api::updateMessageIsPinned &value) { handleMessageIsPinned(value.chat_id_, value.message_id_, value.is_pinned_); },
            [this](td::td_api::updateMessageInteractionInfo &value) {
                handleMessageInteractionInfo(value.chat_id_, value.message_id_, std::move(value.interaction_info_));
            },
            [](auto &) {}});
}

void Message::handleMessageSendSucceeded(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId)
{
    if (m_message->chat_id_ == message->chat_id_ && m_message->id_ == oldMessageId)
    {
        emit dataChanged();
    }
}

void Message::handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId,
                                      td::td_api::object_ptr<td::td_api::error> &&error)
{
    if (m_message->chat_id_ == message->chat_id_ && m_message->id_ == oldMessageId)
    {
        emit dataChanged();
    }
}

void Message::handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        m_message->content_ = std::move(newContent);
        emit dataChanged();
    }
}

void Message::handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        m_message->edit_date_ = editDate;
        m_message->reply_markup_ = std::move(replyMarkup);
        emit dataChanged();
    }
}

void Message::handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        m_message->is_pinned_ = isPinned;
        emit dataChanged();
    }
}

void Message::handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        m_message->interaction_info_ = std::move(interactionInfo);
        emit dataChanged();
    }
}
