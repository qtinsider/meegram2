#include "MessageModel.hpp"

#include "ChatManager.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "MessageService.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QDebug>
#include <QLocale>

#include <algorithm>
#include <ranges>

MessageModel::MessageModel(std::shared_ptr<Chat> chat, std::shared_ptr<Locale> locale, std::shared_ptr<StorageManager> storage)
    : m_client(storage->client())
    , m_locale(std::move(locale))
    , m_storage(std::move(storage))
    , m_chat(std::move(chat))
{
    qDebug() << "MessageModel initialized.";

    connect(m_chat.get(), SIGNAL(chatChanged()), SLOT(handleChatItem()));
    connect(m_client.get(), SIGNAL(result(td::td_api::Object *)), SLOT(handleResult(td::td_api::Object *)));

    setRoleNames(roleNames());

    loadMessages();
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
    const auto maxMessageId = std::ranges::max(m_messages);

    return lastMessageId != maxMessageId;
}

void MessageModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid() || m_messages.empty() || m_loading)
        return;

    m_loading = true;

    getChatHistory(std::ranges::max(m_messages), -MessageSliceLimit, MessageSliceLimit);

    emit loadingChanged();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_messages.size()))
        return QVariant();

    qlonglong messageId = m_messages[index.row()];
    const auto &message = m_messageMap.at(messageId);

    if (!message)
        return QVariant();

    switch (role)
    {
        case IdRole:
            return message->id();
        case SenderRole:
            return Utils::getSenderName(message.get(), m_storage);
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
        case ServiceMessageRole:
            return Utils::getServiceContent(message.get(), m_storage, m_locale, true);
        case SectionRole: {
            static const auto currentDateTime = QDateTime::currentDateTime();
            const auto days = message->date().daysTo(currentDateTime);

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
    roles[ServiceMessageRole] = "serviceMessage";
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

void MessageModel::getChatHistory(qlonglong fromMessageId, int offset, int limit, bool fetchPrevious) noexcept
{
    auto request = td::td_api::make_object<td::td_api::getChatHistory>();
    request->chat_id_ = m_chat->id();
    request->from_message_id_ = fromMessageId;
    request->offset_ = offset;
    request->limit_ = limit;
    request->only_local_ = false;

    m_client->send(std::move(request), [this, fetchPrevious](auto &&response) {
        auto cleanupFlags = [this]() {
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
        };

        if (response->get_id() != td::td_api::messages::ID)
        {
            cleanupFlags();
            return;
        }

        auto messagesResponse = td::td_api::move_object_as<td::td_api::messages>(response);
        if (!messagesResponse || messagesResponse->messages_.empty())
        {
            qDebug() << "No messages received or response is null, cleaning up flags";
            cleanupFlags();
            return;
        }

        std::vector<qlonglong> newMessageIds;
        for (auto &&message : messagesResponse->messages_)
        {
            const auto messageId = message->id_;
            if (!m_messageMap.contains(messageId))
            {
                newMessageIds.emplace_back(messageId);
                m_messageMap[messageId] = std::make_unique<Message>(std::move(message));
            }
        }

        if (!newMessageIds.empty())
        {
            insertMessages(std::move(newMessageIds), fetchPrevious);
        }

        cleanupFlags();
    });
}

void MessageModel::sendMessage(const QString &message, qlonglong replyToMessageId) noexcept
{
    auto request = td::td_api::make_object<td::td_api::sendMessage>();

    auto inputMessageContent = td::td_api::make_object<td::td_api::inputMessageText>();

    inputMessageContent->text_ = td::td_api::make_object<td::td_api::formattedText>();
    inputMessageContent->text_->text_ = message.toStdString();

    request->chat_id_ = m_chat->id();

    if (replyToMessageId != 0)
    {
        // request->reply_to_ = td::td_api::make_object<td::td_api::inputMessageReplyToMessage>(m_chat->id(), replyToMessageId, nullptr);
    }

    request->input_message_content_ = std::move(inputMessageContent);

    m_client->send(std::move(request));
}

void MessageModel::fetchMoreBack() noexcept
{
    if (m_backFetching || m_messages.empty())
        return;

    m_backFetching = true;

    getChatHistory(std::ranges::min(m_messages), 0, MessageSliceLimit, true);

    emit backFetchingChanged();
}

void MessageModel::viewMessages(const QStringList &messageIds) noexcept
{
    std::vector<int64_t> result;
    result.reserve(messageIds.size());

    std::ranges::for_each(messageIds, [&](const auto &messageId) {
        bool ok = false;
        if (auto id = messageId.toLongLong(&ok); ok)
        {
            result.emplace_back(id);
        }
    });

    auto request = td::td_api::make_object<td::td_api::viewMessages>();

    request->chat_id_ = m_chat->id();
    request->message_ids_ = std::move(result);
    request->force_read_ = true;

    m_client->send(std::move(request));
}

void MessageModel::deleteMessage(qlonglong messageId, bool revoke) noexcept
{
    auto request = td::td_api::make_object<td::td_api::deleteMessages>();

    request->chat_id_ = m_chat->id();
    request->message_ids_ = std::move(std::vector<int64_t>(messageId));
    request->revoke_ = revoke;

    m_client->send(std::move(request));
}

void MessageModel::refresh() noexcept
{
    if (m_messages.empty())
        return;

    m_loading = true;
    m_backFetching = true;

    beginResetModel();
    m_messages.clear();
    m_messageMap.clear();
    endResetModel();

    emit countChanged();
}

void MessageModel::handleChatItem() noexcept
{
    qDebug() << "Title" << m_chat->title();
    qDebug() << "Unread count" << m_chat->unreadCount();
    qDebug() << "Last read inbox message id" << m_chat->lastReadInboxMessageId();
    qDebug() << "Last read outbox message id" << m_chat->lastReadOutboxMessageId();
    qDebug() << "Unread mention count" << m_chat->unreadMentionCount();
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
        default:
            break;
    }
}

void MessageModel::handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message) noexcept
{
    if (m_chat->id() != message->chat_id_)
        return;

    auto messageId = message->id_;

    if (m_messageMap.contains(messageId))
        return;

    auto it = std::ranges::lower_bound(m_messages, messageId);
    auto pos = static_cast<int>(std::distance(m_messages.begin(), it));

    beginInsertRows(QModelIndex(), pos, pos);

    m_messages.insert(it, messageId);
    m_messageMap[messageId] = std::make_unique<Message>(std::move(message));

    endInsertRows();

    emit countChanged();
}

