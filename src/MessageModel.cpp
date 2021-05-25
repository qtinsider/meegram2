#include "MessageModel.hpp"

#include "Common.hpp"
#include "TdApi.hpp"
#include "Utils.hpp"

#include <QDateTime>

#include <algorithm>

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    connect(&TdApi::getInstance(), SIGNAL(updateNewMessage(const QVariantMap &)), SLOT(handleNewMessage(const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateMessageSendSucceeded(const QVariantMap &, qint64)),
            SLOT(handleMessageSendSucceeded(const QVariantMap &, qint64)));
    connect(&TdApi::getInstance(), SIGNAL(updateMessageSendFailed(const QVariantMap &, qint64, int, const QString &)),
            SLOT(handleMessageSendFailed(const QVariantMap &, qint64, int, const QString &)));
    connect(&TdApi::getInstance(), SIGNAL(updateMessageContent(qint64, qint64, const QVariantMap &)),
            SLOT(handleMessageContent(qint64, qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateMessageEdited(qint64, qint64, int, const QVariantMap &)),
            SLOT(handleMessageEdited(qint64, qint64, int, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateMessageIsPinned(qint64, qint64, bool)), SLOT(handleMessageIsPinned(qint64, qint64, bool)));
    connect(&TdApi::getInstance(), SIGNAL(updateMessageInteractionInfo(qint64, qint64, const QVariantMap &)),
            SLOT(handleMessageInteractionInfo(qint64, qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateDeleteMessages(qint64, const QVariantList &, bool, bool)),
            SLOT(handleDeleteMessages(qint64, const QVariantList &, bool, bool)));

    connect(&TdApi::getInstance(), SIGNAL(updateChatReadInbox(qint64, qint64, int)), SLOT(handleChatReadInbox(qint64, qint64, int)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatReadOutbox(qint64, qint64)), SLOT(handleChatReadOutbox(qint64, qint64)));

    connect(&TdApi::getInstance(), SIGNAL(message(const QVariantMap &)), SLOT(handleMessage(const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(messages(const QVariantMap &)), SLOT(handleMessages(const QVariantMap &)));

    setRoleNames(roleNames());
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_messages.count();
}

bool MessageModel::canFetchMore(const QModelIndex &parent) const
{
    auto lastMessageId = m_chat.value("last_message").toMap().value("id").toLongLong();

    if (m_messages.size() > 0)
        return !m_refreshing && (lastMessageId != *std::ranges::max_element(m_uniqueIds));

    Q_UNUSED(parent)

    return false;
}

void MessageModel::fetchMore(const QModelIndex &parent)
{
    if (!m_refreshing)
    {
        auto max = std::ranges::max_element(m_uniqueIds);

        if (max != m_uniqueIds.end())
        {
            TdApi::getInstance().getChatHistory(m_chatId, *max, -MessageSliceLimit, MessageSliceLimit, false);
        }
        m_refreshing = true;
        emit refreshingChanged();
    }
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (const auto &message = m_messages.at(index.row()); role)
    {
        case IdRole:
            return message.value("id").toString();
        case SenderRole:
            return Utils::getMessageSenderName(message);
        case ChatIdRole:
            return message.value("chat_id").toString();
        case SendingStateRole:
            return message.value("sending_state").toMap();
        case SchedulingStateRole:
            return message.value("scheduling_state").toMap();
        case IsOutgoingRole:
            return message.value("is_outgoing").toBool();
        case IsPinnedRole:
            return message.value("is_pinned").toBool();
        case CanBeEditedRole:
            return message.value("can_be_edited").toBool();
        case CanBeForwardedRole:
            return message.value("can_be_forwarded").toBool();
        case CanBeDeletedOnlyForSelfRole:
            return message.value("can_be_deleted_only_for_self").toBool();
        case CanBeDeletedForAllUsersRole:
            return message.value("can_be_deleted_for_all_users").toBool();
        case CanGetStatisticsRole:
            return message.value("can_get_statistics").toBool();
        case CanGetMessageThreadRole:
            return message.value("can_get_message_thread").toBool();
        case IsChannelPostRole:
            return message.value("is_channel_post").toBool();
        case ContainsUnreadMentionRole:
            return message.value("contains_unread_mention").toBool();
        case DateRole:
            return QDateTime::fromMSecsSinceEpoch(message.value("date").toLongLong() * 1000);
        case EditDateRole:
            return QDateTime::fromMSecsSinceEpoch(message.value("edit_date").toLongLong() * 1000);
        case ForwardInfoRole:
            return message.value("forward_info").toMap();
        case InteractionInfoRole:
            return message.value("interaction_info").toMap();
        case ReplyInChatIdRole:
            return message.value("reply_in_chat_id").toString();
        case ReplyToMessageIdRole:
            return message.value("reply_to_message_id").toString();
        case MessageThreadIdRole:
            return message.value("message_thread_id").toString();
        case TtlRole:
            return message.value("ttl").toInt();
        case TtlExpiresInRole:
            return message.value("ttl_expires_in").toDouble();
        case ViaBotUserIdRole:
            return message.value("via_bot_user_id").toInt();
        case AuthorSignatureRole:
            return message.value("author_signature").toString();
        case MediaAlbumIdRole:
            return message.value("media_album_id").toString();
        case RestrictionReasonRole:
            return message.value("restriction_reason").toString();
        case ContentRole:
            return message.value("content").toMap();
        case ReplyMarkupRole:
            return message.value("reply_markup").toMap();

        case BubbleColorRole:
            return QVariant();
        case IsServiceMessageRole:
        {
            return Utils::isServiceMessage(message);
        }
        case SectionRole:
        {
            auto date = QDateTime::fromMSecsSinceEpoch(message.value("date").toLongLong() * 1000);
            return date.toString("dddd, d MMMM yyyy");
        }
        case ServiceMessageRole:
        {
            return Utils::getServiceMessageContent(message);
        }
        default:
            return QVariant();
    }
    return QVariant();
}

QHash<int, QByteArray> MessageModel::roleNames() const noexcept
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[SenderRole] = "sender";
    roles[ChatIdRole] = "chatId";
    roles[SendingStateRole] = "sendingState";
    roles[SchedulingStateRole] = "schedulingState";
    roles[IsOutgoingRole] = "isOutgoing";
    roles[IsPinnedRole] = "isPinned";
    roles[CanBeEditedRole] = "canBeEdited";
    roles[CanBeForwardedRole] = "canBeForwarded";
    roles[CanBeDeletedOnlyForSelfRole] = "canBeDeletedOnlyForSelf";
    roles[CanBeDeletedForAllUsersRole] = "canBeDeletedForAllUsers";
    roles[CanGetStatisticsRole] = "canGetStatistics";
    roles[CanGetMessageThreadRole] = "canGetMessageThread";
    roles[IsChannelPostRole] = "isChannelPost";
    roles[ContainsUnreadMentionRole] = "containsUnreadMention";
    roles[DateRole] = "date";
    roles[EditDateRole] = "editDate";
    roles[ForwardInfoRole] = "forwardInfo";
    roles[InteractionInfoRole] = "interactionInfo";
    roles[ReplyInChatIdRole] = "replyInChatId";
    roles[ReplyToMessageIdRole] = "replyToMessageId";
    roles[MessageThreadIdRole] = "messageThreadId";
    roles[TtlRole] = "ttl";
    roles[TtlExpiresInRole] = "ttlExpires";
    roles[ViaBotUserIdRole] = "viaBotUserId";
    roles[AuthorSignatureRole] = "authorSignature";
    roles[MediaAlbumIdRole] = "mediaAlbumId";
    roles[RestrictionReasonRole] = "restrictionReason";
    roles[ContentRole] = "content";
    roles[ReplyMarkupRole] = "replyMarkup";
    // Custom
    roles[BubbleColorRole] = "bubbleColor";
    roles[IsServiceMessageRole] = "isServiceMessage";
    roles[SectionRole] = "section";
    roles[ServiceMessageRole] = "serviceMessage";
    return roles;
}

int MessageModel::count() const noexcept
{
    return m_messages.count();
}

bool MessageModel::refreshing() const noexcept
{
    return m_refreshing;
}

void MessageModel::loadHistory() noexcept
{
    if (!m_refreshing)
    {
        auto min = std::ranges::min_element(m_uniqueIds);

        if (min != m_uniqueIds.end())
        {
            TdApi::getInstance().getChatHistory(m_chatId, *min, 0, MessageSliceLimit, false);
        }

        m_refreshing = true;
        emit refreshingChanged();
    }
}

void MessageModel::openChat(qint64 chatId) noexcept
{
    clearAll();

    m_chat = TdApi::getInstance().chatStore->get(chatId);
    m_chatId = chatId;

    auto unread = m_chat.value("unread_count").toInt() > 0;
    auto fromMessageId = unread ? m_chat.value("last_read_inbox_message_id").toLongLong() : 0;
    auto offset = unread ? -1 - MessageSliceLimit : 0;
    auto limit = unread ? 2 * MessageSliceLimit : MessageSliceLimit;

    TdApi::getInstance().openChat(chatId);
    TdApi::getInstance().getChatHistory(m_chatId, fromMessageId, offset, limit, false);

    m_refreshing = true;
    emit refreshingChanged();
}

void MessageModel::closeChat(qint64 chatId) noexcept
{
    clearAll();

    m_chatId = 0;
    m_chat = QVariantMap();

    TdApi::getInstance().closeChat(chatId);
}

void MessageModel::deleteMessage(qint64 messageId) noexcept
{
    TdApi::getInstance().deleteMessages(m_chatId, QList<qint64>() << messageId, false);
}

void MessageModel::viewMessage(qint64 messageId) noexcept
{
    TdApi::getInstance().viewMessages(m_chatId, 0, QList<qint64>() << messageId, true);
}

QVariantMap MessageModel::get(qint64 messageId) const noexcept
{
    auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message.value("id").toLongLong() == messageId; });

    if (it != m_messages.end())
    {
        auto index = std::distance(m_messages.begin(), it);
        return m_messages.value(index);
    }

    return {};
}

int MessageModel::indexOf(qint64 messageId) const noexcept
{
    auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message.value("id").toLongLong() == messageId; });

    if (it != m_messages.end())
    {
        auto index = std::distance(m_messages.begin(), it);
        return static_cast<int>(index);
    }

    return -1;
}

