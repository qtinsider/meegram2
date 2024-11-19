#include "MessageModel.hpp"

#include "ChatManager.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "MessageService.hpp"
#include "StorageManager.hpp"

#include <QDateTime>
#include <QDebug>
#include <QLocale>

#include <algorithm>
#include <ranges>
#include <unordered_set>

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_client(StorageManager::instance().client())
    , m_storageManager(&StorageManager::instance())
{
    qDebug() << "MessageModel initialized.";

    connect(m_client.get(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    connect(m_storageManager, SIGNAL(basicGroupUpdated(qlonglong)), this, SLOT(handleBasicGroupUpdate(qlonglong)));
    connect(m_storageManager, SIGNAL(supergroupUpdated(qlonglong)), this, SLOT(handleSupergroupUpdate(qlonglong)));
    connect(m_storageManager, SIGNAL(userUpdated(qlonglong)), this, SLOT(handleUserUpdate(qlonglong)));

    setRoleNames(roleNames());
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_messages.size();
}

bool MessageModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid() || m_messages.empty() || m_loading)
        return false;

    const auto lastMessageId = m_chat->lastMessage()->id();
    const auto maxMessageId = std::ranges::max(m_messages | std::views::keys);

    return lastMessageId != maxMessageId;
}

void MessageModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid() || m_loading)
        return;

    m_loading = true;

    const auto maxMessageId = std::ranges::max(m_messages | std::views::keys);
    getChatHistory(maxMessageId, -MessageSliceLimit, MessageSliceLimit);

    emit loadingChanged();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_messages.size()))
        return QVariant();

    auto it = m_messages.begin();
    std::advance(it, index.row());
    const auto &message = it->second;

    switch (role)
    {
        case IdRole:
            return message->id();
        case SenderRole:
            return message->getSenderName();
        case ChatIdRole:
            return message->chatId();
        case IsOutgoingRole:
            return message->isOutgoing();
        case DateRole:
            return message->date().toString(QObject::tr("formatterDay12H"));
        case EditDateRole:
            return message->editDate().toString(QObject::tr("formatterDay12H"));
        case ContentRole: {
            if (auto content = message->content())
            {
                switch (message->contentType())
                {
                    case td::td_api::messageText::ID:
                        return QVariant::fromValue(static_cast<MessageText *>(content));
                    case td::td_api::messageAnimation::ID:
                        return QVariant::fromValue(static_cast<MessageAnimation *>(content));
                    case td::td_api::messageAudio::ID:
                        return QVariant::fromValue(static_cast<MessageAudio *>(content));
                    case td::td_api::messageDocument::ID:
                        return QVariant::fromValue(static_cast<MessageDocument *>(content));
                    case td::td_api::messagePhoto::ID:
                        return QVariant::fromValue(static_cast<MessagePhoto *>(content));
                    case td::td_api::messageSticker::ID:
                        return QVariant::fromValue(static_cast<MessageSticker *>(content));
                    case td::td_api::messageVideo::ID:
                        return QVariant::fromValue(static_cast<MessageVideo *>(content));
                    case td::td_api::messageVideoNote::ID:
                        return QVariant::fromValue(static_cast<MessageVideoNote *>(content));
                    case td::td_api::messageVoiceNote::ID:
                        return QVariant::fromValue(static_cast<MessageVoiceNote *>(content));
                    case td::td_api::messageLocation::ID:
                        return QVariant::fromValue(static_cast<MessageLocation *>(content));
                    case td::td_api::messageVenue::ID:
                        return QVariant::fromValue(static_cast<MessageVenue *>(content));
                    case td::td_api::messageContact::ID:
                        return QVariant::fromValue(static_cast<MessageContact *>(content));
                    case td::td_api::messageAnimatedEmoji::ID:
                        return QVariant::fromValue(static_cast<MessageAnimatedEmoji *>(content));
                    case td::td_api::messagePoll::ID:
                        return QVariant::fromValue(static_cast<MessagePoll *>(content));
                    case td::td_api::messageCall::ID:
                        return QVariant::fromValue(static_cast<MessageCall *>(content));
                    default:
                        return QVariant::fromValue(static_cast<MessageService *>(content));
                }
            }
            return QVariant();
        }
        case ContentTypeRole:
            return message->contentTypeString();
        case IsServiceRole:
            return message->isService();
        case SectionRole: {
            static const auto currentDateTime = QDateTime::currentDateTime();
            const int days = message->date().daysTo(currentDateTime);

            if (days == 0)
                return QObject::tr("MessageScheduleToday");
            else if (days == 1)
                return QObject::tr("Yesterday");
            else
                return message->date().toString(QObject::tr("chatFullDate"));
        }
    }

    return QVariant();
}

