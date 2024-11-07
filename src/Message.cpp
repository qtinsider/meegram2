#include "Message.hpp"

#include "Localization.hpp"
#include "MessageService.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QStringList>

#include <algorithm>
#include <cmath>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

namespace {
QString getCallContent(const MessageCall *call, bool isOutgoing) noexcept
{
    const auto isVideo = call->isVideo();
    const auto isMissed = call->discardReason() == MessageCall::DiscardReason::Missed;
    const auto isBusy = call->discardReason() == MessageCall::DiscardReason::Declined;

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

    const auto artist = performer.isEmpty() ? QObject::tr("AudioUnknownArtist") : performer;
    const auto track = title.isEmpty() ? QObject::tr("AudioUnknownTitle") : title;

    return artist + " - " + track;
}
}  // namespace

Message::Message(td::td_api::object_ptr<td::td_api::message> message, QObject *parent)
    : QObject(parent)
    , m_storageManager(&StorageManager::instance())
    , m_content(nullptr)
    , m_message(std::move(message))
{
    m_id = m_message->id_;
    m_chatId = m_message->chat_id_;

    if (m_message->sender_id_)
    {
        const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

        switch (sender->get_id())
        {
            case td::td_api::messageSenderUser::ID: {
                m_senderId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
                m_senderType = SenderType::User;
                break;
            }
            case td::td_api::messageSenderChat::ID: {
                m_senderId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
                m_senderType = SenderType::Chat;
                break;
            }
            default: {
                m_senderId = 0;
                m_senderType = SenderType::Unknown;
                break;
            }
        }
    }

    m_isOutgoing = m_message->is_outgoing_;

    m_date = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(m_message->date_) * 1000);
    m_editDate = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(m_message->edit_date_) * 1000);

    setContent(std::move(m_message->content_));
}

qlonglong Message::id() const
{
    return m_id;
}

qlonglong Message::chatId() const
{
    return m_chatId;
}

qlonglong Message::senderId() const
{
    return m_senderId;
}

bool Message::isOutgoing() const
{
    return m_isOutgoing;
}

QDateTime Message::date() const
{
    return m_date;
}

QDateTime Message::editDate() const
{
    return m_editDate;
}

MessageContent *Message::content() const
{
    return m_content.get();
}

QString Message::getTitle() const noexcept
{
    switch (m_senderType)
    {
        case SenderType::User:
            return Utils::getUserFullName(m_senderId);

        case SenderType::Chat:
            return Utils::getChatTitle(m_senderId);

        default:
            return {};
    }
}

QString Message::getSenderName() const noexcept
{
    switch (m_senderType)
    {
        case SenderType::User:
            return Utils::getUserShortName(m_senderId);

        case SenderType::Chat:
            return Utils::getChatTitle(m_senderId);

        default:
            return {};
    }
}

