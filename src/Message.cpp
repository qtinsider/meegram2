#include "Message.hpp"

#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

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

QString getAudioTitle(const MessageAudio *audio) noexcept
{
    const auto &fileName = audio->fileName();
    const auto &title = audio->title().trimmed();
    const auto &performer = audio->performer().trimmed();

    if (title.isEmpty() && performer.isEmpty())
        return fileName;

    const auto artist = performer.isEmpty() ? "Unknown Artist" : performer;
    const auto track = title.isEmpty() ? "Unknown Track" : title;

    return artist + " - " + track;
}

}  // namespace

Message::Message(QObject *parent)
    : QObject(parent)
    , m_storageManager(&StorageManager::instance())
{
    connect(m_storageManager->client(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

Message::Message(td::td_api::message *message, QObject *parent)
    : QObject(parent)
    , m_message(message)
    , m_storageManager(&StorageManager::instance())
    , m_content(nullptr)
{
    connect(m_storageManager->client(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    m_chat = m_storageManager->getChat(message->chat_id_);

    setContent(std::move(m_message->content_));
}

qlonglong Message::id() const
{
    return m_message->id_;
}

qlonglong Message::chatId() const
{
    return m_message->chat_id_;
}

qlonglong Message::senderId() const
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        return static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        return static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
    }

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
    return m_message->is_outgoing_;
}

bool Message::isPinned() const
{
    return m_message->is_pinned_;
}

bool Message::isFromOffline() const
{
    return m_message->is_from_offline_;
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

QDateTime Message::date() const
{
    return QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(m_message->date_) * 1000);
}

QDateTime Message::editDate() const
{
    return QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(m_message->edit_date_) * 1000);
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

MessageContent *Message::content() const
{
    return m_content.get();
}

QVariantMap Message::replyMarkup() const
{
    return {};
}

QString Message::getServiceMessageContent() const
{
    auto &sender = m_message->sender_id_;
    auto &content = m_message->content_;
    auto isOutgoing = m_message->is_outgoing_;

    if (!content)
        return {};

    auto isChannel = isChannelChat(m_chat);

    auto author = getMessageAuthor(*m_message, m_storageManager, false);

    switch (content->get_id())
    {
        case td::td_api::messageExpiredPhoto::ID: {
            return tr("AttachPhotoExpired");
        }
        case td::td_api::messageExpiredVideo::ID: {
            return tr("AttachVideoExpired");
        }
        case td::td_api::messageBasicGroupChatCreate::ID: {
            if (isOutgoing)
            {
                return tr("ActionYouCreateGroup");
            }

            return tr("ActionCreateGroup").replace("un1", author);
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            if (isChannel)
            {
                return tr("ActionCreateChannel");
            }

            return tr("ActionCreateMega");
        }
        case td::td_api::messageChatChangeTitle::ID: {
            const auto title = QString::fromStdString(static_cast<const td::td_api::messageChatChangeTitle *>(content.get())->title_);

            if (isChannel)
            {
                return tr("ActionChannelChangedTitle").replace("un2", title);
            }

            if (isOutgoing)
            {
                return tr("ActionYouChangedTitle").replace("un2", title);
            }

            return tr("ActionChangedTitle").replace("un1", author).replace("un2", title);
        }
        case td::td_api::messageChatChangePhoto::ID: {
            if (isChannel)
            {
                return tr("ActionChannelChangedPhoto");
            }

            if (isOutgoing)
            {
                return tr("ActionYouChangedPhoto");
            }

            return tr("ActionChangedPhoto").replace("un1", author);
        }
        case td::td_api::messageChatDeletePhoto::ID: {
            if (isChannel)
            {
                return tr("ActionChannelRemovedPhoto");
            }

            if (isOutgoing)
            {
                return tr("ActionYouRemovedPhoto");
            }

            return tr("ActionRemovedPhoto").replace("un1", author);
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
                        return tr("ChannelJoined");
                    }

                    if (isSupergroup(m_chat) && !isChannel)
                    {
                        if (isMeUser(memberUserId, m_storageManager))
                        {
                            return tr("ChannelMegaJoined");
                        }

                        return tr("ActionAddUserSelfMega").replace("un1", author);
                    }

                    if (isOutgoing)
                    {
                        return tr("ActionAddUserSelfYou");
                    }

                    return tr("ActionAddUserSelf").replace("un1", author);
                }

                if (isOutgoing)
                {
                    return tr("ActionYouAddUser").replace("un2", getUserName(memberUserId, m_storageManager, false));
                }

                if (isMeUser(memberUserId, m_storageManager))
                {
                    if (isSupergroup(m_chat))
                    {
                        if (!isChannel)
                        {
                            return tr("MegaAddedBy").replace("un1", author);
                        }

                        return tr("ChannelAddedBy").replace("un1", author);
                    }

                    return tr("ActionAddUserYou").replace("un1", author);
                }

                return tr("ActionAddUser").replace("un1", author).replace("un2", getUserName(memberUserId, m_storageManager, false));
            }

            QStringList result;
            for (const auto &userId : memberUserIds)
            {
                result.append(getUserName(userId, m_storageManager, false));
            }

            auto users = result.join(", ");

            if (isOutgoing)
            {
                return tr("ActionYouAddUser").arg(users);
            }

            return tr("ActionAddUser").replace("un1", author).replace("un2", users);
        }
        case td::td_api::messageChatJoinByLink::ID: {
            if (isOutgoing)
            {
                return tr("ActionInviteYou");
            }

            return tr("ActionInviteUser").replace("un1", author);
        }
        case td::td_api::messageChatDeleteMember::ID: {
            auto userId = static_cast<const td::td_api::messageChatDeleteMember *>(content.get())->user_id_;

            if (userId == static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_)
            {
                if (isOutgoing)
                {
                    return tr("ActionYouLeftUser");
                }

                return tr("ActionLeftUser").replace("un1", author);
            }

            if (isOutgoing)
            {
                return tr("ActionYouKickUser").replace("un2", getUserName(userId, m_storageManager, false));
            }
            else if (isMeUser(userId, m_storageManager))
            {
                return tr("ActionKickUserYou").replace("un1", author);
            }

            return tr("ActionKickUser").replace("un1", author).replace("un2", getUserName(userId, m_storageManager, false));
        }
        case td::td_api::messageChatUpgradeTo::ID: {
            return tr("ActionMigrateFromGroup");
        }
        case td::td_api::messageChatUpgradeFrom::ID: {
            return tr("ActionMigrateFromGroup");
        }
        case td::td_api::messagePinMessage::ID: {
            return tr("ActionPinned").replace("un1", author);
        }
        case td::td_api::messageScreenshotTaken::ID: {
            if (isOutgoing)
            {
                return tr("ActionTakeScreenshootYou");
            }

            return tr("ActionTakeScreenshoot").replace("un1", author);
        }
        case td::td_api::messageCustomServiceAction::ID: {
            return QString::fromStdString(static_cast<const td::td_api::messageCustomServiceAction *>(content.get())->text_);
        }
        case td::td_api::messageContactRegistered::ID: {
            return tr("NotificationContactJoined")
                .arg(getUserName(static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_, m_storageManager, false));
        }
        case td::td_api::messageUnsupported::ID: {
            return tr("UnsupportedMedia");
        }
    }

    return tr("UnsupportedMedia");
}

