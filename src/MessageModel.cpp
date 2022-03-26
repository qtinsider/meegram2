#include "MessageModel.hpp"

#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "TdApi.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QLocale>
#include <QStringBuilder>
#include <QTimer>

#include <algorithm>

namespace detail {
QString getBasicGroupStatus(const QVariantMap &basicGroup, const QVariantMap &chat) noexcept
{
    auto status = basicGroup.value("status").toMap();
    auto count = basicGroup.value("member_count").toInt();

    if (status.value("@type").toByteArray() == "chatMemberStatusBanned")
        return Localization::getInstance().getString("YouWereKicked");

    if (count <= 1)
        return Localization::getInstance().formatPluralString("Members", count);

    auto onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return Localization::getInstance().formatPluralString("Members", count) % ", " %
               Localization::getInstance().formatPluralString("OnlineCount", onlineCount);
    }

    return Localization::getInstance().formatPluralString("Members", count);
}

QString getChannelStatus(const QVariantMap &supergroup, const QVariantMap &chat) noexcept
{
    auto status = supergroup.value("status").toMap();
    auto count = supergroup.value("member_count").toInt();
    auto username = supergroup.value("username").toString();

    if (!supergroup.value("is_channel").toBool())
        return QString();

    if (count == 0)
    {
        auto fullInfo = StorageManager::getInstance().getSupergroupFullInfo(supergroup.value("id").toLongLong());
        count = fullInfo.value("member_count").toInt();
    }

    if (count <= 0)
    {
        return !username.isEmpty() ? Localization::getInstance().getString("ChannelPublic")
                                   : Localization::getInstance().getString("ChannelPrivate");
    }

    if (count <= 1)
        return Localization::getInstance().formatPluralString("Subscribers", 1);

    auto onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return Localization::getInstance().formatPluralString("Subscribers", count) % ", " %
               Localization::getInstance().formatPluralString("OnlineCount", onlineCount);
    }

    return Localization::getInstance().formatPluralString("Subscribers", count);
}

QString getSupergroupStatus(const QVariantMap &supergroup, const QVariantMap &chat) noexcept
{
    auto status = supergroup.value("status").toMap();
    auto count = supergroup.value("member_count").toInt();
    auto username = supergroup.value("username").toString();
    auto hasLocation = supergroup.value("has_location").toBool();

    if (status.value("@type").toByteArray() == "chatMemberStatusBanned")
        return Localization::getInstance().getString("YouWereKicked");

    if (count == 0)
    {
        auto fullInfo = StorageManager::getInstance().getSupergroupFullInfo(supergroup.value("id").toLongLong());
        count = fullInfo.value("member_count").toInt();
    }

    if (count <= 0)
    {
        if (hasLocation)
            return Localization::getInstance().getString("MegaLocation");

        return !username.isEmpty() ? Localization::getInstance().getString("MegaPublic")
                                   : Localization::getInstance().getString("MegaPrivate");
    }

    if (count <= 1)
        return Localization::getInstance().formatPluralString("Members", count);

    auto onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return Localization::getInstance().formatPluralString("Members", count) % ", " %
               Localization::getInstance().formatPluralString("OnlineCount", onlineCount);
    }

    return Localization::getInstance().formatPluralString("Members", count);
}

