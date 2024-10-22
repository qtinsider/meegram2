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
        const auto userName = "" /*Utils::getUserShortName(userId, store)*/;

        if (openUser)
        {
            return linkStyle + "userId://" + QString::number(userId) + "\">" + userName + linkClose;
        }
        return userName;
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        const auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
        const auto chatTitle = store->getChat(chatId)->title();

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
    // const auto user = store->getUser(userId);

    // switch (user->type_->get_id())
    // {
    //     case td::td_api::userTypeBot::ID:
    //     case td::td_api::userTypeRegular::ID: {
    //         // return QString::fromStdString(user->first_name_ + " " + user->last_name_).trimmed();
    //     }
    //     case td::td_api::userTypeDeleted::ID:
    //     case td::td_api::userTypeUnknown::ID: {
    //         return QObject::tr("HiddenName");
    //     }

    //     default:
    return QString();
    // }
}

QString getUserName(auto userId, StorageManager *store, bool openUser)
{
    QString userName = /*Utils::getUserShortName(userId, store)*/ "";
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

Message::Message(td::td_api::object_ptr<td::td_api::message> message, QObject *parent)
    : QObject(parent)
    , m_storageManager(&StorageManager::instance())
    , m_content(nullptr)
    , m_message(std::move(message))
{
    // m_chat = m_storageManager->getChat(message->chat_id_);

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

bool Message::isOutgoing() const
{
    return m_message->is_outgoing_;
}

QDateTime Message::date() const
{
    return QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(m_message->date_) * 1000);
}

QDateTime Message::editDate() const
{
    return QDateTime::fromMSecsSinceEpoch(static_cast<qlonglong>(m_message->edit_date_) * 1000);
}

MessageContent *Message::content() const
{
    return m_content.get();
}

QString Message::getServiceMessageContent() const
{
    switch (m_contentType)
    {
        case td::td_api::messageExpiredPhoto::ID: {
            return tr("AttachPhotoExpired");
        }
        case td::td_api::messageExpiredVideo::ID: {
            return tr("AttachVideoExpired");
        }
        case td::td_api::messageBasicGroupChatCreate::ID: {
            if (isOutgoing())
            {
                return tr("ActionYouCreateGroup");
            }

            return tr("ActionCreateGroup").replace("un1", "author");
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            return {};
        }
        case td::td_api::messageChatChangeTitle::ID: {
            return {};
        }
        case td::td_api::messageChatChangePhoto::ID: {
            return {};
        }
        case td::td_api::messageChatDeletePhoto::ID: {
            return {};
        }
        case td::td_api::messageChatAddMembers::ID: {
            return {};
        }
        case td::td_api::messageChatJoinByLink::ID: {
            return {};
        }
        case td::td_api::messageChatJoinByRequest::ID: {
            return {};
        }
        case td::td_api::messageChatDeleteMember::ID: {
            return {};
        }
        case td::td_api::messageChatUpgradeTo::ID: {
            return {};
        }
        case td::td_api::messageChatUpgradeFrom::ID: {
            return {};
        }
        case td::td_api::messagePinMessage::ID: {
            return {};
        }
        case td::td_api::messageScreenshotTaken::ID: {
            return {};
        }
        case td::td_api::messageChatSetBackground::ID: {
            return {};
        }
        case td::td_api::messageChatSetTheme::ID: {
            return {};
        }
        case td::td_api::messageChatSetMessageAutoDeleteTime::ID: {
            return {};
        }
        case td::td_api::messageChatBoost::ID: {
            return {};
        }
        case td::td_api::messageForumTopicCreated::ID: {
            return {};
        }
        case td::td_api::messageForumTopicEdited::ID: {
            return {};
        }
        case td::td_api::messageForumTopicIsClosedToggled::ID: {
            return {};
        }
        case td::td_api::messageForumTopicIsHiddenToggled::ID: {
            return {};
        }
        case td::td_api::messageSuggestProfilePhoto::ID: {
            return {};
        }
        case td::td_api::messageCustomServiceAction::ID: {
            return {};
        }
        case td::td_api::messageContactRegistered::ID: {
            return {};
        }
        case td::td_api::messageInviteVideoChatParticipants::ID: {
            return {};
        }
        case td::td_api::messageVideoChatScheduled::ID: {
            return {};
        }
        case td::td_api::messageVideoChatStarted::ID: {
            return {};
        }
        case td::td_api::messageVideoChatEnded::ID: {
            return {};
        }
        case td::td_api::messageGiftedPremium::ID: {
            return {};
        }
        case td::td_api::messagePremiumGiftCode::ID: {
            return {};
        }
        case td::td_api::messagePaymentSuccessful::ID: {
            return {};
        }
        case td::td_api::messagePaymentSuccessfulBot::ID: {
            return {};
        }
        case td::td_api::messageUsersShared::ID: {
            return {};
        }
        case td::td_api::messageChatShared::ID: {
            return {};
        }
        case td::td_api::messageBotWriteAccessAllowed::ID: {
            return {};
        }
        case td::td_api::messageWebAppDataSent::ID: {
            return {};
        }
        case td::td_api::messageWebAppDataReceived::ID: {
            return {};
        }
        case td::td_api::messagePassportDataSent::ID: {
            return {};
        }
        case td::td_api::messagePassportDataReceived::ID: {
            return {};
        }
        case td::td_api::messageProximityAlertTriggered::ID: {
            return {};
        }
        case td::td_api::messageUnsupported::ID: {
            return tr("UnsupportedMedia");
        }
        default: {
            return tr("UnsupportedMedia");
        }
    }
}

bool Message::isService() const noexcept
{
    const std::unordered_set<int> messageTypes = {
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

int Message::contentType() const
{
    return m_contentType;
}

QString Message::getTitle() const noexcept
{
    if (!m_chat)
        return {};

    const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

    if (sender->get_id() == td::td_api::messageSenderUser::ID)
    {
        auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
        return getUserFullName(userId, m_storageManager);
    }
    else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    {
        auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
        return m_storageManager->getChat(chatId)->title();
    }

    return {};
}

QString Message::getContent() const noexcept
{
    auto content = m_content.get();

    auto textOneLine = [](const QString &text) {
        auto result = text;
        result.replace("\n", " ");
        result.replace("\r", " ");
        return result;
    };

    auto getCaption = [&](const QString &text) { return !text.isEmpty() ? ": " + textOneLine(text) : textOneLine(text); };

    switch (m_contentType)
    {
        case td::td_api::messageText::ID: {
            auto text = static_cast<MessageText *>(content);
            return textOneLine(text->text());
        }
        case td::td_api::messageAnimation::ID: {
            auto animation = static_cast<MessageAnimation *>(content);

            return QObject::tr("AttachGif") + getCaption(animation->caption());
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

            return QObject::tr("AttachDocument") + getCaption(document->caption());
        }
        case td::td_api::messagePhoto::ID: {
            auto photo = static_cast<MessagePhoto *>(content);

            return tr("AttachPhoto") + getCaption(photo->caption());
        }
        case td::td_api::messageSticker::ID: {
            auto sticker = static_cast<MessageSticker *>(content);

            return QObject::tr("AttachSticker") + ": " + sticker->emoji();
        }
        case td::td_api::messageVideo::ID: {
            auto video = static_cast<MessageVideo *>(content);

            return tr("AttachVideo") + getCaption(video->caption());
        }
        case td::td_api::messageVideoNote::ID: {
            return QObject::tr("AttachRound");
        }
        case td::td_api::messageVoiceNote::ID: {
            auto voiceNote = static_cast<MessageVoiceNote *>(content);

            return QObject::tr("AttachAudio") + getCaption(voiceNote->caption());
        }
        case td::td_api::messageLocation::ID: {
            return QObject::tr("AttachLocation");
        }
        case td::td_api::messageVenue::ID: {
            auto venue = static_cast<MessageVenue *>(content);

            return QObject::tr("AttachLocation") + getCaption(venue->venue());
        }
        case td::td_api::messageContact::ID: {
            return QObject::tr("AttachContact");
        }
        case td::td_api::messageAnimatedEmoji::ID: {
            return {};
        }
        case td::td_api::messageDice::ID: {
            return {};
        }
        case td::td_api::messageGame::ID: {
            return {};
        }
        case td::td_api::messagePoll::ID: {
            auto poll = static_cast<MessagePoll *>(content);

            return QString::fromUtf8("\xf0\x9f\x93\x8a\x20") + poll->question();
        }
        case td::td_api::messageStory::ID: {
            return {};
        }
        case td::td_api::messageInvoice::ID: {
            return {};
        }
        case td::td_api::messageCall::ID: {
            return {};
        }
        case td::td_api::messageGameScore::ID: {
            return {};
        }
        default: {
            if (isService())
            {
                return getServiceMessageContent();
            }

            return tr("UnsupportedAttachment");
        }
    }
}

QString Message::getSenderName() const noexcept
{
    if (isService())
        return QString();

    if (!m_chat)
        return {};

    const auto sender = static_cast<const td::td_api::MessageSender *>(m_message->sender_id_.get());

    // switch (m_chat->getTypeId())
    // {
    //     case td::td_api::chatTypePrivate::ID:
    //     case td::td_api::chatTypeSecret::ID:
    //         return QString();

    //     case td::td_api::chatTypeBasicGroup::ID:
    //     case td::td_api::chatTypeSupergroup::ID: {
    //         if (isChannelChat(m_chat))
    //         {
    //             return QString();
    //         }

    //         switch (sender->get_id())
    //         {
    //             case td::td_api::messageSenderUser::ID: {
    //                 auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
    //                 if (isMeUser(userId, m_storageManager))
    //                     return tr("FromYou");

    //                 return /*Utils::getUserShortName(userId, m_storageManager)*/ QString();
    //             }
    //             case td::td_api::messageSenderChat::ID:
    //                 return /*Utils::getChatTitle(static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_, m_storageManager)*/ QString();

    //             default:
    //                 return QString();
    //         }
    //     }

    //     default:
    return QString();
    // }
}

void Message::setMessage(td::td_api::object_ptr<td::td_api::message> message)
{
    m_message = std::move(message);

    // m_chat = m_storageManager->getChat(m_message->chat_id_);

    setContent(std::move(m_message->content_));
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