bool Message::isService() const noexcept
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

    if (m_message)
        return serviceMessageTypes.contains(m_contentType);

    return false;
}

QString Message::senderType() const noexcept
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        return "user";
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        return "chat";
    }

    return {};
}

QString Message::getTitle() const noexcept
{
    const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
        return getUserFullName(userId, m_storageManager);
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
        return QString::fromStdString(m_storageManager->getChat(chatId)->title_);
    }

    return {};
}

QString Message::getContent() const noexcept
{
    QString result;

    switch (m_contentType)
    {
        case td::td_api::messageAnimation::ID: {
            const auto *animation = static_cast<const MessageAnimation *>(m_content.get());
            result = tr("AttachGif").append(animation->caption());
            break;
        }
        case td::td_api::messageAudio::ID: {
            const auto *audioMessage = static_cast<const MessageAudio *>(m_content.get());

            QString title = getAudioTitle(audioMessage);
            if (title.isEmpty())
            {
                title = tr("AttachMusic");
            }

            result = title.append(audioMessage->caption());

            break;
        }
        case td::td_api::messageCall::ID: {
            // const auto *call = static_cast<const td::td_api::messageCall *>(content);
            // const auto callText = getCallContent(static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_, *call, m_storageManager);
            // if (const auto duration = call->duration_; duration > 0)
            // {
            //     result =  tr("CallMessageWithDuration").arg(callText).arg(Locale::instance().formatCallDuration(duration));
            // }
            result = tr("CallMessageWithDuration");
            break;
        }
        case td::td_api::messageDocument::ID: {
            // const auto *documentMessage = static_cast<const td::td_api::messageDocument *>(content);
            // const auto &document = documentMessage->document_;
            // const QString fileName = QString::fromStdString(document->file_name_);
            result = tr("AttachDocument") /*QString(fileName.isEmpty() ? tr("AttachDocument") : fileName).append(captionText(*documentMessage->caption_))*/;
            break;
        }
        case td::td_api::messageInvoice::ID: {
            // const auto *invoice = static_cast<const td::td_api::messageInvoice *>(content);
            result = "Invioce" /*sanitizeText(invoice->product_info_->title_)*/;
            break;
        }
        case td::td_api::messageLocation::ID:
            result = tr("AttachLocation");
            break;
        case td::td_api::messagePhoto::ID: {
            const auto *photoMessage = static_cast<const MessagePhoto *>(m_content.get());
            result = tr("AttachPhoto").append(photoMessage->caption());
            break;
        }
        case td::td_api::messagePoll::ID: {
            // const auto *pollMessage = static_cast<const td::td_api::messagePoll *>(content);
            result = QString::fromUtf8("\xf0\x9f\x93\x8a\x20") /*.append(captionText(*pollMessage->poll_->question_))*/;
            break;
        }
        case td::td_api::messageSticker::ID: {
            // const auto *stickerMessage = static_cast<const td::td_api::messageSticker *>(content);
            result = tr("AttachSticker").append(": ") /*.append(QString::fromStdString(stickerMessage->sticker_->emoji_))*/;
            break;
        }
        case td::td_api::messageText::ID: {
            const auto *textMessage = static_cast<const MessageText *>(m_content.get());
            result = textMessage->text();
            break;
        }
        case td::td_api::messageVideo::ID: {
            const auto *videoMessage = static_cast<const MessageVideo *>(m_content.get());
            result = tr("AttachVideo").append(videoMessage->caption());
            break;
        }
        case td::td_api::messageVideoNote::ID:
            result = tr("AttachRound");
            break;
        case td::td_api::messageVoiceNote::ID: {
            const auto *voiceNoteMessage = static_cast<const MessageVoiceNote *>(m_content.get());
            result = tr("AttachAudio").append(voiceNoteMessage->caption());
            break;
        }
        default:
            if (isService())
            {
                result = getServiceMessageContent();
            }
            else
            {
                result = tr("UnsupportedAttachment");
            }
    }

    result.replace('\n', ' ');
    result.replace('\r', ' ');

    return result;
}