QHash<int, QByteArray> MessageModel::roleNames() const noexcept
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[SenderRole] = "sender";
    roles[ChatIdRole] = "chatId";
    roles[IsOutgoingRole] = "isOutgoing";
    roles[DateRole] = "date";
    roles[EditDateRole] = "editDate";
    roles[ContentRole] = "content";
    // Custom
    roles[ContentTypeRole] = "contentType";
    roles[IsServiceRole] = "isService";
    roles[SectionRole] = "section";
    return roles;
}

int MessageModel::count() const noexcept
{
    return m_messages.size();
}

bool MessageModel::backFetching() const noexcept
{
    return m_backFetching;
}

bool MessageModel::loading() const noexcept
{
    return m_loading;
}

ChatInfo *MessageModel::chatInfo() const noexcept
{
    return m_chatInfo.get();
}

Chat *MessageModel::chat() const noexcept
{
    return m_chat;
}

void MessageModel::setChat(Chat *value) noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (m_chat == value)
        return;

    if (m_chat)
    {
        disconnect(m_chat, SIGNAL(chatChanged()), this, SIGNAL(chatChanged()));
        disconnect(m_chat, SIGNAL(chatChanged()), this, SLOT(handleChatItem()));
    }

    m_chat = value;

    if (m_chat)
    {
        connect(m_chat, SIGNAL(chatChanged()), this, SIGNAL(chatChanged()));
        connect(m_chat, SIGNAL(chatChanged()), this, SLOT(handleChatItem()));
    }

    m_chatInfo = std::make_unique<ChatInfo>(m_chat, this);

    connect(m_chatInfo.get(), SIGNAL(infoChanged()), this, SIGNAL(chatInfoChanged()));

    emit chatChanged();
}

Message *MessageModel::getMessage(qlonglong messageId) const noexcept
{
    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        return it->second.get();
    }

    return nullptr;
}

void MessageModel::openChat() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_chat)
        return;

    m_client->send(td::td_api::make_object<td::td_api::openChat>(m_chat->id()), {});
}

void MessageModel::closeChat() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_chat)
        return;

    m_client->send(td::td_api::make_object<td::td_api::closeChat>(m_chat->id()), {});
}

void MessageModel::getChatHistory(qlonglong fromMessageId, int offset, int limit, bool previous) noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_chat)
        return;

    auto request = td::td_api::make_object<td::td_api::getChatHistory>();
    request->chat_id_ = m_chat->id();
    request->from_message_id_ = fromMessageId;
    request->offset_ = offset;
    request->limit_ = limit;
    request->only_local_ = false;

    m_client->send(std::move(request), [this, previous](auto &&response) {
        if (response->get_id() != td::td_api::messages::ID)
        {
            if (m_loading)
            {
                m_loading = false;
                emit loadingChanged();
            }

            if (m_backFetching)
            {
                m_backFetching = false;
                emit backFetchingChanged();
            }
            return;
        }

        auto messages = td::td_api::move_object_as<td::td_api::messages>(response);

        if (!messages || messages->messages_.empty())
        {
            if (m_loading)
            {
                m_loading = false;
                emit loadingChanged();
            }

            if (m_backFetching)
            {
                m_backFetching = false;
                emit backFetchingChanged();
            }
            return;
        }

        if (previous)
            emit backFetchable();

        std::vector<std::unique_ptr<Message>> newMessages;
        newMessages.reserve(messages->messages_.size());  // Reserve space for exact number of messages

        for (auto &&message : messages->messages_)
        {
            if (message)
            {
                newMessages.emplace_back(std::make_unique<Message>(std::move(message)));
            }
        }

        handleMessages(std::move(newMessages), previous);
    });
}

