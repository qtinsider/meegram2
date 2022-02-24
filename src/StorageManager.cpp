#include "StorageManager.hpp"

#include "TdApi.hpp"
#include "Utils.hpp"

#include <QMutexLocker>

#include <algorithm>

StorageManager::StorageManager()
{
    connect(&TdApi::getInstance(), SIGNAL(updateBasicGroup(const QVariantMap &)), SLOT(handleBasicGroup(const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateBasicGroupFullInfo(qint64, const QVariantMap &)),
            SLOT(handleBasicGroupFullInfo(qint64, const QVariantMap &)));

    connect(&TdApi::getInstance(), SIGNAL(updateNewChat(const QVariantMap &)), SLOT(handleNewChat(const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatTitle(qint64, const QString &)), SLOT(handleChatTitle(qint64, const QString &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatPhoto(qint64, const QVariantMap &)),
            SLOT(handleChatPhoto(qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatPermissions(qint64, const QVariantMap &)),
            SLOT(handleChatPermissions(qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatLastMessage(qint64, const QVariantMap &, const QVariantList &)),
            SLOT(handleChatLastMessage(qint64, const QVariantMap &, const QVariantList &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatPosition(qint64, const QVariantMap &)),
            SLOT(handleChatPosition(qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatIsMarkedAsUnread(qint64, bool)), SLOT(handleChatIsMarkedAsUnread(qint64, bool)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatIsBlocked(qint64, bool)), SLOT(handleChatIsBlocked(qint64, bool)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatHasScheduledMessages(qint64, bool)),
            SLOT(handleChatHasScheduledMessages(qint64, bool)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatDefaultDisableNotification(qint64, bool)),
            SLOT(handleChatDefaultDisableNotification(qint64, bool)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatReadInbox(qint64, qint64, int)), SLOT(handleChatReadInbox(qint64, qint64, int)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatReadOutbox(qint64, qint64)), SLOT(handleChatReadOutbox(qint64, qint64)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatUnreadMentionCount(qint64, int)), SLOT(handleChatUnreadMentionCount(qint64, int)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatNotificationSettings(qint64, const QVariantMap &)),
            SLOT(handleChatNotificationSettings(qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatActionBar(qint64, const QVariantMap &)),
            SLOT(handleChatActionBar(qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatReplyMarkup(qint64, qint64)), SLOT(handleChatReplyMarkup(qint64, qint64)));
    connect(&TdApi::getInstance(), SIGNAL(updateChatDraftMessage(qint64, const QVariantMap &, const QVariantList &)),
            SLOT(handleChatDraftMessage(qint64, const QVariantMap &, const QVariantList &)));

    connect(&TdApi::getInstance(), SIGNAL(updateFile(const QVariantMap &)), SLOT(handleFile(const QVariantMap &)));

    connect(&TdApi::getInstance(), SIGNAL(updateOption(const QString &, const QVariantMap &)),
            SLOT(handleOption(const QString &, const QVariantMap &)));

    connect(&TdApi::getInstance(), SIGNAL(updateSupergroup(const QVariantMap &)), SLOT(handleSupergroup(const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateSupergroupFullInfo(qint64, const QVariantMap &)),
            SLOT(handleSupergroupFullInfo(qint64, const QVariantMap &)));

    connect(&TdApi::getInstance(), SIGNAL(updateUserStatus(qint64, const QVariantMap &)),
            SLOT(handleUserStatus(qint64, const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateUser(const QVariantMap &)), SLOT(handleUser(const QVariantMap &)));
    connect(&TdApi::getInstance(), SIGNAL(updateUserFullInfo(qint64, const QVariantMap &)),
            SLOT(handleUserFullInfo(qint64, const QVariantMap &)));
}

StorageManager &StorageManager::getInstance()
{
    static StorageManager staticObject;
    return staticObject;
}

QVector<qint64> StorageManager::getChatIds() const noexcept
{
    QVector<qint64> result;

    result.reserve(m_chats.size());

    std::ranges::transform(m_chats, std::back_inserter(result), [](const auto &value) { return value.first; });
    return result;
}

QVariantMap StorageManager::getBasicGroup(qint64 groupId) const
{
    if (auto it = m_basicGroup.find(groupId); it != m_basicGroup.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getBasicGroupFullInfo(qint64 groupId) const
{
    if (auto it = m_basicGroupFullInfo.find(groupId); it != m_basicGroupFullInfo.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getChat(qint64 chatId) const
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getFile(qint32 fileId) const
{
    if (auto it = m_files.find(fileId); it != m_files.end())
    {
        return it->second;
    }

    return {};
}

QVariant StorageManager::getOption(const QString &name) const
{
    if (auto it = m_options.find(name); it != m_options.end())
        return it.value();

    return {};
}

QVariantMap StorageManager::getSupergroup(qint64 groupId) const
{
    if (auto it = m_supergroup.find(groupId); it != m_supergroup.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getSupergroupFullInfo(qint64 groupId) const
{
    if (auto it = m_supergroupFullInfo.find(groupId); it != m_supergroupFullInfo.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getUser(qint64 userId) const
{
    if (auto it = m_users.find(userId); it != m_users.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getUserFullInfo(qint64 userId) const
{
    if (auto it = m_userFullInfo.find(userId); it != m_userFullInfo.end())
    {
        return it->second;
    }

    return {};
}

qint64 StorageManager::getMyId() const
{
    const auto myId = getOption("my_id");
    if (myId.isNull())
        return {};

    return myId.toLongLong();
}

void StorageManager::setChat(const QVariantMap &chat) noexcept
{
    QMutexLocker lock(&m_chatMutex);

    m_chats.insert_or_assign(chat.value("id").toLongLong(), chat);
}

void StorageManager::handleBasicGroup(const QVariantMap &basicGroup)
{
    m_basicGroup.emplace(basicGroup.value("id").toLongLong(), basicGroup);
}

void StorageManager::handleBasicGroupFullInfo(qint64 basicGroupId, const QVariantMap &basicGroupFullInfo)
{
    m_basicGroupFullInfo.emplace(basicGroupId, basicGroupFullInfo);
}

void StorageManager::handleNewChat(const QVariantMap &chat)
{
    QMutexLocker lock(&m_chatMutex);

    m_chats.emplace(chat.value("id").toLongLong(), chat);
}

void StorageManager::handleChatTitle(qint64 chatId, const QString &title)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("title", title);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatPhoto(qint64 chatId, const QVariantMap &photo)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("photo", photo);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatPermissions(qint64 chatId, const QVariantMap &permissions)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("permissions", permissions);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_message", lastMessage);

        if (not positions.isEmpty())
            emit updateChatPosition(chatId);

        emit updateChatItem(chatId);
    }

    setChatPositions(chatId, positions);
}

void StorageManager::handleChatPosition(qint64 chatId, const QVariantMap &position)
{
    QMutexLocker lock(&m_chatMutex);

    setChatPositions(chatId, QVariantList() << position);
}

void StorageManager::handleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("is_marked_as_unread", isMarkedAsUnread);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatIsBlocked(qint64 chatId, bool isBlocked)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("is_blocked", isBlocked);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatHasScheduledMessages(qint64 chatId, bool hasScheduledMessages)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("has_scheduled_messages", hasScheduledMessages);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatDefaultDisableNotification(qint64 chatId, bool defaultDisableNotification)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("default_disable_notification", defaultDisableNotification);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_read_inbox_message_id", lastReadInboxMessageId);
        it->second.insert("unread_count", unreadCount);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_read_outbox_message_id", lastReadOutboxMessageId);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatUnreadMentionCount(qint64 chatId, int unreadMentionCount)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("unread_mention_count", unreadMentionCount);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("notification_settings", notificationSettings);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatActionBar(qint64 chatId, const QVariantMap &actionBar)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("action_bar", actionBar);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatReplyMarkup(qint64 chatId, qint64 replyMarkupMessageId)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("reply_markup_message_id", replyMarkupMessageId);

        emit updateChatItem(chatId);
    }
}

void StorageManager::handleChatDraftMessage(qint64 chatId, const QVariantMap &draftMessage, const QVariantList &positions)
{
    QMutexLocker lock(&m_chatMutex);

    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("draft_message", draftMessage);

        if (not positions.isEmpty())
            emit updateChatPosition(chatId);

        emit updateChatItem(chatId);
    }

    setChatPositions(chatId, positions);
}

void StorageManager::setChatPositions(qint64 chatId, const QVariantList &positions) noexcept
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

void StorageManager::handleFile(const QVariantMap &file)
{
    m_files.emplace(file.value("id").toInt(), file);
}

void StorageManager::handleOption(const QString &name, const QVariantMap &value)
{
    m_options.insert(name, value.value("value"));
}

void StorageManager::handleSupergroup(const QVariantMap &supergroup)
{
    m_supergroup.emplace(supergroup.value("id").toLongLong(), supergroup);
}

void StorageManager::handleSupergroupFullInfo(qint64 supergroupId, const QVariantMap &supergroupFullInfo)
{
    m_supergroupFullInfo.emplace(supergroupId, supergroupFullInfo);
}

void StorageManager::handleUserStatus(qint64 userId, const QVariantMap &status)
{
    if (auto it = m_users.find(userId); it != m_users.end())
        it->second.insert("status", status);
}

void StorageManager::handleUser(const QVariantMap &user)
{
    m_users.emplace(user.value("id").toLongLong(), user);
}

void StorageManager::handleUserFullInfo(qint64 userId, const QVariantMap &userFullInfo)
{
    m_userFullInfo.emplace(userId, userFullInfo);
}