QString Message::getSenderName() const noexcept
{
    if (isService())
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
                        return tr("FromYou");

                    return Utils::getUserShortName(userId, m_storageManager);
                }
                case td::td_api::messageSenderChat::ID:
                    return Utils::getChatTitle(static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_, m_storageManager);

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

    setContent(std::move(m_message->content_));
}

void Message::setContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    m_content = nullptr;
    m_contentType = content->get_id();

    static const std::unordered_map<int32_t, std::function<std::unique_ptr<MessageContent>()>> contentFactory = {
        {td::td_api::messageText::ID, []() { return std::make_unique<MessageText>(); }},
        {td::td_api::messageAnimation::ID, []() { return std::make_unique<MessageAnimation>(); }},
        {td::td_api::messageAudio::ID, []() { return std::make_unique<MessageAudio>(); }},
        {td::td_api::messageDocument::ID, []() { return std::make_unique<MessageDocument>(); }},
        {td::td_api::messagePhoto::ID, []() { return std::make_unique<MessagePhoto>(); }},
        {td::td_api::messageSticker::ID, []() { return std::make_unique<MessageSticker>(); }},
        {td::td_api::messageVideo::ID, []() { return std::make_unique<MessageVideo>(); }},
        {td::td_api::messageVideoNote::ID, []() { return std::make_unique<MessageVideoNote>(); }},
        {td::td_api::messageVoiceNote::ID, []() { return std::make_unique<MessageVoiceNote>(); }},
        {td::td_api::messageLocation::ID, []() { return std::make_unique<MessageLocation>(); }},
        {td::td_api::messageVenue::ID, []() { return std::make_unique<MessageVenue>(); }},
        {td::td_api::messageContact::ID, []() { return std::make_unique<MessageContact>(); }},
        {td::td_api::messageAnimatedEmoji::ID, []() { return std::make_unique<MessageAnimatedEmoji>(); }},
        {td::td_api::messagePoll::ID, []() { return std::make_unique<MessagePoll>(); }},
        {td::td_api::messageInvoice::ID, []() { return std::make_unique<MessageInvoice>(); }},
        {td::td_api::messageCall::ID, []() { return std::make_unique<MessageCall>(); }},
    };

    if (auto it = contentFactory.find(m_contentType); it != contentFactory.end())
    {
        m_content = it->second();
    }

    if (m_content)
    {
        m_content->handleContent(std::move(content));
    }
    else
    {
        m_message->content_ = std::move(content);
    }

    emit messageChanged();
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
        emit messageChanged();
    }
}

void Message::handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId,
                                      td::td_api::object_ptr<td::td_api::error> &&error)
{
    if (m_message->chat_id_ == message->chat_id_ && m_message->id_ == oldMessageId)
    {
        emit messageChanged();
    }
}

void Message::handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        setContent(std::move(newContent));

        emit messageChanged();
    }
}

void Message::handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        m_message->edit_date_ = editDate;
        m_message->reply_markup_ = std::move(replyMarkup);
        emit messageChanged();
    }
}

void Message::handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        m_message->is_pinned_ = isPinned;
        emit messageChanged();
    }
}

void Message::handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo)
{
    if (m_message->chat_id_ == chatId && m_message->id_ == messageId)
    {
        m_message->interaction_info_ = std::move(interactionInfo);
        emit messageChanged();
    }
}