void MessageModel::sendMessage(const QString &message, qlonglong replyToMessageId) noexcept
{
    if (!m_chat)
        return;

    auto request = td::td_api::make_object<td::td_api::sendMessage>();

    auto inputMessageContent = td::td_api::make_object<td::td_api::inputMessageText>();

    inputMessageContent->text_ = td::td_api::make_object<td::td_api::formattedText>();
    inputMessageContent->text_->text_ = message.toStdString();

    request->chat_id_ = m_chat->id();

    if (replyToMessageId != 0)
    {
        request->reply_to_ = td::td_api::make_object<td::td_api::inputMessageReplyToMessage>(m_chat->id(), replyToMessageId, nullptr);
    }

    request->input_message_content_ = std::move(inputMessageContent);

    m_client->send(std::move(request), {});
}

bool MessageModel::canFetchMoreBack() const noexcept
{
    if (m_messages.empty() || m_backFetching)
        return false;

    return true;
}

void MessageModel::fetchMoreBack() noexcept
{
    if (m_backFetching)
        return;

    m_backFetching = true;

    getChatHistory(std::ranges::min(m_messages | std::views::keys), 0, MessageSliceLimit, true);

    emit backFetchingChanged();
}

void MessageModel::viewMessages(const QList<qlonglong> &messageIds) noexcept
{
    if (!m_chat)
        return;

    std::vector<int64_t> result;
    result.reserve(messageIds.size());

    for (const auto &id : messageIds)
    {
        result.emplace_back(id);
    }

    auto request = td::td_api::make_object<td::td_api::viewMessages>();

    request->chat_id_ = m_chat->id();
    request->message_ids_ = std::move(result);
    request->force_read_ = true;

    m_client->send(std::move(request), {});
}

void MessageModel::deleteMessage(qlonglong messageId, bool revoke) noexcept
{
    if (!m_chat)
        return;

    auto request = td::td_api::make_object<td::td_api::deleteMessages>();

    request->chat_id_ = m_chat->id();
    request->message_ids_ = std::move(std::vector<int64_t>(messageId));
    request->revoke_ = revoke;

    m_client->send(std::move(request), {});
}

void MessageModel::refresh() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
    if (m_messages.empty())
        return;

    m_loading = true;
    m_backFetching = false;

    beginResetModel();
    m_messages.clear();
    endResetModel();

    emit countChanged();
}

void MessageModel::classBegin()
{
    qDebug() << __PRETTY_FUNCTION__;
}

void MessageModel::componentComplete()
{
    qDebug() << __PRETTY_FUNCTION__;
    openChat();
    loadMessages();
}

void MessageModel::handleChatItem() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
}

void MessageModel::handleResult(td::td_api::Object *object) noexcept
{
    switch (object->get_id())
    {
        case td::td_api::updateNewMessage::ID: {
            auto update = static_cast<td::td_api::updateNewMessage *>(object);
            handleNewMessage(std::move(update->message_));
            break;
        }
        case td::td_api::updateMessageContent::ID: {
            auto update = static_cast<td::td_api::updateMessageContent *>(object);
            handleMessageContent(update->chat_id_, update->message_id_, std::move(update->new_content_));
            break;
        }
        case td::td_api::updateMessageEdited::ID: {
            auto update = static_cast<td::td_api::updateMessageEdited *>(object);
            handleMessageEdited(update->chat_id_, update->message_id_, update->edit_date_, std::move(update->reply_markup_));
            break;
        }
        case td::td_api::updateDeleteMessages::ID: {
            auto update = static_cast<td::td_api::updateDeleteMessages *>(object);
            handleDeleteMessages(update->chat_id_, std::move(update->message_ids_), update->is_permanent_, update->from_cache_);
            break;
        }
        case td::td_api::updateChatOnlineMemberCount::ID: {
            auto update = static_cast<td::td_api::updateChatOnlineMemberCount *>(object);
            handleChatOnlineMemberCount(update->chat_id_, update->online_member_count_);
            break;
        }
        default:
            break;
    }
}

void MessageModel::handleBasicGroupUpdate(qlonglong groupId) noexcept
{
    if (!m_chat || m_chat->typeId() != groupId)
        return;

    if (m_chat->type() == Chat::BasicGroup)
    {
        if (auto group = m_storageManager->getBasicGroup(groupId))
        {
            m_chatInfo->setBasicGroup(group);
        }
    }
}

