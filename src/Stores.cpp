#include "Stores.hpp"

#include "TdApi.hpp"

void BasicGroupStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateBasicGroup(const QVariantMap &)), SLOT(handleUpdateBasicGroup(const QVariantMap &)));
    connect(controller, SIGNAL(updateBasicGroupFullInfo(int, const QVariantMap &)),
            SLOT(handleUpdateBasicGroupFullInfo(int, const QVariantMap &)));
}

QVariantMap BasicGroupStore::get(int groupId) const
{
    if (m_basicGroup.contains(groupId))
        return m_basicGroup.value(groupId);

    return {};
}

QVariantMap BasicGroupStore::getFullInfo(int groupId) const
{
    if (m_fullInfo.contains(groupId))
        return m_fullInfo.value(groupId);

    return {};
}

void BasicGroupStore::handleUpdateBasicGroup(const QVariantMap &basicGroup)
{
    auto groupId = basicGroup.value("id").toInt();

    m_basicGroup.insert(groupId, basicGroup);
}

void BasicGroupStore::handleUpdateBasicGroupFullInfo(int basicGroupId, const QVariantMap &basicGroupFullInfo)
{
    m_fullInfo.insert(basicGroupId, basicGroupFullInfo);
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

    connect(controller, SIGNAL(updateChatOnlineMemberCount(qint64, int)), SLOT(handleChatOnlineMemberCount(qint64, int)));
}

QVariantMap ChatStore::get(qint64 chatId) const
{
    if (m_chats.contains(chatId))
        return m_chats.value(chatId);

    return {};
}

int ChatStore::getOnlineMemberCount(qint64 chatId) const
{
    if (m_onlineMemberCount.contains(chatId))
        return m_onlineMemberCount.value(chatId);

    return {};
}

void ChatStore::handleNewChat(const QVariantMap &chat)
{
    m_chats.insert(chat.value("id").toLongLong(), chat);
}

void ChatStore::handleChatTitle(qint64 chatId, const QString &title)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("title", title);
    }
}

void ChatStore::handleChatPhoto(qint64 chatId, const QVariantMap &photo)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("photo", photo);
    }
}

void ChatStore::handleChatPermissions(qint64 chatId, const QVariantMap &permissions)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("permissions", permissions);
    }
}

void ChatStore::handleChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("last_message", lastMessage);
        it->insert("positions", positions);
    }
}

void ChatStore::handleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
        it->insert("is_marked_as_unread", isMarkedAsUnread);
}

void ChatStore::handleChatIsBlocked(qint64 chatId, bool isBlocked)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
        it->insert("is_blocked", isBlocked);
}

void ChatStore::handleChatHasScheduledMessages(qint64 chatId, bool hasScheduledMessages)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
        it->insert("has_scheduled_messages", hasScheduledMessages);
}

void ChatStore::handleChatDefaultDisableNotification(qint64 chatId, bool defaultDisableNotification)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("default_disable_notification", defaultDisableNotification);
    }
}

void ChatStore::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("last_read_inbox_message_id", lastReadInboxMessageId);
        it->insert("unread_count", unreadCount);
    }
}

void ChatStore::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("last_read_outbox_message_id", lastReadOutboxMessageId);
    }
}

void ChatStore::handleChatUnreadMentionCount(qint64 chatId, int unreadMentionCount)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("unread_mention_count", unreadMentionCount);
    }
}

void ChatStore::handleChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("notification_settings", notificationSettings);
    }
}

void ChatStore::handleChatActionBar(qint64 chatId, const QVariantMap &actionBar)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("action_bar", actionBar);
    }
}

void ChatStore::handleChatReplyMarkup(qint64 chatId, qint64 replyMarkupMessageId)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("reply_markup_message_id", replyMarkupMessageId);
    }
}

void ChatStore::handleChatDraftMessage(qint64 chatId, const QVariantMap &draftMessage, const QVariantList &positions)
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        it->insert("draft_message", draftMessage);
        it->insert("positions", positions);
    }
}

void ChatStore::handleChatOnlineMemberCount(qint64 chatId, int onlineMemberCount)
{
    m_onlineMemberCount.insert(chatId, onlineMemberCount);
}

void FileStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateFile(const QVariantMap &)),
            SLOT(handleUpdateFile(const QVariantMap &)));
}

QVariantMap FileStore::get(int fileId) const
{
    if (m_files.contains(fileId))
        return m_files.value(fileId);

    return {};
}

void FileStore::handleUpdateFile(const QVariantMap &file)
{
    m_files.insert(file.value("id").toInt(), file);
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
    connect(controller, SIGNAL(updateSupergroupFullInfo(int, const QVariantMap &)),
            SLOT(handleUpdateSupergroupFullInfo(int, const QVariantMap &)));
}

QVariantMap SupergroupStore::get(int groupId) const
{
    if (m_supergroup.contains(groupId))
        return m_supergroup.value(groupId);

    return {};
}

QVariantMap SupergroupStore::getFullInfo(int groupId) const
{
    if (m_fullInfo.contains(groupId))
        return m_fullInfo.value(groupId);

    return {};
}

void SupergroupStore::handleUpdateSupergroup(const QVariantMap &supergroup)
{
    m_supergroup.insert(supergroup.value("id").toInt(), supergroup);
}

void SupergroupStore::handleUpdateSupergroupFullInfo(int supergroupId, const QVariantMap &supergroupFullInfo)
{
    m_fullInfo.insert(supergroupId, supergroupFullInfo);
}

void UserStore::initialize(TdApi *controller)
{
    connect(controller, SIGNAL(updateUserStatus(int, const QVariantMap &)), SLOT(handleUpdateUserStatus(int, const QVariantMap &)));
    connect(controller, SIGNAL(updateUser(const QVariantMap &)), SLOT(handleUpdateUser(const QVariantMap &)));
    connect(controller, SIGNAL(updateUserFullInfo(int, const QVariantMap &)), SLOT(handleUpdateUserFullInfo(int, const QVariantMap &)));
}

int UserStore::getMyId() const
{
    auto myId = TdApi::getInstance().optionStore->get("my_id");
    if (myId.isNull())
        return {};

    return myId.toInt();
}

QVariantMap UserStore::get(int userId) const
{
    if (m_users.contains(userId))
        return m_users.value(userId);

    return {};
}

QVariantMap UserStore::getFullInfo(int userId) const
{
    if (m_fullInfo.contains(userId))
        return m_fullInfo.value(userId);

    return {};
}

void UserStore::handleUpdateUserStatus(int userId, const QVariantMap &status)
{
    if (auto it = m_users.find(userId); it != m_users.end())
        it->insert("status", status);
}

void UserStore::handleUpdateUser(const QVariantMap &user)
{
    auto userId = user.value("id").toInt();

    m_users.insert(userId, user);
}

void UserStore::handleUpdateUserFullInfo(int userId, const QVariantMap &userFullInfo)
{
    m_fullInfo.insert(userId, userFullInfo);
}