QString Message::getContent() const noexcept
{
    auto content = m_content.get();

    auto textOneLine = [](const QString &text) noexcept -> QString {
        QString result = text;
        result.replace("\n", " ");
        result.replace("\r", " ");
        return result;
    };

    auto getCaption = [&](const QString &text) noexcept -> QString { return !text.isEmpty() ? ": " + textOneLine(text) : textOneLine(text); };

    switch (m_contentType)
    {
        case td::td_api::messageText::ID: {
            auto text = static_cast<MessageText *>(content);
            return textOneLine(text->text());
        }
        case td::td_api::messageAnimation::ID: {
            auto animation = static_cast<MessageAnimation *>(content);

            return tr("AttachGif") + getCaption(animation->caption());
        }
        case td::td_api::messageAudio::ID: {
            auto audio = static_cast<MessageAudio *>(content);

            QString title = getAudioTitle(audio);
            if (title.isEmpty())
            {
                title = tr("AttachMusic");
            }

            return title + getCaption(audio->caption());
        }
        case td::td_api::messageDocument::ID: {
            auto document = static_cast<MessageDocument *>(content);

            if (auto fileName = document->fileName(); !fileName.isEmpty())
            {
                return fileName + getCaption(document->caption());
            }

            return tr("AttachDocument") + getCaption(document->caption());
        }
        case td::td_api::messagePhoto::ID: {
            auto photo = static_cast<MessagePhoto *>(content);

            return tr("AttachPhoto") + getCaption(photo->caption());
        }
        case td::td_api::messageSticker::ID: {
            auto sticker = static_cast<MessageSticker *>(content);

            return sticker->emoji() + " " + tr("AttachSticker");
        }
        case td::td_api::messageVideo::ID: {
            auto video = static_cast<MessageVideo *>(content);

            return tr("AttachVideo") + getCaption(video->caption());
        }
        case td::td_api::messageVideoNote::ID: {
            return tr("AttachRound");
        }
        case td::td_api::messageVoiceNote::ID: {
            auto voiceNote = static_cast<MessageVoiceNote *>(content);

            return tr("AttachAudio") + getCaption(voiceNote->caption());
        }
        case td::td_api::messageLocation::ID: {
            return tr("AttachLocation");
        }
        case td::td_api::messageVenue::ID: {
            auto venue = static_cast<MessageVenue *>(content);

            return tr("AttachLocation") + getCaption(venue->venue());
        }
        case td::td_api::messageContact::ID: {
            return tr("AttachContact");
        }
        case td::td_api::messageAnimatedEmoji::ID: {
            return tr("AttachSticker");
        }
        case td::td_api::messageGame::ID: {
            return tr("AttachGame");
        }
        case td::td_api::messagePoll::ID: {
            auto poll = static_cast<MessagePoll *>(content);

            return QString::fromUtf8("\xf0\x9f\x93\x8a\x20") + " " + poll->question();
        }
        case td::td_api::messageCall::ID: {
            auto call = static_cast<MessageCall *>(content);

            auto text = getCallContent(call, m_isOutgoing);

            if (call->duration() > 0)
            {
                const auto callDuration = Locale::instance().formatCallDuration(call->duration());

                return tr("CallMessageWithDuration").arg(text).arg(callDuration);
            }

            return text;
        }
        default: {
            if (isService())
            {
                return getServiceContent();
            }

            return tr("UnsupportedAttachment");
        }
    }
}

