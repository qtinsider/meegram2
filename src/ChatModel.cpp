#include "ChatModel.hpp"

#include "Common.hpp"
#include "Utils.hpp"

#include <fnv-cpp/fnv.h>

#include <algorithm>

namespace {
QVariantMap getChatPosition(const QVariantMap &chat, const QVariantMap &chatList) noexcept
{
    auto positions = chat.value("positions").toList();

    auto chatListType = chatList.value("@type").toByteArray();
    switch (fnv::hashRuntime(chatListType.constData()))
    {
        case fnv::hash("chatListMain"): {
            if (auto it = std::ranges::find_if(positions,
                                               [](const auto &x) {
                                                   auto list = x.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListMain";
                                               });
                it != positions.end())
                return it->toMap();
        }
        case fnv::hash("chatListArchive"): {
            if (auto it = std::ranges::find_if(positions,
                                               [](const auto &x) {
                                                   auto list = x.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListArchive";
                                               });
                it != positions.end())
                return it->toMap();
        }
        case fnv::hash("chatListFilter"): {
            if (auto it = std::ranges::find_if(positions,
                                               [chatList](const auto &x) {
                                                   auto list = x.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListFilter" &&
                                                          list.value("chat_filter_id").toInt() == chatList.value("chat_filter_id").toInt();
                                               });
                it != positions.end())
                return it->toMap();
        }
    }

    return {};
}

}  // namespace

class ChatSorter
{
public:
    ChatSorter(QVariantMap chatList)
        : m_chatList(std::move(chatList))
    {
    }