void MessageModel::handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent) noexcept
{
    if (chatId != m_chat->id())
        return;

    if (auto it = m_messageMap.find(messageId); it != m_messageMap.end())
    {
        it->second->setContent(std::move(newContent));

        itemChanged(std::distance(m_messages.begin(), std::ranges::find(m_messages, messageId)));
    }
}

void MessageModel::handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate,
                                       td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup) noexcept
{
    Q_UNUSED(replyMarkup)

    if (chatId != m_chat->id())
        return;

    if (auto it = m_messageMap.find(messageId); it != m_messageMap.end())
    {
        it->second->setEditDate(editDate);

        itemChanged(std::distance(m_messages.begin(), std::ranges::find(m_messages, messageId)));
    }
}

void MessageModel::handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool isPermanent, bool fromCache) noexcept
{
    Q_UNUSED(isPermanent)
    Q_UNUSED(fromCache)

    if (chatId != m_chat->id())
        return;

    std::unordered_set idsToDelete(messageIds.begin(), messageIds.end());

    std::vector<int> indicesToRemove;
    for (int i = 0; i < static_cast<int>(m_messages.size()); ++i)
    {
        if (idsToDelete.contains(m_messages[i]))
        {
            indicesToRemove.emplace_back(i);
        }
    }

    if (indicesToRemove.empty())
        return;

    std::ranges::sort(indicesToRemove, std::less<>());

    beginRemoveRows(QModelIndex(), indicesToRemove.front(), indicesToRemove.back());

    std::erase_if(m_messages, [&idsToDelete](const auto &id) { return idsToDelete.contains(id); });
    std::erase_if(m_messageMap, [&idsToDelete](const auto &pair) { return idsToDelete.contains(pair.first); });

    endRemoveRows();
}

void MessageModel::loadMessages() noexcept
{
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

void MessageModel::insertMessages(std::vector<qlonglong> &&newIds, bool prepend)
{
    if (newIds.empty())
        return;

    std::ranges::sort(newIds);

    if (prepend)
    {
        auto pos = static_cast<int>(newIds.size());
        beginInsertRows(QModelIndex(), 0, pos - 1);
        m_messages.insert(m_messages.begin(), newIds.begin(), newIds.end());
    }
    else
    {
        auto pos = static_cast<int>(m_messages.size());
        beginInsertRows(QModelIndex(), pos, pos + static_cast<int>(newIds.size()) - 1);
        m_messages.insert(m_messages.end(), newIds.begin(), newIds.end());
    }

    endInsertRows();

    auto mid = m_messages.begin() + (prepend ? newIds.size() : (m_messages.size() - newIds.size()));
    std::ranges::inplace_merge(m_messages.begin(), mid, m_messages.end());

    if (prepend)
    {
        emit fetchedPosition(static_cast<int>(newIds.size()));
    }
}

int MessageModel::lastMessageIndex() const noexcept
{
    const auto unread = m_chat->unreadCount() > 0;
    const auto fromMessageId = unread ? m_chat->lastReadInboxMessageId() : m_chat->lastMessage()->id();

    return std::distance(m_messages.begin(), std::ranges::find(m_messages, fromMessageId));
}
