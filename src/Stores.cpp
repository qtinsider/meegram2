#include "Stores.hpp"

#include "TdApi.hpp"
#include "Utils.hpp"

#include <algorithm>

void BasicGroupStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateBasicGroup(const QVariantMap &)), SLOT(handleUpdateBasicGroup(const QVariantMap &)));
    connect(controller, SIGNAL(updateBasicGroupFullInfo(qint64, const QVariantMap &)),
            SLOT(handleUpdateBasicGroupFullInfo(qint64, const QVariantMap &)));
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

void BasicGroupStore::handleUpdateBasicGroup(const QVariantMap &basicGroup)
{
    auto groupId = basicGroup.value("id").toLongLong();

    m_basicGroup.emplace(groupId, basicGroup);
}

void BasicGroupStore::handleUpdateBasicGroupFullInfo(qint64 basicGroupId, const QVariantMap &basicGroupFullInfo)
{
    m_fullInfo.emplace(basicGroupId, basicGroupFullInfo);
}

void ChatStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateNewChat(const QVariantMap &)), SLOT(handleNewChat(const QVariantMap &)));
    connect(controller, SIGNAL(updateChatTitle(qint64, const QString &)), SLOT(handleChatTitle(qint64, const QString &)));
    connect(controller, SIGNAL(updateChatPhoto(qint64, const QVariantMap &)), SLOT(handleChatPhoto(qint64, const QVariantMap &)));
    connect(controller, SIGNAL(updateChatPermissions(qint64, const QVariantMap &)),
            SLOT(handleChatPermissions(qint64, const QVariantMap &)));
    connect(controller, SIGNAL(updateChatLastMessage(qint64, const QVariantMap &, const QVariantList &)),
            SLOT(handleChatLastMessage(qint64, const QVariantMap &, const QVariantList &)));
    connect(controller, SIGNAL(updateChatPosition(qint64, const QVariantMap &)), SLOT(handleChatPosition(qint64, const QVariantMap &)));
    connect(controller, SIGNAL(updateChatIsMarkedAsUnread(qint64, bool)), SLOT(handleChatIsMarkedAsUnread(qint64, bool)));
    connect(controller, SIGNAL(updateChatIsBlocked(qint64, bool)), SLOT(handleChatIsBlocked(qint64, bool)));
    connect(controller, SIGNAL(updateChatHasScheduledMessages(qint64, bool)), SLOT(handleChatHasScheduledMessages(qint64, bool)));
    connect(controller, SIGNAL(updateChatDefaultDisableNotification(qint64, bool)),
            SLOT(handleChatDefaultDisableNotification(qint64, bool)));
    connect(controller, SIGNAL(updateChatReadInbox(qint64, qint64, int)), SLOT(handleChatReadInbox(qint64, qint64, int)));
    connect(controller, SIGNAL(updateChatReadOutbox(qint64, qint64)), SLOT(handleChatReadOutbox(qint64, qint64)));
    connect(controller, SIGNAL(updateChatUnreadMentionCount(qint64, int)), SLOT(handleChatUnreadMentionCount(qint64, int)));
    connect(controller, SIGNAL(updateChatNotificationSettings(qint64, const QVariantMap &)),
            SLOT(handleChatNotificationSettings(qint64, const QVariantMap &)));
    connect(controller, SIGNAL(updateChatActionBar(qint64, const QVariantMap &)), SLOT(handleChatActionBar(qint64, const QVariantMap &)));
    connect(controller, SIGNAL(updateChatReplyMarkup(qint64, qint64)), SLOT(handleChatReplyMarkup(qint64, qint64)));
    connect(controller, SIGNAL(updateChatDraftMessage(qint64, const QVariantMap &, const QVariantList &)),
            SLOT(handleChatDraftMessage(qint64, const QVariantMap &, const QVariantList &)));
}

QList<qint64> ChatStore::getIds() const noexcept
{
    QList<qint64> result;
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

void ChatStore::handleNewChat(const QVariantMap &chat)
{
    m_chats.emplace(chat.value("id").toLongLong(), chat);
}

void ChatStore::handleChatTitle(qint64 chatId, const QString &title)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("title", title);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatPhoto(qint64 chatId, const QVariantMap &photo)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("photo", photo);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatPermissions(qint64 chatId, const QVariantMap &permissions)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("permissions", permissions);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions)
{
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
    setChatPositions(chatId, QVariantList() << position);
}

