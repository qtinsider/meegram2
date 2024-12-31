#include "Message.hpp"

#include "MessageService.hpp"

#include <functional>
#include <unordered_map>
#include <unordered_set>

Message::Message(td::td_api::object_ptr<td::td_api::message> message, QObject *parent)
    : QObject(parent)
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

Message::SenderType Message::senderType() const
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