QString Message::getServiceContent(bool openUser) const
{
    if (!m_storageManager)
        return {};

    auto chat = m_storageManager->getChat(m_chatId);
    if (!chat)
        return {};

    const auto author = getSenderAuthor(openUser);
    const bool isChannel = chat->type() == Chat::Type::Channel;
    auto message = static_cast<MessageService *>(m_content.get());

    switch (m_contentType)
    {
        case td::td_api::messageExpiredPhoto::ID: {
            return tr("AttachPhotoExpired");
        }
        case td::td_api::messageExpiredVideo::ID: {
            return tr("AttachVideoExpired");
        }
        case td::td_api::messageBasicGroupChatCreate::ID: {
            return m_isOutgoing ? tr("ActionYouCreateGroup") : tr("ActionCreateGroup").replace("un1", author);
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            return isChannel ? tr("ActionCreateChannel") : tr("ActionCreateMega");
        }
        case td::td_api::messageChatChangeTitle::ID: {
            return isChannel ? tr("ActionChannelChangedTitle").replace("un2", message->groupTitle())
                             : (m_isOutgoing ? tr("ActionYouChangedTitle").replace("un2", message->groupTitle())
                                             : tr("ActionChangedTitle").replace("un1", author).replace("un2", message->groupTitle()));
        }
        case td::td_api::messageChatChangePhoto::ID: {
            return isChannel ? tr("ActionChannelChangedPhoto") : (m_isOutgoing ? tr("ActionYouChangedPhoto") : tr("ActionChangedPhoto").replace("un1", author));
        }
        case td::td_api::messageChatDeletePhoto::ID: {
            return isChannel ? tr("ActionChannelRemovedPhoto") : (m_isOutgoing ? tr("ActionYouRemovedPhoto") : tr("ActionRemovedPhoto").replace("un1", author));
        }
        case td::td_api::messageChatAddMembers::ID: {
            const bool singleMember = message->addedMembers().size() == 1;
            if (singleMember)
            {
                bool ok = false;
                const auto memberUserId = message->addedMembers().front().toLongLong(&ok);
                if (!ok)
                    return {};

                if (m_senderId == memberUserId)
                {
                    if (chat->type() == Chat::Type::Supergroup)
                    {
                        return isChannel ? tr("ChannelJoined")
                                         : (Utils::isMeUser(memberUserId) ? tr("ChannelMegaJoined") : tr("ActionAddUserSelfMega").replace("un1", author));
                    }
                    return m_isOutgoing ? tr("ActionAddUserSelfYou") : tr("ActionAddUserSelf").replace("un1", author);
                }
                return m_isOutgoing ? tr("ActionYouAddUser").replace("un2", Utils::getUserName(memberUserId, openUser))
                                    : (Utils::isMeUser(memberUserId)
                                           ? (isChannel ? tr("ChannelAddedBy").replace("un1", author) : tr("MegaAddedBy").replace("un1", author))
                                           : tr("ActionAddUser").replace("un1", author).replace("un2", Utils::getUserName(memberUserId, openUser)));
            }

            QStringList userList;
            std::ranges::for_each(message->addedMembers(), [&](const auto &userId) {
                bool ok = false;
                if (auto id = userId.toLongLong(&ok); ok)
                {
                    userList.append(Utils::getUserName(id, openUser));
                }
            });

            const QString users = userList.join(", ");
            return m_isOutgoing ? tr("ActionYouAddUser").replace("un1", users) : tr("ActionAddUser").replace("un1", author).replace("un2", users);
        }
        case td::td_api::messageChatJoinByLink::ID: {
            return m_isOutgoing ? tr("ActionInviteYou") : tr("ActionInviteUser").replace("un1", author);
        }
        case td::td_api::messageChatDeleteMember::ID: {
            if (message->removedMember() == m_senderId)
            {
                return m_isOutgoing ? tr("ActionYouLeftUser") : tr("ActionLeftUser").replace("un1", author);
            }
            return m_isOutgoing ? tr("ActionYouKickUser").replace("un2", Utils::getUserName(message->removedMember(), openUser))
                                : (Utils::isMeUser(message->removedMember())
                                       ? tr("ActionKickUserYou").replace("un1", author)
                                       : tr("ActionKickUser").replace("un1", author).replace("un2", Utils::getUserName(message->removedMember(), openUser)));
        }
        case td::td_api::messageChatUpgradeTo::ID:
        case td::td_api::messageChatUpgradeFrom::ID: {
            return tr("ActionMigrateFromGroup");
        }
        case td::td_api::messagePinMessage::ID: {
            return tr("ActionPinned").replace("un1", author);
        }
        case td::td_api::messageScreenshotTaken::ID: {
            return m_isOutgoing ? tr("ActionTakeScreenshootYou") : tr("ActionTakeScreenshoot").replace("un1", author);
        }
        case td::td_api::messageChatSetMessageAutoDeleteTime::ID: {
            const auto autoDeleteTime = message->autoDeleteTime();
            const auto ttlString = Locale::instance().formatTtl(autoDeleteTime);

            QString actionKey = m_isOutgoing ? (autoDeleteTime <= 0 ? tr("ActionTTLYouDisabled") : tr("ActionTTLYouChanged").arg(ttlString))
                                             : (autoDeleteTime <= 0 ? tr("ActionTTLDisabled") : tr("ActionTTLChanged").arg(ttlString));

            return actionKey.replace("un1", author);
        }
        case td::td_api::messageCustomServiceAction::ID: {
            return message->customAction();
        }
        case td::td_api::messageContactRegistered::ID: {
            return tr("NotificationContactJoined").arg(Utils::getUserName(m_senderId, openUser));
        }
        default: {
            return tr("UnsupportedMedia");
        }
    }
}

int Message::contentType() const
{
    return m_contentType;
}

