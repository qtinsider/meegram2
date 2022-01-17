#include "Stores.hpp"

#include "TdApi.hpp"
#include "Utils.hpp"

#include <QMutexLocker>

#include <algorithm>

void BasicGroupStore::initialize(TdApi *client)
{
    connect(client, SIGNAL(updateBasicGroup(const QVariantMap &)), SLOT(handleBasicGroup(const QVariantMap &)));
    connect(client, SIGNAL(updateBasicGroupFullInfo(qint64, const QVariantMap &)),
            SLOT(handleBasicGroupFullInfo(qint64, const QVariantMap &)));
}

QVariantMap BasicGroupStore::get(qint64 groupId) const
{
    if (auto it = m_basicGroup.find(groupId); it != m_basicGroup.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap BasicGroupStore::getFullInfo(qint64 groupId) const
{
    if (auto it = m_fullInfo.find(groupId); it != m_fullInfo.end())
    {
        return it->second;
    }

    return {};
}

void BasicGroupStore::handleBasicGroup(const QVariantMap &basicGroup)
{
    QMutexLocker lock(&m_mutex);

    m_basicGroup.emplace(basicGroup.value("id").toLongLong(), basicGroup);
}

void BasicGroupStore::handleBasicGroupFullInfo(qint64 basicGroupId, const QVariantMap &basicGroupFullInfo)
{
    QMutexLocker lock(&m_mutex);

    m_fullInfo.emplace(basicGroupId, basicGroupFullInfo);
}

void ChatStore::initialize(TdApi *client)
{
    connect(client, SIGNAL(updateNewChat(const QVariantMap &)), SLOT(handleNewChat(const QVariantMap &)));
    connect(client, SIGNAL(updateChatTitle(qint64, const QString &)), SLOT(handleChatTitle(qint64, const QString &)));
    connect(client, SIGNAL(updateChatPhoto(qint64, const QVariantMap &)), SLOT(handleChatPhoto(qint64, const QVariantMap &)));
    connect(client, SIGNAL(updateChatPermissions(qint64, const QVariantMap &)), SLOT(handleChatPermissions(qint64, const QVariantMap &)));
    connect(client, SIGNAL(updateChatLastMessage(qint64, const QVariantMap &, const QVariantList &)),
            SLOT(handleChatLastMessage(qint64, const QVariantMap &, const QVariantList &)));
    connect(client, SIGNAL(updateChatPosition(qint64, const QVariantMap &)), SLOT(handleChatPosition(qint64, const QVariantMap &)));
    connect(client, SIGNAL(updateChatIsMarkedAsUnread(qint64, bool)), SLOT(handleChatIsMarkedAsUnread(qint64, bool)));
    connect(client, SIGNAL(updateChatIsBlocked(qint64, bool)), SLOT(handleChatIsBlocked(qint64, bool)));
    connect(client, SIGNAL(updateChatHasScheduledMessages(qint64, bool)), SLOT(handleChatHasScheduledMessages(qint64, bool)));
    connect(client, SIGNAL(updateChatDefaultDisableNotification(qint64, bool)), SLOT(handleChatDefaultDisableNotification(qint64, bool)));
    connect(client, SIGNAL(updateChatReadInbox(qint64, qint64, int)), SLOT(handleChatReadInbox(qint64, qint64, int)));
    connect(client, SIGNAL(updateChatReadOutbox(qint64, qint64)), SLOT(handleChatReadOutbox(qint64, qint64)));
    connect(client, SIGNAL(updateChatUnreadMentionCount(qint64, int)), SLOT(handleChatUnreadMentionCount(qint64, int)));
    connect(client, SIGNAL(updateChatNotificationSettings(qint64, const QVariantMap &)),
            SLOT(handleChatNotificationSettings(qint64, const QVariantMap &)));
    connect(client, SIGNAL(updateChatActionBar(qint64, const QVariantMap &)), SLOT(handleChatActionBar(qint64, const QVariantMap &)));
    connect(client, SIGNAL(updateChatReplyMarkup(qint64, qint64)), SLOT(handleChatReplyMarkup(qint64, qint64)));
    connect(client, SIGNAL(updateChatDraftMessage(qint64, const QVariantMap &, const QVariantList &)),
            SLOT(handleChatDraftMessage(qint64, const QVariantMap &, const QVariantList &)));
}

QVector<qint64> ChatStore::getIds() const noexcept
{
    QVector<qint64> result;

    result.reserve(m_chats.size());

    std::ranges::transform(m_chats, std::back_inserter(result), [](const auto &value) { return value.first; });
    return result;
}

QVariantMap ChatStore::get(qint64 chatId) const
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        return it->second;
    }

    return {};
}

void ChatStore::set(const QVariantMap &chat) noexcept
{
    QMutexLocker lock(&m_mutex);

    m_chats.insert_or_assign(chat.value("id").toLongLong(), chat);
}

void ChatStore::handleNewChat(const QVariantMap &chat)
{
    QMutexLocker lock(&m_mutex);

    m_chats.emplace(chat.value("id").toLongLong(), chat);
}

void ChatStore::handleChatTitle(qint64 chatId, const QString &title)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("title", title);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatPhoto(qint64 chatId, const QVariantMap &photo)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("photo", photo);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatPermissions(qint64 chatId, const QVariantMap &permissions)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("permissions", permissions);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_message", lastMessage);

        if (not positions.isEmpty())
            emit updateChatPosition(chatId);

        emit updateChatItem(chatId);
    }

    setChatPositions(chatId, positions);
}

void ChatStore::handleChatPosition(qint64 chatId, const QVariantMap &position)
{
    QMutexLocker lock(&m_mutex);

    setChatPositions(chatId, QVariantList() << position);
}