void MessageModel::scrollToMessage(qint64 messageId) noexcept
{
    beginResetModel();
    m_uniqueIds.clear();
    m_messages.clear();

    TdApi::getInstance().getChatHistory(m_chatId, messageId, -MessageSliceLimit, 2 * MessageSliceLimit, false);

    endResetModel();
}

void MessageModel::clearAll() noexcept
{
    if (m_messages.isEmpty())
        return;

    beginResetModel();
    m_messages.clear();
    m_uniqueIds.clear();
    endResetModel();

    emit countChanged();
}

void MessageModel::handleNewMessage(const QVariantMap &message)
{
    auto lastMessageId = m_chat.value("last_message").toMap().value("id").toLongLong();

    if (m_chatId != message.value("chat_id").toLongLong())
        return;

    if (m_uniqueIds.contains(lastMessageId))
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_messages.append(message);
        m_uniqueIds.insert(message.value("id").toLongLong());
        endInsertRows();

        viewMessage(message.value("id").toLongLong());

        emit countChanged();
    }
}

void MessageModel::handleMessageSendSucceeded(const QVariantMap &message, qint64 oldMessageId)
{
}

void MessageModel::handleMessageSendFailed(const QVariantMap &message, qint64 oldMessageId, int errorCode, const QString &errorMessage)
{
}