QString Message::contentTypeString() const
{
    static const std::unordered_map<int32_t, std::string_view> messageTypeMap = {
        {td::td_api::messageText::ID, "messageText"},
        {td::td_api::messageAudio::ID, "messageAudio"},
        {td::td_api::messageDocument::ID, "messageDocument"},
        {td::td_api::messagePhoto::ID, "messagePhoto"},
        {td::td_api::messageSticker::ID, "messageSticker"},
        {td::td_api::messageVideo::ID, "messageVideo"},
        {td::td_api::messageVideoNote::ID, "messageVideoNote"},
        {td::td_api::messageVoiceNote::ID, "messageVoiceNote"},
        {td::td_api::messageExpiredPhoto::ID, "messageExpiredPhoto"},
        {td::td_api::messageExpiredVideo::ID, "messageExpiredVideo"},
        {td::td_api::messageExpiredVideoNote::ID, "messageExpiredVideoNote"},
        {td::td_api::messageExpiredVoiceNote::ID, "messageExpiredVoiceNote"},
        {td::td_api::messageLocation::ID, "messageLocation"},
        {td::td_api::messageVenue::ID, "messageVenue"},
        {td::td_api::messageContact::ID, "messageContact"},
        {td::td_api::messageAnimatedEmoji::ID, "messageAnimatedEmoji"},
        {td::td_api::messageDice::ID, "messageDice"},
        {td::td_api::messageGame::ID, "messageGame"},
        {td::td_api::messagePoll::ID, "messagePoll"},
        {td::td_api::messageStory::ID, "messageStory"},
        {td::td_api::messageInvoice::ID, "messageInvoice"},
        {td::td_api::messageCall::ID, "messageCall"},
        {td::td_api::messageVideoChatScheduled::ID, "messageVideoChatScheduled"},
        {td::td_api::messageVideoChatStarted::ID, "messageVideoChatStarted"},
        {td::td_api::messageVideoChatEnded::ID, "messageVideoChatEnded"},
        {td::td_api::messageInviteVideoChatParticipants::ID, "messageInviteVideoChatParticipants"},
        {td::td_api::messageBasicGroupChatCreate::ID, "messageBasicGroupChatCreate"},
        {td::td_api::messageSupergroupChatCreate::ID, "messageSupergroupChatCreate"},
        {td::td_api::messageChatChangeTitle::ID, "messageChatChangeTitle"},
        {td::td_api::messageChatChangePhoto::ID, "messageChatChangePhoto"},
        {td::td_api::messageChatDeletePhoto::ID, "messageChatDeletePhoto"},
        {td::td_api::messageChatAddMembers::ID, "messageChatAddMembers"},
        {td::td_api::messageChatJoinByLink::ID, "messageChatJoinByLink"},
        {td::td_api::messageChatJoinByRequest::ID, "messageChatJoinByRequest"},
        {td::td_api::messageChatDeleteMember::ID, "messageChatDeleteMember"},
        {td::td_api::messageChatUpgradeTo::ID, "messageChatUpgradeTo"},
        {td::td_api::messageChatUpgradeFrom::ID, "messageChatUpgradeFrom"},
        {td::td_api::messagePinMessage::ID, "messagePinMessage"},
        {td::td_api::messageScreenshotTaken::ID, "messageScreenshotTaken"},
        {td::td_api::messageChatSetBackground::ID, "messageChatSetBackground"},
        {td::td_api::messageChatSetTheme::ID, "messageChatSetTheme"},
        {td::td_api::messageChatSetMessageAutoDeleteTime::ID, "messageChatSetMessageAutoDeleteTime"},
        {td::td_api::messageChatBoost::ID, "messageChatBoost"},
        {td::td_api::messageForumTopicCreated::ID, "messageForumTopicCreated"},
        {td::td_api::messageForumTopicEdited::ID, "messageForumTopicEdited"},
        {td::td_api::messageForumTopicIsClosedToggled::ID, "messageForumTopicIsClosedToggled"},
        {td::td_api::messageForumTopicIsHiddenToggled::ID, "messageForumTopicIsHiddenToggled"},
        {td::td_api::messageSuggestProfilePhoto::ID, "messageSuggestProfilePhoto"},
        {td::td_api::messageCustomServiceAction::ID, "messageCustomServiceAction"},
        {td::td_api::messageGameScore::ID, "messageGameScore"},
        {td::td_api::messagePaymentSuccessful::ID, "messagePaymentSuccessful"},
        {td::td_api::messagePaymentSuccessfulBot::ID, "messagePaymentSuccessfulBot"},
        {td::td_api::messageGiftedPremium::ID, "messageGiftedPremium"},
        {td::td_api::messagePremiumGiftCode::ID, "messagePremiumGiftCode"},
        {td::td_api::messageContactRegistered::ID, "messageContactRegistered"},
        {td::td_api::messageUsersShared::ID, "messageUsersShared"},
        {td::td_api::messageChatShared::ID, "messageChatShared"},
        {td::td_api::messageBotWriteAccessAllowed::ID, "messageBotWriteAccessAllowed"},
        {td::td_api::messageWebAppDataSent::ID, "messageWebAppDataSent"},
        {td::td_api::messageWebAppDataReceived::ID, "messageWebAppDataReceived"},
        {td::td_api::messagePassportDataSent::ID, "messagePassportDataSent"},
        {td::td_api::messagePassportDataReceived::ID, "messagePassportDataReceived"},
        {td::td_api::messageProximityAlertTriggered::ID, "messageProximityAlertTriggered"},
        {td::td_api::messageUnsupported::ID, "messageUnsupported"}};

    if (auto it = messageTypeMap.find(m_contentType); it != messageTypeMap.end())
    {
        return QString::fromStdString(std::string(it->second));
    }
    else
    {
        return QLatin1String("messageUnsupported");
    }
}