void MessageModel::handleSupergroupUpdate(qlonglong groupId) noexcept
{
    if (!m_chat || m_chat->typeId() != groupId)
        return;

    if (m_chat->type() == Chat::Supergroup)
    {
        if (auto group = m_storageManager->getSupergroup(groupId))
        {
            m_chatInfo->setSupergroup(group);
        }
    }
}

void MessageModel::handleUserUpdate(qlonglong userId) noexcept
{
    if (!m_chat || m_chat->typeId() != userId)
        return;

    if (m_chat->type() == Chat::Private || m_chat->type() == Chat::Secret)
    {
        if (auto user = m_storageManager->getUser(userId))
        {
            m_chatInfo->setUser(user);
        }
    }
}

void MessageModel::handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message) noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_chat || m_chat->id() != message->chat_id_)
        return;

    const auto messageId = message->id_;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_messages.emplace(messageId, std::make_unique<Message>(std::move(message)));
    endInsertRows();

    emit countChanged();
}

void MessageModel::handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent) noexcept
{
    if (!m_chat || chatId != m_chat->id())
        return;

    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        it->second->setContent(std::move(newContent));

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate,
                                       td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup) noexcept
{
    Q_UNUSED(replyMarkup)

    if (!m_chat || chatId != m_chat->id())
        return;

    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        it->second->setEditDate(editDate);

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool isPermanent, bool fromCache) noexcept
{
    Q_UNUSED(isPermanent)
    Q_UNUSED(fromCache)

    if (!m_chat || chatId != m_chat->id())
        return;

    std::unordered_set idsSet(messageIds.begin(), messageIds.end());

    std::vector<int> indicesToRemove;

    for (const auto &[id, message] : m_messages)
    {
        if (idsSet.contains(id))
        {
            auto it = m_messages.find(id);
            auto index = std::ranges::distance(m_messages.begin(), it);
            indicesToRemove.emplace_back(index);
        }
    }

    std::ranges::sort(indicesToRemove, std::less<>());

    if (!indicesToRemove.empty())
    {
        beginRemoveRows(QModelIndex(), indicesToRemove.front(), indicesToRemove.back());

        std::erase_if(m_messages, [&idsSet](const auto &pair) { return idsSet.contains(pair.first); });

        endRemoveRows();
    }
}

void MessageModel::handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount) noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_chat || chatId != m_chat->id())
        return;

    m_onlineCount = onlineMemberCount;

    emit chatChanged();
}

void MessageModel::handleMessages(std::vector<std::unique_ptr<Message>> &&messages, bool previous) noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
    const int messageCount = static_cast<int>(messages.size());

    qDebug() << "handleMessages called. Number of messages received:" << messageCount;

    if (messageCount == 0)
    {
        if (m_loading)
        {
            m_loading = false;
            emit loadingChanged();
        }

        if (m_backFetching)
        {
            m_backFetching = false;
            emit backFetchingChanged();
        }
        return;
    }

    const int insertPos = previous ? 0 : rowCount();

    beginInsertRows(QModelIndex(), insertPos, insertPos + messageCount - 1);

    for (auto &message : messages)
    {
        m_messages.emplace(message->id(), std::move(message));
    }

    endInsertRows();

    if (previous)
    {
        emit backFetched(messageCount);
    }

    if (m_loading)
    {
        m_loading = false;
        emit loadingChanged();
    }

    if (m_backFetching)
    {
        m_backFetching = false;
        emit backFetchingChanged();
    }

    emit countChanged();
}

void MessageModel::loadMessages() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;
    if (!m_chat)
        return;

    const auto unread = m_chat->unreadCount() > 0;

    const auto fromMessageId = unread ? m_chat->lastReadInboxMessageId() : m_chat->lastMessage()->id();

    const auto offset = unread ? -1 - MessageSliceLimit : 0;
    const auto limit = unread ? 2 * MessageSliceLimit : MessageSliceLimit;

    getChatHistory(fromMessageId, offset, limit);
}

void MessageModel::itemChanged(size_t index) noexcept
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}