void MessageModel::handleMessageContent(qint64 chatId, qint64 messageId, const QVariantMap &newContent)
{
    if (chatId != m_chatId)
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message.value("id").toLongLong() == messageId; });

    if (it != m_messages.end())
    {
        it->insert("content", newContent);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleMessageEdited(qint64 chatId, qint64 messageId, int editDate, const QVariantMap &replyMarkup)
{
    if (chatId != m_chatId)
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message.value("id").toLongLong() == messageId; });

    if (it != m_messages.end())
    {
        it->insert("edit_date", editDate);
        it->insert("reply_markup", replyMarkup);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleMessageIsPinned(qint64 chatId, qint64 messageId, bool isPinned)
{
    if (chatId != m_chatId)
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message.value("id").toLongLong() == messageId; });

    if (it != m_messages.end())
    {
        it->insert("is_pinned", isPinned);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleMessageInteractionInfo(qint64 chatId, qint64 messageId, const QVariantMap &interactionInfo)
{
    if (chatId != m_chatId)
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message.value("id").toLongLong() == messageId; });

    if (it != m_messages.end())
    {
        it->insert("interaction_info", interactionInfo);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleDeleteMessages(qint64 chatId, const QVariantList &messageIds, bool isPermanent, bool fromCache)
{
    if (chatId == m_chatId)
        return;

    QListIterator<QVariant> it(messageIds);
    while (it.hasNext())
    {
        qlonglong messageId = it.next().toLongLong();
        auto it =
            std::ranges::find_if(m_messages, [messageId](const auto &message) { return message.value("id").toLongLong() == messageId; });

        if (it != m_messages.end())
        {
            auto index = std::distance(m_messages.begin(), it);

            beginRemoveRows(QModelIndex(), index, index);
            m_messages.removeAt(index);
            endRemoveRows();
        }
    }
}

void MessageModel::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    if (chatId == m_chatId)
    {
        m_chat.insert("last_read_inbox_message_id", lastReadInboxMessageId);
        m_chat.insert("unread_count", unreadCount);
    }
}