bool Message::isService() const noexcept
{
    static const std::unordered_set<int> messageTypes = {
        td::td_api::messageExpiredPhoto::ID,
        td::td_api::messageExpiredVideo::ID,
        td::td_api::messageBasicGroupChatCreate::ID,
        td::td_api::messageSupergroupChatCreate::ID,
        td::td_api::messageChatChangeTitle::ID,
        td::td_api::messageChatChangePhoto::ID,
        td::td_api::messageChatDeletePhoto::ID,
        td::td_api::messageChatAddMembers::ID,
        td::td_api::messageChatJoinByLink::ID,
        td::td_api::messageChatJoinByRequest::ID,
        td::td_api::messageChatDeleteMember::ID,
        td::td_api::messageChatUpgradeTo::ID,
        td::td_api::messageChatUpgradeFrom::ID,
        td::td_api::messagePinMessage::ID,
        td::td_api::messageScreenshotTaken::ID,
        td::td_api::messageChatSetBackground::ID,
        td::td_api::messageChatSetTheme::ID,
        td::td_api::messageChatSetMessageAutoDeleteTime::ID,
        td::td_api::messageChatBoost::ID,
        td::td_api::messageForumTopicCreated::ID,
        td::td_api::messageForumTopicEdited::ID,
        td::td_api::messageForumTopicIsClosedToggled::ID,
        td::td_api::messageForumTopicIsHiddenToggled::ID,
        td::td_api::messageSuggestProfilePhoto::ID,
        td::td_api::messageCustomServiceAction::ID,
        td::td_api::messageContactRegistered::ID,
        td::td_api::messageInviteVideoChatParticipants::ID,
        td::td_api::messageVideoChatScheduled::ID,
        td::td_api::messageVideoChatStarted::ID,
        td::td_api::messageVideoChatEnded::ID,
        td::td_api::messageGiftedPremium::ID,
        td::td_api::messagePremiumGiftCode::ID,
        td::td_api::messagePaymentSuccessful::ID,
        td::td_api::messagePaymentSuccessfulBot::ID,
        td::td_api::messageUsersShared::ID,
        td::td_api::messageChatShared::ID,
        td::td_api::messageBotWriteAccessAllowed::ID,
        td::td_api::messageWebAppDataSent::ID,
        td::td_api::messageWebAppDataReceived::ID,
        td::td_api::messagePassportDataSent::ID,
        td::td_api::messagePassportDataReceived::ID,
        td::td_api::messageProximityAlertTriggered::ID,
        td::td_api::messageUnsupported::ID,
    };

    if (m_message)
        return messageTypes.contains(m_contentType);

    return false;
}

Message::SenderType Message::senderType() const noexcept
{
    return m_senderType;
}

