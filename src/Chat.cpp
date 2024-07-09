#include "Chat.hpp"

#include "Message.hpp"

Chat::Chat(QObject *parent)
    : QObject(parent)
{
}

Chat::~Chat()
{
}

qint64 Chat::id() const
{
    return m_id;
}
QVariantMap Chat::type() const
{
    return m_type;
}
QString Chat::title() const
{
    return m_title;
}
QVariantMap Chat::photo() const
{
    return m_photo;
}
qint32 Chat::accentColorId() const
{
    return m_accentColorId;
}
qint64 Chat::backgroundCustomEmojiId() const
{
    return m_backgroundCustomEmojiId;
}
qint32 Chat::profileAccentColorId() const
{
    return m_profileAccentColorId;
}
qint64 Chat::profileBackgroundCustomEmojiId() const
{
    return m_profileBackgroundCustomEmojiId;
}
QVariantMap Chat::permissions() const
{
    return m_permissions;
}
Message *Chat::lastMessage() const
{
    return m_lastMessage;
}
QVariantList Chat::positions() const
{
    return m_positions;
}
QVariantList Chat::chatLists() const
{
    return m_chatLists;
}
QVariantMap Chat::messageSenderId() const
{
    return m_messageSenderId;
}
QVariantMap Chat::blockList() const
{
    return m_blockList;
}
bool Chat::hasProtectedContent() const
{
    return m_hasProtectedContent;
}
bool Chat::isTranslatable() const
{
    return m_isTranslatable;
}
bool Chat::isMarkedAsUnread() const
{
    return m_isMarkedAsUnread;
}
bool Chat::viewAsTopics() const
{
    return m_viewAsTopics;
}
bool Chat::hasScheduledMessages() const
{
    return m_hasScheduledMessages;
}
bool Chat::canBeDeletedOnlyForSelf() const
{
    return m_canBeDeletedOnlyForSelf;
}
bool Chat::canBeDeletedForAllUsers() const
{
    return m_canBeDeletedForAllUsers;
}
bool Chat::canBeReported() const
{
    return m_canBeReported;
}
bool Chat::defaultDisableNotification() const
{
    return m_defaultDisableNotification;
}
qint32 Chat::unreadCount() const
{
    return m_unreadCount;
}
qint64 Chat::lastReadInboxMessageId() const
{
    return m_lastReadInboxMessageId;
}
qint64 Chat::lastReadOutboxMessageId() const
{
    return m_lastReadOutboxMessageId;
}
qint32 Chat::unreadMentionCount() const
{
    return m_unreadMentionCount;
}
qint32 Chat::unreadReactionCount() const
{
    return m_unreadReactionCount;
}
QVariantMap Chat::notificationSettings() const
{
    return m_notificationSettings;
}
QVariantMap Chat::availableReactions() const
{
    return m_availableReactions;
}
qint32 Chat::messageAutoDeleteTime() const
{
    return m_messageAutoDeleteTime;
}
QVariantMap Chat::emojiStatus() const
{
    return m_emojiStatus;
}
QVariantMap Chat::background() const
{
    return m_background;
}
QString Chat::themeName() const
{
    return m_themeName;
}
QVariantMap Chat::actionBar() const
{
    return m_actionBar;
}
QVariantMap Chat::businessBotManageBar() const
{
    return m_businessBotManageBar;
}
QVariantMap Chat::videoChat() const
{
    return m_videoChat;
}
QVariantMap Chat::pendingJoinRequests() const
{
    return m_pendingJoinRequests;
}
qint64 Chat::replyMarkupMessageId() const
{
    return m_replyMarkupMessageId;
}
Message *Chat::draftMessage() const
{
    return m_draftMessage;
}
QString Chat::clientData() const
{
    return m_clientData;
}