void MessageModel::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    if (chatId == m_chatId)
        m_chat.insert("last_read_outbox_message_id", lastReadOutboxMessageId);
}

void MessageModel::handleMessage(const QVariantMap &message)
{
}

void MessageModel::handleMessages(const QVariantMap &messages)
{
    const auto list = messages.value("messages").toList();

    QVariantList result;
    std::ranges::copy_if(list, std::back_inserter(result), [this](const auto &message) {
        return !m_uniqueIds.contains(message.toMap().value("id").toLongLong()) && message.toMap().value("chat_id").toLongLong() == m_chatId;
    });

    if (result.isEmpty())
        return;

    std::sort(result.begin(), result.end(),
              [](const auto &a, const auto &b) { return a.toMap().value("id").toLongLong() < b.toMap().value("id").toLongLong(); });

    insertMessages(result);

    m_refreshing = false;

    emit refreshingChanged();
    emit countChanged();
}

void MessageModel::insertMessages(const QVariantList &messages)
{
    auto min = std::ranges::min_element(m_uniqueIds);

    if (min != m_uniqueIds.end() && messages.last().toMap().value("id").toLongLong() < *min)
    {
        beginInsertRows(QModelIndex(), 0, messages.count() - 1);

        int offset = 0;
        for (const auto &message : messages)
        {
            m_messages.insert(offset, message.toMap());
            m_uniqueIds.emplace(message.toMap().value("id").toLongLong());
            ++offset;
        }
        endInsertRows();

        return;
    }

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + messages.count() - 1);

    for (const auto &message : messages)
    {
        m_messages.append(message.toMap());
        m_uniqueIds.emplace(message.toMap().value("id").toLongLong());
    }
    endInsertRows();

    QList<qint64> messageIds;

    std::ranges::for_each(messages, [this, &messageIds](const auto &message) {
        if (m_chat.value("unread_count").toInt() > 0)
        {
            auto id = message.toMap().value("id").toLongLong();
            if (!message.toMap().value("is_outgoing").toBool() && id > m_chat.value("last_read_inbox_message_id").toLongLong())
            {
                messageIds.append(id);
            }
        }
    });

    if (messageIds.size() > 0)
        TdApi::getInstance().viewMessages(m_chatId, 0, messageIds, true);

    messageIds.clear();
}

void MessageModel::itemChanged(int64_t index)
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}