void ChatStore::handleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("is_marked_as_unread", isMarkedAsUnread);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatIsBlocked(qint64 chatId, bool isBlocked)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("is_blocked", isBlocked);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatHasScheduledMessages(qint64 chatId, bool hasScheduledMessages)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("has_scheduled_messages", hasScheduledMessages);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatDefaultDisableNotification(qint64 chatId, bool defaultDisableNotification)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("default_disable_notification", defaultDisableNotification);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_read_inbox_message_id", lastReadInboxMessageId);
        it->second.insert("unread_count", unreadCount);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("last_read_outbox_message_id", lastReadOutboxMessageId);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatUnreadMentionCount(qint64 chatId, int unreadMentionCount)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("unread_mention_count", unreadMentionCount);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("notification_settings", notificationSettings);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatActionBar(qint64 chatId, const QVariantMap &actionBar)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("action_bar", actionBar);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatReplyMarkup(qint64 chatId, qint64 replyMarkupMessageId)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->second.insert("reply_markup_message_id", replyMarkupMessageId);

        emit updateChatItem(chatId);
    }
}

void ChatStore::handleChatDraftMessage(qint64 chatId, const QVariantMap &draftMessage, const QVariantList &positions)
{
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

void FileStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateFile(const QVariantMap &)), SLOT(handleUpdateFile(const QVariantMap &)));
}

QVariantMap FileStore::get(int fileId) const
{
    if (auto it = m_files.find(fileId); it != m_files.end())
    {
        return it->second;
    }

    return {};
}

void FileStore::handleUpdateFile(const QVariantMap &file)
{
    m_files.emplace(file.value("id").toInt(), file);
}

void OptionStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateOption(const QString &, const QVariantMap &)),
            SLOT(handleUpdateOption(const QString &, const QVariantMap &)));
}

QVariant OptionStore::get(const QString &name) const
{
    if (m_options.contains(name))
        return m_options.value(name);

    return {};
}

void OptionStore::handleUpdateOption(const QString &name, const QVariantMap &value)
{
    m_options.insert(name, value.value("value"));
}

void SupergroupStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateSupergroup(const QVariantMap &)), SLOT(handleUpdateSupergroup(const QVariantMap &)));
    connect(controller, SIGNAL(updateSupergroupFullInfo(qint64, const QVariantMap &)),
            SLOT(handleUpdateSupergroupFullInfo(qint64, const QVariantMap &)));
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

void SupergroupStore::handleUpdateSupergroup(const QVariantMap &supergroup)
{
    m_supergroup.emplace(supergroup.value("id").toLongLong(), supergroup);
}

void SupergroupStore::handleUpdateSupergroupFullInfo(qint64 supergroupId, const QVariantMap &supergroupFullInfo)
{
    m_fullInfo.emplace(supergroupId, supergroupFullInfo);
}

void UserStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateUserStatus(qint64, const QVariantMap &)), SLOT(handleUpdateUserStatus(qint64, const QVariantMap &)));
    connect(controller, SIGNAL(updateUser(const QVariantMap &)), SLOT(handleUpdateUser(const QVariantMap &)));
    connect(controller, SIGNAL(updateUserFullInfo(qint64, const QVariantMap &)),
            SLOT(handleUpdateUserFullInfo(qint64, const QVariantMap &)));
}

qint64 UserStore::getMyId() const
{
    auto myId = TdApi::getInstance().optionStore->get("my_id");
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

void UserStore::handleUpdateUserStatus(qint64 userId, const QVariantMap &status)
{
    if (auto it = m_users.find(userId); it != m_users.end())
        it->second.insert("status", status);
}

void UserStore::handleUpdateUser(const QVariantMap &user)
{
    auto userId = user.value("id").toLongLong();

    m_users.emplace(userId, user);
}

void UserStore::handleUpdateUserFullInfo(qint64 userId, const QVariantMap &userFullInfo)
{
    m_fullInfo.emplace(userId, userFullInfo);
}