    bool operator()(const QVariantMap &first, const QVariantMap &second) const noexcept
    {
        auto order1 = getChatPosition(first, m_chatList).value("order").toLongLong();
        auto order2 = getChatPosition(second, m_chatList).value("order").toLongLong();

        return std::cmp_greater(order1, order2);
    }

private:
    QVariantMap m_chatList;
};

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_sortTimer(new QTimer(this))
{
    connect(&TdApi::getInstance(), SIGNAL(updateChatTitle(qint64, const QString &)), SLOT(handleChatTitle(qint64, const QString &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatLastMessage(qint64, const QVariantMap &, const QVariantList &)),
            SLOT(handleChatLastMessage(qint64, const QVariantMap &, const QVariantList &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatIsMarkedAsUnread(qint64, bool)), SLOT(handleChatIsMarkedAsUnread(qint64, bool)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatReadInbox(qint64, qint64, int)), SLOT(handleChatReadInbox(qint64, qint64, int)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatReadOutbox(qint64, qint64)), SLOT(handleChatReadOutbox(qint64, qint64)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatNotificationSettings(qint64, const QVariantMap &)),
            SLOT(handleChatNotificationSettings(qint64, const QVariantMap &)));

    connect(&TdApi::getInstance(), SIGNAL(chat(const QVariantMap &)), SLOT(handleChat(const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(chats(const QVariantMap &)), SLOT(handleChats(const QVariantMap &)));

    connect(&TdApi::getInstance(), SIGNAL(updateFile(const QVariantMap &)), SLOT(handleChatPhoto(const QVariantMap &)));

    connect(this, SIGNAL(statusChanged()), SLOT(refresh()));

    connect(m_sortTimer, SIGNAL(timeout()), SLOT(sort()));
    m_sortTimer->setInterval(1000);
    m_sortTimer->setSingleShot(true);

    setRoleNames(roleNames());
}

ChatModel::~ChatModel()
{
    if (m_sortTimer)
        delete m_sortTimer;
}

int ChatModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_chats.size();
}

bool ChatModel::canFetchMore(const QModelIndex &) const
{
    return m_canFetchMore;
}

void ChatModel::fetchMore(const QModelIndex &parent)
{
    if (!canFetchMore(parent))
        return;

    m_canFetchMore = false;

    loadChats();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (const auto &chat = m_chats.at(index.row()); role)
    {
        case IdRole:
            return chat.value("id").toString();
        case TypeRole:
            return chat.value("type").toMap();
        case TitleRole:
            return Utils::getChatTitle(chat);
        case PhotoRole: {
            // TODO(strawberry):
            auto chatPhoto = chat.value("photo").toMap();
            if (!chat.value("photo").isNull() &&
                chatPhoto.value("small").toMap().value("local").toMap().value("is_downloading_completed").toBool())
            {
                return chatPhoto.value("small").toMap().value("local").toMap().value("path").toString();
            }
            return QString();
        }
        case PermissionsRole:
            return chat.value("permissions").toMap();
        case LastMessageRole:
            return chat.value("last_message").toMap();
        case PositionsRole:
            return chat.value("positions").toMap();
        case IsMarkedAsUnreadRole:
            return chat.value("is_marked_as_unread").toBool();
        case IsBlockedRole:
            return chat.value("is_blocked").toBool();
        case HasScheduledMessagesRole:
            return chat.value("has_scheduled_messages").toMap();
        case CanBeDeletedOnlyForSelfRole:
            return chat.value("can_be_deleted_only_for_self").toBool();
        case CanBeDeletedForAllUsersRole:
            return chat.value("can_be_deleted_for_all_users").toBool();
        case CanBeReportedRole:
            return chat.value("can_be_reported").toMap();
        case DefaultDisableNotificationRole:
            return chat.value("default_disable_notification").toMap();
        case UnreadCountRole:
            return chat.value("unread_count").toInt();
        case LastReadInboxMessageIdRole:
            return chat.value("last_read_inbox_message_id").toString();
        case LastReadOutboxMessageIdRole:
            return chat.value("last_read_outbox_message_id").toString();
        case UnreadMentionCountRole:
            return chat.value("unread_mention_count").toMap();
        case NotificationSettingsRole:
            return chat.value("notification_settings").toMap();
        case MessageTtlSettingRole:
            return chat.value("message_ttl_setting").toMap();
        case ActionBarRole:
            return chat.value("action_bar").toMap();
        case VoiceChatRole:
            return chat.value("voice_chat").toMap();
        case ReplyMarkupMessageIdRole:
            return chat.value("reply_markup_message_id").toMap();
        case DraftMessageRole:
            return chat.value("draft_message").toMap();
        case ClientDataRole:
            return chat.value("client_data").toMap();

        case ChatListRole: {
            const auto position = getChatPosition(chat, getChatList(m_chatList));
            return position.value("list").toMap();
        }
        case IsMuteRole: {
            auto notificationSettings = chat.value("notification_settings").toMap();
            return notificationSettings.value("mute_for").toInt() > 0;
        }
        case IsPinnedRole: {
            const auto position = getChatPosition(chat, getChatList(m_chatList));
            return position.value("is_pinned").toBool();
        }
        case LastMessageAuthorRole: {
            // TODO(strawberry):
            return Utils::getMessageSenderName(chat, chat.value("last_message").toMap());
        }
        case LastMessageContentRole:
            return Utils::getContent(chat);
        case LastMessageDateRole:
            return Utils::getMessageDate(chat.value("last_message").toMap());
    }

    return QVariant();
}

QHash<int, QByteArray> ChatModel::roleNames() const noexcept
{
    QHash<int, QByteArray> roles;

    roles[IdRole] = "id";
    roles[TypeRole] = "type";
    roles[TitleRole] = "title";
    roles[PhotoRole] = "photo";
    roles[PermissionsRole] = "permissions";
    roles[LastMessageRole] = "lastMessage";
    roles[PositionsRole] = "positions";
    roles[IsMarkedAsUnreadRole] = "isMarkedAsUnread";
    roles[IsBlockedRole] = "isBlocked";
    roles[HasScheduledMessagesRole] = "hasScheduledMessages";
    roles[CanBeDeletedOnlyForSelfRole] = "canBeDeletedOnlyForSelf";
    roles[CanBeDeletedForAllUsersRole] = "canBeDeletedForAllUsers";
    roles[CanBeReportedRole] = "canBeReported";
    roles[DefaultDisableNotificationRole] = "defaultDisableNotification";
    roles[UnreadCountRole] = "unreadCount";
    roles[LastReadInboxMessageIdRole] = "lastReadInboxMessageId";
    roles[LastReadOutboxMessageIdRole] = "lastReadOutboxMessageId";
    roles[UnreadMentionCountRole] = "unreadMentionCount";
    roles[NotificationSettingsRole] = "notificationSettings";
    roles[MessageTtlSettingRole] = "messageTtlSetting";
    roles[ActionBarRole] = "actionBar";
    roles[VoiceChatRole] = "voiceChat";
    roles[ReplyMarkupMessageIdRole] = "replyMarkupMessageId";
    roles[DraftMessageRole] = "draftMessage";
    roles[ClientDataRole] = "clientData";

    roles[ChatListRole] = "chatList";
    roles[IsMuteRole] = "isMute";
    roles[IsPinnedRole] = "isPinned";
    roles[LastMessageAuthorRole] = "lastMessageAuthor";
    roles[LastMessageContentRole] = "lastMessageContent";
    roles[LastMessageDateRole] = "lastMessageDate";
    return roles;
}

int ChatModel::count() const noexcept
{
    return m_chats.count();
}

QVariantMap ChatModel::get(qint64 chatId) const noexcept
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &message) { return message.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        auto index = std::distance(m_chats.begin(), it);
        return m_chats.value(index);
    }

    return {};
}

void ChatModel::loadChats() noexcept
{
    const auto chatList = getChatList(m_chatList);

    if (not m_chats.isEmpty())
    {
        auto &&[offsetChatId, offsetOrder] = getChatOrder(m_lastChatId, chatList);

        TdApi::getInstance().getChats(chatList, offsetOrder, offsetChatId, ChatSliceLimit);
    }
    else
        TdApi::getInstance().getChats(chatList, std::numeric_limits<std::int64_t>::max(), 0, ChatSliceLimit);
}

TdApi::ChatList ChatModel::chatList() const noexcept
{
    return m_chatList;
}

void ChatModel::setChatList(TdApi::ChatList chatList) noexcept
{
    if (m_chatList == chatList)
        return;

    m_chatList = chatList;
    emit statusChanged();
}

int ChatModel::chatFilterId() const noexcept
{
    return m_chatFilterId;
}

void ChatModel::setChatFilterId(int chatFilterId) noexcept
{
    m_chatFilterId = chatFilterId;
    m_chatList = TdApi::ChatListFilter;

    emit statusChanged();
}

QPair<qint64, qint64> ChatModel::getChatOrder(qint64 chatId, const QVariantMap &chatList) const noexcept
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        auto position = getChatPosition(*it, chatList);
        return qMakePair(chatId, position.value("order").toLongLong());
    }

    return {};
}