void Message::setContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    m_content = nullptr;
    m_contentType = content->get_id();

    using MessageContentHandler = std::function<std::unique_ptr<MessageContent>(td::td_api::object_ptr<td::td_api::MessageContent>)>;
    static const std::unordered_map<int32_t, MessageContentHandler> contentFactory = {
        {td::td_api::messageText::ID, [](auto content) { return std::make_unique<MessageText>(td::td_api::move_object_as<td::td_api::messageText>(content)); }},
        {td::td_api::messageAnimation::ID,
         [](auto content) { return std::make_unique<MessageAnimation>(td::td_api::move_object_as<td::td_api::messageAnimation>(content)); }},
        {td::td_api::messageAudio::ID,
         [](auto content) { return std::make_unique<MessageAudio>(td::td_api::move_object_as<td::td_api::messageAudio>(content)); }},
        {td::td_api::messageDocument::ID,
         [](auto content) { return std::make_unique<MessageDocument>(td::td_api::move_object_as<td::td_api::messageDocument>(content)); }},
        {td::td_api::messagePhoto::ID,
         [](auto content) { return std::make_unique<MessagePhoto>(td::td_api::move_object_as<td::td_api::messagePhoto>(content)); }},
        {td::td_api::messageSticker::ID,
         [](auto content) { return std::make_unique<MessageSticker>(td::td_api::move_object_as<td::td_api::messageSticker>(content)); }},
        {td::td_api::messageVideo::ID,
         [](auto content) { return std::make_unique<MessageVideo>(td::td_api::move_object_as<td::td_api::messageVideo>(content)); }},
        {td::td_api::messageVideoNote::ID,
         [](auto content) { return std::make_unique<MessageVideoNote>(td::td_api::move_object_as<td::td_api::messageVideoNote>(content)); }},
        {td::td_api::messageVoiceNote::ID,
         [](auto content) { return std::make_unique<MessageVoiceNote>(td::td_api::move_object_as<td::td_api::messageVoiceNote>(content)); }},
        {td::td_api::messageLocation::ID,
         [](auto content) { return std::make_unique<MessageLocation>(td::td_api::move_object_as<td::td_api::messageLocation>(content)); }},
        {td::td_api::messageVenue::ID,
         [](auto content) { return std::make_unique<MessageVenue>(td::td_api::move_object_as<td::td_api::messageVenue>(content)); }},
        {td::td_api::messageContact::ID,
         [](auto content) { return std::make_unique<MessageContact>(td::td_api::move_object_as<td::td_api::messageContact>(content)); }},
        {td::td_api::messageAnimatedEmoji::ID,
         [](auto content) { return std::make_unique<MessageAnimatedEmoji>(td::td_api::move_object_as<td::td_api::messageAnimatedEmoji>(content)); }},
        {td::td_api::messagePoll::ID, [](auto content) { return std::make_unique<MessagePoll>(td::td_api::move_object_as<td::td_api::messagePoll>(content)); }},
        {td::td_api::messageInvoice::ID,
         [](auto content) { return std::make_unique<MessageInvoice>(td::td_api::move_object_as<td::td_api::messageInvoice>(content)); }},
        {td::td_api::messageCall::ID,
         [](auto content) { return std::make_unique<MessageCall>(td::td_api::move_object_as<td::td_api::messageCall>(content)); }}};

    if (auto it = contentFactory.find(m_contentType); it != contentFactory.end())
    {
        m_content = it->second(std::move(content));
    }
    else
    {
        m_content = std::make_unique<MessageService>(std::move(content));
    }

    emit messageChanged();
}

void Message::setEditDate(int editDate)
{
    m_editDate = QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(editDate) * 1000);

    emit messageChanged();
}

QString Message::getGroupSenderName() const noexcept
{
    switch (m_senderType)
    {
        case SenderType::User: {
            if (Utils::isMeUser(m_senderId))
                return tr("FromYou");
            return Utils::getUserShortName(m_senderId);
        }
        case SenderType::Chat: {
            return Utils::getChatTitle(m_senderId);
        }
        default:
            return QString();
    }
}

QString Message::getSenderAuthor(bool openUser) const noexcept
{
    const QString linkStyle = QLatin1String("<a style=\"text-decoration: none; font-weight: bold; color: darkgray\" href=\"");
    const QString linkClose = QLatin1String("</a>");

    switch (m_senderType)
    {
        case SenderType::User: {
            const auto userName = Utils::getUserShortName(m_senderId);

            if (openUser)
            {
                return linkStyle + QLatin1String("userId://") + QString::number(m_senderId) + QLatin1String("\">") + userName + linkClose;
            }

            return userName;
        }
        case SenderType::Chat: {
            const auto chatTitle = Utils::getChatTitle(m_senderId);

            if (openUser)
            {
                return linkStyle + QLatin1String("chatId://") + QString::number(m_senderId) + QLatin1String("\">") + chatTitle + linkClose;
            }
            return chatTitle;
        }
        default:
            return {};
    }
}