void ChatStore::handleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("is_marked_as_unread", isMarkedAsUnread);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatIsBlocked(qint64 chatId, bool isBlocked)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("is_blocked", isBlocked);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatHasScheduledMessages(qint64 chatId, bool hasScheduledMessages)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("has_scheduled_messages", hasScheduledMessages);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatDefaultDisableNotification(qint64 chatId, bool defaultDisableNotification)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("default_disable_notification", defaultDisableNotification);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_read_inbox_message_id", lastReadInboxMessageId);
        it->second.insert("unread_count", unreadCount);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_read_outbox_message_id", lastReadOutboxMessageId);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatUnreadMentionCount(qint64 chatId, int unreadMentionCount)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("unread_mention_count", unreadMentionCount);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("notification_settings", notificationSettings);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatActionBar(qint64 chatId, const QVariantMap &actionBar)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("action_bar", actionBar);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatReplyMarkup(qint64 chatId, qint64 replyMarkupMessageId)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("reply_markup_message_id", replyMarkupMessageId);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatDraftMessage(qint64 chatId, const QVariantMap &draftMessage, const QVariantList &positions)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("draft_message", draftMessage);

        if (not positions.isEmpty())
            emit updateChatPosition(chatId);

        emit updateChatItem(chatId);
    }

    setChatPositions(chatId, positions);
}

void ChatStore::setChatPositions(qint64 chatId, const QVariantList &positions) noexcept
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        QVariantList result(it->second.value("positions").toList());

        std::ranges::for_each(positions, [&result](const auto &position) {
            result.erase(std::remove_if(result.begin(), result.end(),
                                        [position](const auto &value) {
                                            return Utils::chatListEquals(value.toMap().value("list").toMap(),
                                                                         position.toMap().value("list").toMap());
                                        }),
                         result.end());

            result.append(position);
        });

        it->second.insert("positions", QVariantList() << result);
    }
}

void FileStore::initialize(TdApi *client)
{
    connect(client, SIGNAL(updateFile(const QVariantMap &)), SLOT(handleFile(const QVariantMap &)));
}

QVariantMap FileStore::get(int fileId) const
{
    if (auto it = m_files.find(fileId); it != m_files.end())
    {
        return it->second;
    }

    return {};
}

void FileStore::handleFile(const QVariantMap &file)
{
    QMutexLocker lock(&m_mutex);

    m_files.emplace(file.value("id").toInt(), file);
}

void OptionStore::initialize(TdApi *client)
{
    connect(client, SIGNAL(updateOption(const QString &, const QVariantMap &)), SLOT(handleOption(const QString &, const QVariantMap &)));
}

QVariant OptionStore::get(const QString &name) const
{
    if (auto it = m_options.find(name); it != m_options.end())
        return it.value();

    return {};
}

void OptionStore::handleOption(const QString &name, const QVariantMap &value)
{
    QMutexLocker lock(&m_mutex);

    m_options.insert(name, value.value("value"));
}

void SupergroupStore::initialize(TdApi *client)
{
    connect(client, SIGNAL(updateSupergroup(const QVariantMap &)), SLOT(handleSupergroup(const QVariantMap &)));
    connect(client, SIGNAL(updateSupergroupFullInfo(qint64, const QVariantMap &)),
            SLOT(handleSupergroupFullInfo(qint64, const QVariantMap &)));
}

QVariantMap SupergroupStore::get(qint64 groupId) const
{
    if (auto it = m_supergroup.find(groupId); it != m_supergroup.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap SupergroupStore::getFullInfo(qint64 groupId) const
{
    if (auto it = m_fullInfo.find(groupId); it != m_fullInfo.end())
    {
        return it->second;
    }

    return {};
}

void SupergroupStore::handleSupergroup(const QVariantMap &supergroup)
{
    QMutexLocker lock(&m_mutex);

    m_supergroup.emplace(supergroup.value("id").toLongLong(), supergroup);
}

void SupergroupStore::handleSupergroupFullInfo(qint64 supergroupId, const QVariantMap &supergroupFullInfo)
{
    QMutexLocker lock(&m_mutex);

    m_fullInfo.emplace(supergroupId, supergroupFullInfo);
}

void UserStore::initialize(TdApi *client)
{
    connect(client, SIGNAL(updateUserStatus(qint64, const QVariantMap &)), SLOT(handleUserStatus(qint64, const QVariantMap &)));
    connect(client, SIGNAL(updateUser(const QVariantMap &)), SLOT(handleUser(const QVariantMap &)));
    connect(client, SIGNAL(updateUserFullInfo(qint64, const QVariantMap &)), SLOT(handleUserFullInfo(qint64, const QVariantMap &)));
}

qint64 UserStore::getMyId() const
{
    auto myId = TdApi::getInstance().getOption("my_id");
    if (myId.isNull())
        return {};

    return myId.toLongLong();
}

QVariantMap UserStore::get(qint64 userId) const
{
    if (auto it = m_users.find(userId); it != m_users.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap UserStore::getFullInfo(qint64 userId) const
{
    if (auto it = m_fullInfo.find(userId); it != m_fullInfo.end())
    {
        return it->second;
    }

    return {};
}

void UserStore::handleUserStatus(qint64 userId, const QVariantMap &status)
{
    QMutexLocker lock(&m_mutex);

    if (auto it = m_users.find(userId); it != m_users.end())
        it->second.insert("status", status);
}

void UserStore::handleUser(const QVariantMap &user)
{
    QMutexLocker lock(&m_mutex);

    m_users.emplace(user.value("id").toLongLong(), user);
}

void UserStore::handleUserFullInfo(qint64 userId, const QVariantMap &userFullInfo)
{
    QMutexLocker lock(&m_mutex);

    m_fullInfo.emplace(userId, userFullInfo);
}