QVariantMap ChatModel::getChatList(TdApi::ChatList chatList) const noexcept
{
    QVariantMap result;

    switch (chatList)
    {
        case TdApi::ChatListMain:
            result.insert("@type", "chatListMain");
            break;
        case TdApi::ChatListArchive:
            result.insert("@type", "chatListArchive");
            break;
        case TdApi::ChatListFilter:
            result.insert("@type", "chatListFilter");
            result.insert("chat_filter_id", m_chatFilterId);
            break;
    }

    return result;
}

void ChatModel::clear() noexcept
{
    if (m_chats.isEmpty())
        return;

    beginResetModel();
    m_chats.clear();
    endResetModel();

    emit countChanged();
}

void ChatModel::sort(int, Qt::SortOrder)
{
    emit layoutAboutToBeChanged();

    auto chatList = getChatList(m_chatList);

    ChatSorter sorter(chatList);
    std::sort(m_chats.begin(), m_chats.end(), sorter);

    emit layoutChanged();
}

void ChatModel::handleChatTitle(qint64 chatId, const QString &title)
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        it->insert("title", title);

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::handleChatPhoto(qint64 chatId, const QVariantMap &photo)
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        it->insert("photo", photo);

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::handleChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions)
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        it->insert("last_message", lastMessage);

        if (!positions.isEmpty())
        {
            // emit delayed event
            if (not m_sortTimer->isActive())
                m_sortTimer->start();

            it->insert("positions", positions);
        }

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::handleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread)
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        it->insert("is_marked_as_unread", isMarkedAsUnread);

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        it->insert("last_read_inbox_message_id", lastReadInboxMessageId);
        it->insert("unread_count", unreadCount);

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        it->insert("last_read_outbox_message_id", lastReadOutboxMessageId);

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::handleChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings)
{
    auto it = std::ranges::find_if(m_chats, [chatId](const auto &chat) { return chat.value("id").toLongLong() == chatId; });

    if (it != m_chats.end())
    {
        it->insert("notification_settings", notificationSettings);

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::handleChat(const QVariantMap &chat)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());

    auto it =
        std::ranges::remove_if(m_chats, [chat](const auto &c) { return chat.value("id").toLongLong() == c.value("id").toLongLong(); });

    m_chats.erase(it.begin(), it.end());
    m_chats.push_back(chat);

    m_canFetchMore = true;

    endInsertRows();

    auto chatPhoto = chat.value("photo").toMap();
    if (!chat.value("photo").isNull() &&
        !chatPhoto.value("small").toMap().value("local").toMap().value("is_downloading_completed").toBool())
    {
        TdApi::getInstance().downloadFile(chatPhoto.value("small").toMap().value("id").toInt(), 1, 0, 0, false);
    }

    emit countChanged();
}

void ChatModel::handleChats(const QVariantMap &chats)
{
    const auto list = chats.value("chat_ids").toList();

    if (list.isEmpty())
        return;

    m_lastChatId = list.last().toLongLong();

    std::ranges::for_each(list, [](const auto &chatId) { TdApi::getInstance().getChat(chatId.toLongLong()); });
}

void ChatModel::handleChatPhoto(const QVariantMap &file)
{
    auto it = std::ranges::find_if(m_chats, [file](const auto &chat) {
        return chat.value("photo").toMap().value("small").toMap().value("id").toInt() == file.value("id").toInt();
    });

    if (it != m_chats.end())
    {
        QVariantMap chatPhoto;
        chatPhoto.insert("small", file);

        it->insert("photo", chatPhoto);

        auto index = std::distance(m_chats.begin(), it);
        itemChanged(index);
    }
}

void ChatModel::refresh() noexcept
{
    m_canFetchMore = false;

    clear();
    loadChats();
}

void ChatModel::itemChanged(int64_t index)
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}