QString getUserStatus(const QVariantMap &user) noexcept
{
    if (std::ranges::any_of(ServiceNotificationsUserIds, [user](qint64 userId) { return userId == user.value("id").toLongLong(); }))
    {
        return Localization::getInstance().getString("ServiceNotifications");
    }

    if (user.value("is_support").toBool())
        return Localization::getInstance().getString("SupportStatus");

    auto type = user.value("type").toMap();

    if (type.value("@type").toByteArray() == "userTypeBot")
        return Localization::getInstance().getString("Bot");

    auto status = user.value("status").toMap();

    auto statusType = status.value("@type").toByteArray();
    switch (fnv::hashRuntime(statusType.constData()))
    {
        case fnv::hash("userStatusEmpty"):
            return Localization::getInstance().getString("ALongTimeAgo");
        case fnv::hash("userStatusLastMonth"):
            return Localization::getInstance().getString("WithinAMonth");
        case fnv::hash("userStatusLastWeek"):
            return Localization::getInstance().getString("WithinAWeek");
        case fnv::hash("userStatusOffline"): {
            auto was_online = status.value("was_online").toLongLong();
            if (was_online == 0)
                return Localization::getInstance().getString("Invisible");

            auto wasOnline = QDateTime::fromMSecsSinceEpoch(was_online * 1000);

            if (QDate::currentDate() == wasOnline.date())  // TODAY
                return Localization::getInstance()
                    .getString("LastSeenFormatted")
                    .arg(Localization::getInstance().getString("TodayAtFormatted"))
                    .arg(wasOnline.toString(Localization::getInstance().getString("formatterDay12H")));

            else if (wasOnline.date().daysTo(QDate::currentDate()) < 2)
                return Localization::getInstance()
                    .getString("LastSeenFormatted")
                    .arg(Localization::getInstance().getString("YesterdayAtFormatted"))
                    .arg(wasOnline.toString(Localization::getInstance().getString("formatterDay12H")));

            return Localization::getInstance()
                .getString("LastSeenDateFormatted")
                .arg(Localization::getInstance()
                         .getString("formatDateAtTime")
                         .arg(wasOnline.toString(Localization::getInstance().getString("formatterYear")))
                         .arg(wasOnline.toString(Localization::getInstance().getString("formatterDay12H"))));
        }
        case fnv::hash("userStatusOnline"):
            return Localization::getInstance().getString("Online");
        case fnv::hash("userStatusRecently"):
            return Localization::getInstance().getString("Lately");
    }

    return QString();
}

}  // namespace detail

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

    connect(&TdApi::getInstance(), SIGNAL(updateChatOnlineMemberCount(qint64, int)), SLOT(handleChatOnlineMemberCount(qint64, int)));

    connect(&TdApi::getInstance(), SIGNAL(updateChatReadInbox(qint64, qint64, int)), SLOT(handleChatReadInbox(qint64, qint64, int)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatReadOutbox(qint64, qint64)), SLOT(handleChatReadOutbox(qint64, qint64)));

    QTimer::singleShot(1000, this, SLOT(loadMessages()));

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
    if (parent.isValid())
        return false;

    auto lastMessageId = m_chat.value("last_message").toMap().value("id").toLongLong();

    if (m_messages.size() > 0)
        return !m_loading && lastMessageId != *std::ranges::max_element(m_messageIds);

    return false;
}

void MessageModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    if (!m_loading)
    {
        if (auto max = std::ranges::max_element(m_messageIds); max != m_messageIds.end())
        {
            getChatHistory(*max, -MessageSliceLimit, MessageSliceLimit);
        }

        m_loading = true;

        emit loadingChanged();
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
        case SenderRole: {
            if (message.value("is_outgoing").toBool())
                return QString();

            return Utils::getTitle(message);
        }
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
        case DateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(message.value("date").toLongLong() * 1000);

            return QLocale::system().toString(date.time(), QLocale::ShortFormat);
        }
        case EditDateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(message.value("edit_date").toLongLong() * 1000);

            return QLocale::system().toString(date.time(), QLocale::ShortFormat);
        }
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
        case IsServiceMessageRole: {
            return Utils::isServiceMessage(message);
        }
        case SectionRole: {
            const auto date = QDateTime::fromMSecsSinceEpoch(message.value("date").toLongLong() * 1000);

            const auto days = date.daysTo(QDateTime::currentDateTime());

            if (days == 0)
                return tr("Today");
            else if (days < 2)
                return tr("Yesterday");

            return QLocale::system().toString(date.date(), "dddd, d MMMM yyyy");
        }
        case ServiceMessageRole: {
            return Utils::getServiceMessageContent(message, true);
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

bool MessageModel::loading() const noexcept
{
    return m_loading;
}

bool MessageModel::loadingHistory() const noexcept
{
    return m_loadingHistory;
}

QString MessageModel::getChatId() const noexcept
{
    return m_chatId;
}

void MessageModel::setChatId(const QString value) noexcept
{
    if (m_chatId == value)
        return;

    m_chatId = value;
    emit chatIdChanged();
}

QString MessageModel::getChatSubtitle() const noexcept
{
    auto type = m_chat.value("type").toMap();

    auto chatType = type.value("@type").toByteArray();

    switch (fnv::hashRuntime(chatType.constData()))
    {
        case fnv::hash("chatTypeBasicGroup"): {
            auto basicGroup = StorageManager::getInstance().getBasicGroup(type.value("basic_group_id").toLongLong());
            return detail::getBasicGroupStatus(basicGroup, m_chat);
        }
        case fnv::hash("chatTypePrivate"):
        case fnv::hash("chatTypeSecret"): {
            auto user = StorageManager::getInstance().getUser(type.value("user_id").toLongLong());
            return detail::getUserStatus(user);
        }
        case fnv::hash("chatTypeSupergroup"): {
            auto supergroup = StorageManager::getInstance().getSupergroup(type.value("supergroup_id").toLongLong());
            return supergroup.value("is_channel").toBool() ? detail::getChannelStatus(supergroup, m_chat)
                                                           : detail::getSupergroupStatus(supergroup, m_chat);
        }
    }

    return QString();
}

QString MessageModel::getChatTitle() const noexcept
{
    return Utils::getChatTitle(m_chatId.toLongLong());
}

void MessageModel::loadHistory() noexcept
{
    if (auto min = std::ranges::min_element(m_messageIds); min != m_messageIds.end() && !m_loadingHistory)
    {
        m_loadingHistory = true;

        getChatHistory(*min, 0, MessageSliceLimit);

        emit loadingChanged();
    }
}

void MessageModel::openChat() noexcept
{
    QVariantMap request;
    request.insert("@type", "openChat");
    request.insert("chat_id", m_chatId);

    TdApi::getInstance().sendRequest(request);
}

void MessageModel::closeChat() noexcept
{
    QVariantMap request;
    request.insert("@type", "closeChat");
    request.insert("chat_id", m_chatId);

    TdApi::getInstance().sendRequest(request);
}

void MessageModel::getChatHistory(qint64 fromMessageId, qint32 offset, qint32 limit)
{
    QVariantMap request;
    request.insert("@type", "getChatHistory");
    request.insert("chat_id", m_chatId);
    request.insert("from_message_id", fromMessageId);
    request.insert("offset", offset);
    request.insert("limit", limit);
    request.insert("only_local", false);

    TdApi::getInstance().sendRequest(request);
}

void MessageModel::sendMessage(const QString &message, qint64 replyToMessageId)
{
    QVariantMap formattedText, inputMessageContent;
    formattedText.insert("@type", "formattedText");
    formattedText.insert("text", message);

    inputMessageContent.insert("@type", "inputMessageText");
    inputMessageContent.insert("text", formattedText);

    QVariantMap request;
    request.insert("@type", "sendMessage");
    request.insert("chat_id", m_chatId);

    if (replyToMessageId != 0)
    {
        request.insert("reply_to_message_id", replyToMessageId);
    }

    request.insert("input_message_content", inputMessageContent);

    TdApi::getInstance().sendRequest(request);
}

void MessageModel::viewMessages(const QVariantList &messageIds)
{
    QVariantMap request;
    request.insert("@type", "viewMessages");
    request.insert("chat_id", m_chatId);
    request.insert("message_thread_id", 0);
    request.insert("message_ids", messageIds);
    request.insert("force_read", true);

    TdApi::getInstance().sendRequest(request);
}

void MessageModel::deleteMessage(qint64 messageId, bool revoke) noexcept
{
    QVariantMap request;
    request.insert("@type", "deleteMessages");
    request.insert("chat_id", m_chatId);
    request.insert("message_ids", QVariantList() << messageId);
    request.insert("revoke", revoke);

    TdApi::getInstance().sendRequest(request);
}

void MessageModel::refresh() noexcept
{
    if (m_messages.isEmpty())
        return;

    m_loading = true;
    m_loadingHistory = true;

    beginResetModel();
    m_messages.clear();
    m_messageIds.clear();
    endResetModel();

    emit countChanged();
}

void MessageModel::handleNewMessage(const QVariantMap &message)
{
    if (m_chatId != message.value("chat_id").toString())
        return;

    if (auto lastMessageId = m_chat.value("last_message").toMap().value("id").toLongLong(); m_messageIds.contains(lastMessageId))
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_messages.append(message);
        m_messageIds.insert(message.value("id").toLongLong());
        endInsertRows();

        viewMessages(QVariantList() << message.value("id").toLongLong());

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
    if (chatId != m_chatId.toLongLong())
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
    if (chatId != m_chatId.toLongLong())
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
    if (chatId != m_chatId.toLongLong())
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
    if (chatId != m_chatId.toLongLong())
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
    if (chatId != m_chatId.toLongLong())
        return;

    QListIterator<QVariant> it(messageIds);
    while (it.hasNext())
    {
        auto result = std::ranges::find_if(m_messages,
                                           [&](const auto &message) { return message.value("id").toLongLong() == it.next().toLongLong(); });

        if (result != m_messages.end())
        {
            auto index = std::distance(m_messages.begin(), result);

            beginRemoveRows(QModelIndex(), index, index);
            m_messages.removeAt(index);
            endRemoveRows();
        }
    }
}

void MessageModel::handleChatOnlineMemberCount(qint64 chatId, int onlineMemberCount)
{
    if (chatId == m_chatId.toLongLong())
    {
        m_chat.insert("online_member_count", onlineMemberCount);

        emit chatIdChanged();
        emit statusChanged();
    }
}

void MessageModel::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    if (chatId == m_chatId.toLongLong())
    {
        m_chat.insert("last_read_inbox_message_id", lastReadInboxMessageId);
        m_chat.insert("unread_count", unreadCount);

        emit chatIdChanged();
    }
}

