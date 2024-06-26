#include "MessageModel.hpp"

#include "ChatModel.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "TdManager.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QStringBuilder>
#include <QTimer>

#include <algorithm>
#include <utility>

namespace detail {
QString getBasicGroupStatus(const QVariantMap &basicGroup, const QVariantMap &chat, Locale *locale) noexcept
{
    const auto statusType = basicGroup.value("status").toMap().value("@type").toByteArray();
    const int count = basicGroup.value("member_count").toInt();

    if (statusType == "chatMemberStatusBanned")
    {
        return locale->getString("YouWereKicked");
    }

    const QString memberString = locale->formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    const int onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return memberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getChannelStatus(const QVariantMap &supergroup, const QVariantMap &chat, StorageManager *store, Locale *locale) noexcept
{
    const bool isChannel = supergroup.value("is_channel").toBool();
    if (!isChannel)
    {
        return QString();
    }

    int count = supergroup.value("member_count").toInt();
    const QString username = supergroup.value("username").toString();

    if (count == 0)
    {
        const auto fullInfo = store->getSupergroupFullInfo(supergroup.value("id").toLongLong());
        count = fullInfo.value("member_count").toInt();
    }

    if (count <= 0)
    {
        return !username.isEmpty() ? locale->getString("ChannelPublic") : locale->getString("ChannelPrivate");
    }

    const QString subscriberString = locale->formatPluralString("Subscribers", count);
    if (count <= 1)
    {
        return subscriberString;
    }

    const int onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return subscriberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return subscriberString;
}

QString getSupergroupStatus(const QVariantMap &supergroup, const QVariantMap &chat, StorageManager *store, Locale *locale) noexcept
{
    const auto statusType = supergroup.value("status").toMap().value("@type").toByteArray();
    const auto username = supergroup.value("username").toString();
    const auto hasLocation = supergroup.value("has_location").toBool();

    auto count = supergroup.value("member_count").toInt();

    if (statusType == "chatMemberStatusBanned")
    {
        return locale->getString("YouWereKicked");
    }

    if (count == 0)
    {
        const auto fullInfo = store->getSupergroupFullInfo(supergroup.value("id").toLongLong());
        count = fullInfo.value("member_count").toInt();
    }

    if (count <= 0)
    {
        if (hasLocation)
        {
            return locale->getString("MegaLocation");
        }

        return !username.isEmpty() ? locale->getString("MegaPublic") : locale->getString("MegaPrivate");
    }

    const auto memberString = locale->formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    const auto onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return memberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getUserStatus(const QVariantMap &user, Locale *locale) noexcept
{
    const auto userId = user.value("id").toLongLong();
    if (std::ranges::any_of(ServiceNotificationsUserIds, [userId](auto id) { return id == userId; }))
    {
        return locale->getString("ServiceNotifications");
    }

    if (user.value("is_support").toBool())
    {
        return locale->getString("SupportStatus");
    }

    const auto userType = user.value("type").toMap().value("@type").toByteArray();
    if (userType == "userTypeBot")
    {
        return locale->getString("Bot");
    }

    const auto status = user.value("status").toMap();
    const auto statusType = status.value("@type").toByteArray();

    if (statusType == "userStatusEmpty")
    {
        return locale->getString("ALongTimeAgo");
    }
    else if (statusType == "userStatusLastMonth")
    {
        return locale->getString("WithinAMonth");
    }
    else if (statusType == "userStatusLastWeek")
    {
        return locale->getString("WithinAWeek");
    }
    else if (statusType == "userStatusOffline")
    {
        const auto was_online = status.value("was_online").toLongLong();
        if (was_online == 0)
        {
            return locale->getString("Invisible");
        }

        const auto wasOnline = QDateTime::fromMSecsSinceEpoch(was_online * 1000);
        const auto currentDate = QDate::currentDate();

        if (currentDate == wasOnline.date())  // TODAY
        {
            return locale->getString("LastSeenFormatted")
                .arg(locale->getString("TodayAtFormatted"))
                .arg(wasOnline.toString(locale->getString("formatterDay12H")));
        }
        else if (wasOnline.date().daysTo(currentDate) < 2)  // YESTERDAY
        {
            return locale->getString("LastSeenFormatted")
                .arg(locale->getString("YesterdayAtFormatted"))
                .arg(wasOnline.toString(locale->getString("formatterDay12H")));
        }

        // OTHER DAYS
        return locale->getString("LastSeenDateFormatted")
            .arg(locale->getString("formatDateAtTime")
                     .arg(wasOnline.toString(locale->getString("formatterYear")))
                     .arg(wasOnline.toString(locale->getString("formatterDay12H"))));
    }
    else if (statusType == "userStatusOnline")
    {
        return locale->getString("Online");
    }
    else if (statusType == "userStatusRecently")
    {
        return locale->getString("Lately");
    }

    return QString();
}

}  // namespace detail

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    setRoleNames(roleNames());
}

TdManager *MessageModel::manager() const
{
    return m_manager;
}

void MessageModel::setManager(TdManager *manager)
{
    m_manager = manager;
    m_client = m_manager->storageManager()->client();
    m_locale = m_manager->locale();
    m_storageManager = m_manager->storageManager();

    connect(m_client, SIGNAL(result(const QVariantMap &)), SLOT(handleResult(const QVariantMap &)));

    connect(m_storageManager, SIGNAL(updateChatItem(qint64)), SLOT(handleChatItem(qint64)));
    connect(m_storageManager, SIGNAL(updateChatPosition(qint64)), SLOT(handleChatPosition(qint64)));

    m_chat = m_storageManager->getChat(m_chatId.toLong());
    loadMessages();
    emit statusChanged();
    emit chatIdChanged();
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

    const auto &message = m_messages.at(index.row());
    switch (role)
    {
        case IdRole:
            return message.value("id").toString();
        case SenderRole: {
            if (message.value("is_outgoing").toBool())
                return QString();

            return Utils::getTitle(message, m_storageManager, m_locale);
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

            return date.toString(m_locale->getString("formatterDay12H"));
        }
        case EditDateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(message.value("edit_date").toLongLong() * 1000);

            return date.toString(m_locale->getString("formatterDay12H"));
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
                return m_locale->getString("Today");
            else if (days < 2)
                return m_locale->getString("Yesterday");

            return date.toString(m_locale->getString("chatFullDate"));
        }
        case ServiceMessageRole: {
            return Utils::getServiceMessageContent(message, m_storageManager, m_locale, true);
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

void MessageModel::setChatId(const QString &value) noexcept
{
    if (m_chatId != value)
    {
        m_chatId = value;
        emit chatIdChanged();
    }
}

QString MessageModel::getChatSubtitle() const noexcept
{
    static const std::unordered_map<std::string,
                                    std::function<QString(const QVariantMap &, const QVariantMap &, StorageManager *, Locale *)>>
        chatTypeHandlers = {
            {"chatTypeBasicGroup",
             [](const QVariantMap &type, const QVariantMap &chat, StorageManager *store, Locale *locale) {
                 return detail::getBasicGroupStatus(store->getBasicGroup(type.value("basic_group_id").toLongLong()), chat, locale);
             }},
            {"chatTypePrivate",
             [](const QVariantMap &type, const QVariantMap &, StorageManager *store, Locale *locale) {
                 return detail::getUserStatus(store->getUser(type.value("user_id").toLongLong()), locale);
             }},
            {"chatTypeSecret",
             [](const QVariantMap &type, const QVariantMap &, StorageManager *store, Locale *locale) {
                 return detail::getUserStatus(store->getUser(type.value("user_id").toLongLong()), locale);
             }},
            {"chatTypeSupergroup", [](const QVariantMap &type, const QVariantMap &chat, StorageManager *store, Locale *locale) {
                 const auto supergroup = store->getSupergroup(type.value("supergroup_id").toLongLong());
                 return supergroup.value("is_channel").toBool() ? detail::getChannelStatus(supergroup, chat, store, locale)
                                                                : detail::getSupergroupStatus(supergroup, chat, store, locale);
             }}};

    const auto type = m_chat.value("type").toMap();
    const auto chatType = type.value("@type").toString();

    if (auto it = chatTypeHandlers.find(chatType.toStdString()); it != chatTypeHandlers.end())
    {
        return it->second(type, m_chat, m_storageManager, m_locale);
    }

    return QString();
}

QString MessageModel::getChatTitle() const noexcept
{
    auto isMeChat = [this](const QVariantMap &chat) {
        auto type = chat.value("type").toMap();

        auto chatType = type.value("@type").toByteArray();
        if (chatType == "chatTypeSecret" || chatType == "chatTypePrivate")
        {
            return m_storageManager->getMyId() == type.value("user_id").toLongLong();
        }

        return false;
    };

    if (const auto title = m_chat.value("title").toString(); isMeChat(m_chat))
        return title.isEmpty() ? m_locale->getString("HiddenName") : title;

    return m_locale->getString("SavedMessages");
}

QString MessageModel::getChatPhoto() const noexcept
{
    if (auto chatPhoto = m_chat.value("photo").toMap();
        chatPhoto.value("small").toMap().value("local").toMap().value("is_downloading_completed").toBool())
    {
        return "image://chatPhoto/" + chatPhoto.value("small").toMap().value("local").toMap().value("path").toString();
    }

    return "image://theme/icon-l-content-avatar-placeholder";
}

QString MessageModel::getFormattedText(const QVariantMap &formattedText, const QVariantMap &options) noexcept
{
    return Utils::getFormattedText(formattedText, m_storageManager, m_locale, options);
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

    m_manager->sendRequest(request);
}

void MessageModel::closeChat() noexcept
{
    QVariantMap request;
    request.insert("@type", "closeChat");
    request.insert("chat_id", m_chatId);

    m_manager->sendRequest(request);
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

    m_manager->sendRequest(request, [this](const auto &messages) { handleMessages(messages); });
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

    m_manager->sendRequest(request);
}

void MessageModel::viewMessages(const QVariantList &messageIds)
{
    QVariantMap request;
    request.insert("@type", "viewMessages");
    request.insert("chat_id", m_chatId);
    request.insert("message_thread_id", 0);
    request.insert("message_ids", messageIds);
    request.insert("force_read", true);

    m_manager->sendRequest(request);
}

void MessageModel::deleteMessage(qint64 messageId, bool revoke) noexcept
{
    QVariantMap request;
    request.insert("@type", "deleteMessages");
    request.insert("chat_id", m_chatId);
    request.insert("message_ids", QVariantList() << messageId);
    request.insert("revoke", revoke);

    m_manager->sendRequest(request);
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

void MessageModel::handleResult(const QVariantMap &object)
{
    static const std::unordered_map<std::string, std::function<void(const QVariantMap &)>> handlers = {
        {"updateNewMessage", [this](const QVariantMap &obj) { handleNewMessage(obj.value("message").toMap()); }},
        {"updateMessageSendSucceeded",
         [this](const QVariantMap &obj) {
             handleMessageSendSucceeded(obj.value("message").toMap(), obj.value("old_message_id").toLongLong());
         }},
        {"updateMessageSendFailed",
         [this](const QVariantMap &obj) {
             handleMessageSendFailed(obj.value("message").toMap(), obj.value("old_message_id").toLongLong(),
                                     obj.value("error_code").toInt(), obj.value("error_message").toString());
         }},
        {"updateMessageContent",
         [this](const QVariantMap &obj) {
             handleMessageContent(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(),
                                  obj.value("new_content").toMap());
         }},
        {"updateMessageEdited",
         [this](const QVariantMap &obj) {
             handleMessageEdited(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(), obj.value("edit_date").toInt(),
                                 obj.value("reply_markup").toMap());
         }},
        {"updateMessageIsPinned",
         [this](const QVariantMap &obj) {
             handleMessageIsPinned(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(),
                                   obj.value("is_pinned").toBool());
         }},
        {"updateMessageInteractionInfo",
         [this](const QVariantMap &obj) {
             handleMessageInteractionInfo(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(),
                                          obj.value("interaction_info").toMap());
         }},
    };

    const auto objectType = object.value("@type").toString().toStdString();

    if (const auto it = handlers.find(objectType); it != handlers.end())
    {
        it->second(object);
    }
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
        return !m_messageIds.contains(message.toMap().value("id").toLongLong()) && message.toMap().value("chat_id").toString() == m_chatId;
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
    if (m_loadingHistory)
    {
        m_loadingHistory = false;

        int offset = 0;

        beginInsertRows(QModelIndex(), 0, messages.count() - 1);

        std::ranges::for_each(messages, [this, &offset](const auto &message) {
            m_messages.insert(offset, message.toMap());
            m_messageIds.emplace(message.toMap().value("id").toLongLong());
            ++offset;
        });

        endInsertRows();

        if (offset > 0)
            emit moreHistoriesLoaded(offset);
    }

    if (m_loading)
    {
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

        if (messageIds.size() > 0)
            viewMessages(messageIds);
    }

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