void Chat::setId(qint64 id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}
void Chat::setType(const QVariantMap &type)
{
    if (m_type != type)
    {
        m_type = type;
        emit typeChanged();
    }
}
void Chat::setTitle(const QString &title)
{
    if (m_title != title)
    {
        m_title = title;
        emit titleChanged();
    }
}
void Chat::setPhoto(const QVariantMap &photo)
{
    if (m_photo != photo)
    {
        m_photo = photo;
        emit photoChanged();
    }
}
void Chat::setAccentColorId(qint32 accentColorId)
{
    if (m_accentColorId != accentColorId)
    {
        m_accentColorId = accentColorId;
        emit accentColorIdChanged();
    }
}
void Chat::setBackgroundCustomEmojiId(qint64 backgroundCustomEmojiId)
{
    if (m_backgroundCustomEmojiId != backgroundCustomEmojiId)
    {
        m_backgroundCustomEmojiId = backgroundCustomEmojiId;
        emit backgroundCustomEmojiIdChanged();
    }
}
void Chat::setProfileAccentColorId(qint32 profileAccentColorId)
{
    if (m_profileAccentColorId != profileAccentColorId)
    {
        m_profileAccentColorId = profileAccentColorId;
        emit profileAccentColorIdChanged();
    }
}
void Chat::setProfileBackgroundCustomEmojiId(qint64 profileBackgroundCustomEmojiId)
{
    if (m_profileBackgroundCustomEmojiId != profileBackgroundCustomEmojiId)
    {
        m_profileBackgroundCustomEmojiId = profileBackgroundCustomEmojiId;
        emit profileBackgroundCustomEmojiIdChanged();
    }
}
void Chat::setPermissions(const QVariantMap &permissions)
{
    if (m_permissions != permissions)
    {
        m_permissions = permissions;
        emit permissionsChanged();
    }
}
void Chat::setLastMessage(Message *lastMessage)
{
    if (m_lastMessage != lastMessage)
    {
        m_lastMessage = lastMessage;
        emit lastMessageChanged();
    }
}
void Chat::setPositions(const QVariantList &positions)
{
    if (m_positions != positions)
    {
        m_positions = positions;
        emit positionsChanged();
    }
}
void Chat::setChatLists(const QVariantList &chatLists)
{
    if (m_chatLists != chatLists)
    {
        m_chatLists = chatLists;
        emit chatListsChanged();
    }
}
void Chat::setMessageSenderId(const QVariantMap &messageSenderId)
{
    if (m_messageSenderId != messageSenderId)
    {
        m_messageSenderId = messageSenderId;
        emit messageSenderIdChanged();
    }
}
void Chat::setBlockList(const QVariantMap &blockList)
{
    if (m_blockList != blockList)
    {
        m_blockList = blockList;
        emit blockListChanged();
    }
}
void Chat::setHasProtectedContent(bool hasProtectedContent)
{
    if (m_hasProtectedContent != hasProtectedContent)
    {
        m_hasProtectedContent = hasProtectedContent;
        emit hasProtectedContentChanged();
    }
}
void Chat::setIsTranslatable(bool isTranslatable)
{
    if (m_isTranslatable != isTranslatable)
    {
        m_isTranslatable = isTranslatable;
        emit isTranslatableChanged();
    }
}
void Chat::setIsMarkedAsUnread(bool isMarkedAsUnread)
{
    if (m_isMarkedAsUnread != isMarkedAsUnread)
    {
        m_isMarkedAsUnread = isMarkedAsUnread;
        emit isMarkedAsUnreadChanged();
    }
}
void Chat::setViewAsTopics(bool viewAsTopics)
{
    if (m_viewAsTopics != viewAsTopics)
    {
        m_viewAsTopics = viewAsTopics;
        emit viewAsTopicsChanged();
    }
}
void Chat::setHasScheduledMessages(bool hasScheduledMessages)
{
    if (m_hasScheduledMessages != hasScheduledMessages)
    {
        m_hasScheduledMessages = hasScheduledMessages;
        emit hasScheduledMessagesChanged();
    }
}
void Chat::setCanBeDeletedOnlyForSelf(bool canBeDeletedOnlyForSelf)
{
    if (m_canBeDeletedOnlyForSelf != canBeDeletedOnlyForSelf)
    {
        m_canBeDeletedOnlyForSelf = canBeDeletedOnlyForSelf;
        emit canBeDeletedOnlyForSelfChanged();
    }
}
void Chat::setCanBeDeletedForAllUsers(bool canBeDeletedForAllUsers)
{
    if (m_canBeDeletedForAllUsers != canBeDeletedForAllUsers)
    {
        m_canBeDeletedForAllUsers = canBeDeletedForAllUsers;
        emit canBeDeletedForAllUsersChanged();
    }
}
void Chat::setCanBeReported(bool canBeReported)
{
    if (m_canBeReported != canBeReported)
    {
        m_canBeReported = canBeReported;
        emit canBeReportedChanged();
    }
}
void Chat::setDefaultDisableNotification(bool defaultDisableNotification)
{
    if (m_defaultDisableNotification != defaultDisableNotification)
    {
        m_defaultDisableNotification = defaultDisableNotification;
        emit defaultDisableNotificationChanged();
    }
}
void Chat::setUnreadCount(qint32 unreadCount)
{
    if (m_unreadCount != unreadCount)
    {
        m_unreadCount = unreadCount;
        emit unreadCountChanged();
    }
}
void Chat::setLastReadInboxMessageId(qint64 lastReadInboxMessageId)
{
    if (m_lastReadInboxMessageId != lastReadInboxMessageId)
    {
        m_lastReadInboxMessageId = lastReadInboxMessageId;
        emit lastReadInboxMessageIdChanged();
    }
}
void Chat::setLastReadOutboxMessageId(qint64 lastReadOutboxMessageId)
{
    if (m_lastReadOutboxMessageId != lastReadOutboxMessageId)
    {
        m_lastReadOutboxMessageId = lastReadOutboxMessageId;
        emit lastReadOutboxMessageIdChanged();
    }
}
void Chat::setUnreadMentionCount(qint32 unreadMentionCount)
{
    if (m_unreadMentionCount != unreadMentionCount)
    {
        m_unreadMentionCount = unreadMentionCount;
        emit unreadMentionCountChanged();
    }
}
void Chat::setUnreadReactionCount(qint32 unreadReactionCount)
{
    if (m_unreadReactionCount != unreadReactionCount)
    {
        m_unreadReactionCount = unreadReactionCount;
        emit unreadReactionCountChanged();
    }
}
void Chat::setNotificationSettings(const QVariantMap &notificationSettings)
{
    if (m_notificationSettings != notificationSettings)
    {
        m_notificationSettings = notificationSettings;
        emit notificationSettingsChanged();
    }
}
void Chat::setAvailableReactions(const QVariantMap &availableReactions)
{
    if (m_availableReactions != availableReactions)
    {
        m_availableReactions = availableReactions;
        emit availableReactionsChanged();
    }
}
void Chat::setMessageAutoDeleteTime(qint32 messageAutoDeleteTime)
{
    if (m_messageAutoDeleteTime != messageAutoDeleteTime)
    {
        m_messageAutoDeleteTime = messageAutoDeleteTime;
        emit messageAutoDeleteTimeChanged();
    }
}
void Chat::setEmojiStatus(const QVariantMap &emojiStatus)
{
    if (m_emojiStatus != emojiStatus)
    {
        m_emojiStatus = emojiStatus;
        emit emojiStatusChanged();
    }
}
void Chat::setBackground(const QVariantMap &background)
{
    if (m_background != background)
    {
        m_background = background;
        emit backgroundChanged();
    }
}
void Chat::setThemeName(const QString &themeName)
{
    if (m_themeName != themeName)
    {
        m_themeName = themeName;
        emit themeNameChanged();
    }
}
void Chat::setActionBar(const QVariantMap &actionBar)
{
    if (m_actionBar != actionBar)
    {
        m_actionBar = actionBar;
        emit actionBarChanged();
    }
}
void Chat::setBusinessBotManageBar(const QVariantMap &businessBotManageBar)
{
    if (m_businessBotManageBar != businessBotManageBar)
    {
        m_businessBotManageBar = businessBotManageBar;
        emit businessBotManageBarChanged();
    }
}
void Chat::setVideoChat(const QVariantMap &videoChat)
{
    if (m_videoChat != videoChat)
    {
        m_videoChat = videoChat;
        emit videoChatChanged();
    }
}
void Chat::setPendingJoinRequests(const QVariantMap &pendingJoinRequests)
{
    if (m_pendingJoinRequests != pendingJoinRequests)
    {
        m_pendingJoinRequests = pendingJoinRequests;
        emit pendingJoinRequestsChanged();
    }
}
void Chat::setReplyMarkupMessageId(qint64 replyMarkupMessageId)
{
    if (m_replyMarkupMessageId != replyMarkupMessageId)
    {
        m_replyMarkupMessageId = replyMarkupMessageId;
        emit replyMarkupMessageIdChanged();
    }
}
void Chat::setDraftMessage(Message *draftMessage)
{
    if (m_draftMessage != draftMessage)
    {
        m_draftMessage = draftMessage;
        emit draftMessageChanged();
    }
}
void Chat::setClientData(const QString &clientData)
{
    if (m_clientData != clientData)
    {
        m_clientData = clientData;
        emit clientDataChanged();
    }
}