void MessageModel::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    if (chatId != m_chatId.toLongLong())
        return;

    m_chat.insert("last_read_outbox_message_id", lastReadOutboxMessageId);
    emit chatIdChanged();
}

void MessageModel::handleMessages(const QVariantMap &messages)
{
    const auto list = messages.value("messages").toList();

    QVariantList result;
    std::ranges::copy_if(list, std::back_inserter(result), [this](const auto &message) {
        return !m_messageIds.contains(message.toMap().value("id").toLongLong()) &&
               message.toMap().value("chat_id").toString() == m_chatId;
    });

    if (result.isEmpty())
    {
        if (m_loadingHistory)
        {
            m_loadingHistory = false;
        }
        else
        {
            m_loading = false;
        }

        emit loadingChanged();
    }
    else
    {
        std::sort(result.begin(), result.end(), [](const auto &a, const auto &b) {
            return std::cmp_less(a.toMap().value("id").toLongLong(), b.toMap().value("id").toLongLong());
        });

        insertMessages(result);
    }

    emit countChanged();
}

void MessageModel::insertMessages(const QVariantList &messages) noexcept
{
    if (auto min = std::ranges::min_element(m_messageIds);
        min != m_messageIds.end() && messages.last().toMap().value("id").toLongLong() < *min)
    {
        m_loadingHistory = false;

        beginInsertRows(QModelIndex(), 0, messages.count() - 1);

        int offset = 0;

        std::ranges::for_each(messages, [this, &offset](const auto &message) {
            m_messages.insert(offset, message.toMap());
            m_messageIds.emplace(message.toMap().value("id").toLongLong());
            ++offset;
        });

        endInsertRows();

        if (offset > 0)
            emit moreHistoriesLoaded(offset);

        emit loadingChanged();

        return;
    }

    m_loading = false;

    beginInsertRows(QModelIndex(), rowCount(), rowCount() + messages.count() - 1);

    std::ranges::for_each(messages, [this](const auto &message) {
        m_messages.append(message.toMap());
        m_messageIds.emplace(message.toMap().value("id").toLongLong());
    });

    endInsertRows();

    QVariantList messageIds;

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

    m_loadingHistory = false;

    if (messageIds.size() > 0)
        viewMessages(messageIds);

    emit loadingChanged();
}

void MessageModel::loadMessages() noexcept
{
    auto unread = m_chat.value("unread_count").toInt() > 0;
    auto fromMessageId =
        unread ? m_chat.value("last_read_inbox_message_id").toLongLong() : m_chat.value("last_message").toMap().value("id").toLongLong();

    auto offset = unread ? -1 - MessageSliceLimit : 0;
    auto limit = unread ? 2 * MessageSliceLimit : MessageSliceLimit;

    getChatHistory(fromMessageId, offset, limit);
}

void MessageModel::itemChanged(int64_t index)
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}