void Chat::setFromVariantMap(const QVariantMap &map)
{
    setId(map.value("id").toLongLong());
    setType(map.value("type").toMap());
    setTitle(map.value("title").toString());
    setPhoto(map.value("photo").toMap());
    setAccentColorId(map.value("accent_color_id").toInt());
    setBackgroundCustomEmojiId(map.value("background_custom_emoji_id").toLongLong());
    setProfileAccentColorId(map.value("profile_accent_color_id").toInt());
    setProfileBackgroundCustomEmojiId(map.value("profile_background_custom_emoji_id").toLongLong());
    setPermissions(map.value("permissions").toMap());

    if (map.contains("last_message"))
    {
        QVariantMap last_message = map.value("last_message").toMap();
        if (!m_lastMessage)
        {
            m_lastMessage = new Message(this);
        }
        m_lastMessage->setFromVariantMap(last_message);
        emit lastMessageChanged();
    }

    setPositions(map.value("positions").toList());
    setChatLists(map.value("chat_lists").toList());
    setMessageSenderId(map.value("message_sender_id").toMap());
    setBlockList(map.value("block_list").toMap());
    setHasProtectedContent(map.value("has_protected_content").toBool());
    setIsTranslatable(map.value("is_translatable").toBool());
    setIsMarkedAsUnread(map.value("is_marked_as_unread").toBool());
    setViewAsTopics(map.value("view_as_topics").toBool());
    setHasScheduledMessages(map.value("has_scheduled_messages").toBool());
    setCanBeDeletedOnlyForSelf(map.value("can_be_deleted_only_for_self").toBool());
    setCanBeDeletedForAllUsers(map.value("can_be_deleted_for_all_users").toBool());
    setCanBeReported(map.value("can_be_reported").toBool());
    setDefaultDisableNotification(map.value("default_disable_notification").toBool());
    setUnreadCount(map.value("unread_count").toInt());
    setLastReadInboxMessageId(map.value("last_read_inbox_message_id").toLongLong());
    setLastReadOutboxMessageId(map.value("last_read_outbox_message_id").toLongLong());
    setUnreadMentionCount(map.value("unread_mention_count").toInt());
    setUnreadReactionCount(map.value("unread_reaction_count").toInt());
    setNotificationSettings(map.value("notification_settings").toMap());
    setAvailableReactions(map.value("available_reactions").toMap());
    setMessageAutoDeleteTime(map.value("message_auto_delete_time").toInt());
    setEmojiStatus(map.value("emoji_status").toMap());
    setBackground(map.value("background").toMap());
    setThemeName(map.value("theme_name").toString());
    setActionBar(map.value("action_bar").toMap());
    setBusinessBotManageBar(map.value("business_bot_manage_bar").toMap());
    setVideoChat(map.value("video_chat").toMap());
    setPendingJoinRequests(map.value("pending_join_requests").toMap());
    setReplyMarkupMessageId(map.value("reply_markup_message_id").toLongLong());

    if (map.contains("draft_message"))
    {
        QVariantMap draftMessage = map.value("draft_message").toMap();
        if (!m_draftMessage)
        {
            m_draftMessage = new Message(this);
        }
        m_draftMessage->setFromVariantMap(draftMessage);
        emit draftMessageChanged();
    }

    setClientData(map.value("client_data").toString());
}
