#include "Message.hpp"

Message::Message(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_chatId(0)
    , m_isOutgoing(false)
    , m_isPinned(false)
    , m_isFromOffline(false)
    , m_canBeEdited(false)
    , m_canBeForwarded(false)
    , m_canBeRepliedInAnotherChat(false)
    , m_canBeSaved(false)
    , m_canBeDeletedOnlyForSelf(false)
    , m_canBeDeletedForAllUsers(false)
    , m_canGetAddedReactions(false)
    , m_canGetStatistics(false)
    , m_canGetMessageThread(false)
    , m_canGetReadDate(false)
    , m_canGetViewers(false)
    , m_canGetMediaTimestampLinks(false)
    , m_canReportReactions(false)
    , m_hasTimestampedMedia(false)
    , m_isChannelPost(false)
    , m_isTopicMessage(false)
    , m_containsUnreadMention(false)
    , m_date(0)
    , m_editDate(0)
    , m_messageThreadId(0)
    , m_savedMessagesTopicId(0)
    , m_selfDestructIn(0.0)
    , m_autoDeleteIn(0.0)
    , m_viaBotUserId(0)
    , m_senderBusinessBotUserId(0)
    , m_senderBoostCount(0)
    , m_authorSignature("")
    , m_mediaAlbumId(0)
    , m_effectId(0)
    , m_restrictionReason("")
{
}

qint64 Message::id() const
{
    return m_id;
}
void Message::setId(qint64 id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}

QVariantMap Message::senderId() const
{
    return m_senderId;
}
void Message::setSenderId(const QVariantMap &senderId)
{
    if (m_senderId != senderId)
    {
        m_senderId = senderId;
        emit senderIdChanged();
    }
}

qint64 Message::chatId() const
{
    return m_chatId;
}
void Message::setChatId(qint64 chatId)
{
    if (m_chatId != chatId)
    {
        m_chatId = chatId;
        emit chatIdChanged();
    }
}

QVariantMap Message::sendingState() const
{
    return m_sendingState;
}
void Message::setSendingState(const QVariantMap &sendingState)
{
    if (m_sendingState != sendingState)
    {
        m_sendingState = sendingState;
        emit sendingStateChanged();
    }
}

QVariantMap Message::schedulingState() const
{
    return m_schedulingState;
}
void Message::setSchedulingState(const QVariantMap &schedulingState)
{
    if (m_schedulingState != schedulingState)
    {
        m_schedulingState = schedulingState;
        emit schedulingStateChanged();
    }
}

bool Message::isOutgoing() const
{
    return m_isOutgoing;
}
void Message::setIsOutgoing(bool isOutgoing)
{
    if (m_isOutgoing != isOutgoing)
    {
        m_isOutgoing = isOutgoing;
        emit isOutgoingChanged();
    }
}

bool Message::isPinned() const
{
    return m_isPinned;
}
void Message::setIsPinned(bool isPinned)
{
    if (m_isPinned != isPinned)
    {
        m_isPinned = isPinned;
        emit isPinnedChanged();
    }
}

bool Message::isFromOffline() const
{
    return m_isFromOffline;
}
void Message::setIsFromOffline(bool isFromOffline)
{
    if (m_isFromOffline != isFromOffline)
    {
        m_isFromOffline = isFromOffline;
        emit isFromOfflineChanged();
    }
}

bool Message::canBeEdited() const
{
    return m_canBeEdited;
}
void Message::setCanBeEdited(bool canBeEdited)
{
    if (m_canBeEdited != canBeEdited)
    {
        m_canBeEdited = canBeEdited;
        emit canBeEditedChanged();
    }
}

bool Message::canBeForwarded() const
{
    return m_canBeForwarded;
}
void Message::setCanBeForwarded(bool canBeForwarded)
{
    if (m_canBeForwarded != canBeForwarded)
    {
        m_canBeForwarded = canBeForwarded;
        emit canBeForwardedChanged();
    }
}

bool Message::canBeRepliedInAnotherChat() const
{
    return m_canBeRepliedInAnotherChat;
}
void Message::setCanBeRepliedInAnotherChat(bool canBeRepliedInAnotherChat)
{
    if (m_canBeRepliedInAnotherChat != canBeRepliedInAnotherChat)
    {
        m_canBeRepliedInAnotherChat = canBeRepliedInAnotherChat;
        emit canBeRepliedInAnotherChatChanged();
    }
}

bool Message::canBeSaved() const
{
    return m_canBeSaved;
}
void Message::setCanBeSaved(bool canBeSaved)
{
    if (m_canBeSaved != canBeSaved)
    {
        m_canBeSaved = canBeSaved;
        emit canBeSavedChanged();
    }
}

bool Message::canBeDeletedOnlyForSelf() const
{
    return m_canBeDeletedOnlyForSelf;
}
void Message::setCanBeDeletedOnlyForSelf(bool canBeDeletedOnlyForSelf)
{
    if (m_canBeDeletedOnlyForSelf != canBeDeletedOnlyForSelf)
    {
        m_canBeDeletedOnlyForSelf = canBeDeletedOnlyForSelf;
        emit canBeDeletedOnlyForSelfChanged();
    }
}

bool Message::canBeDeletedForAllUsers() const
{
    return m_canBeDeletedForAllUsers;
}
void Message::setCanBeDeletedForAllUsers(bool canBeDeletedForAllUsers)
{
    if (m_canBeDeletedForAllUsers != canBeDeletedForAllUsers)
    {
        m_canBeDeletedForAllUsers = canBeDeletedForAllUsers;
        emit canBeDeletedForAllUsersChanged();
    }
}

bool Message::canGetAddedReactions() const
{
    return m_canGetAddedReactions;
}
void Message::setCanGetAddedReactions(bool canGetAddedReactions)
{
    if (m_canGetAddedReactions != canGetAddedReactions)
    {
        m_canGetAddedReactions = canGetAddedReactions;
        emit canGetAddedReactionsChanged();
    }
}

bool Message::canGetStatistics() const
{
    return m_canGetStatistics;
}
void Message::setCanGetStatistics(bool canGetStatistics)
{
    if (m_canGetStatistics != canGetStatistics)
    {
        m_canGetStatistics = canGetStatistics;
        emit canGetStatisticsChanged();
    }
}

bool Message::canGetMessageThread() const
{
    return m_canGetMessageThread;
}
void Message::setCanGetMessageThread(bool canGetMessageThread)
{
    if (m_canGetMessageThread != canGetMessageThread)
    {
        m_canGetMessageThread = canGetMessageThread;
        emit canGetMessageThreadChanged();
    }
}

bool Message::canGetReadDate() const
{
    return m_canGetReadDate;
}
void Message::setCanGetReadDate(bool canGetReadDate)
{
    if (m_canGetReadDate != canGetReadDate)
    {
        m_canGetReadDate = canGetReadDate;
        emit canGetReadDateChanged();
    }
}

bool Message::canGetViewers() const
{
    return m_canGetViewers;
}
void Message::setCanGetViewers(bool canGetViewers)
{
    if (m_canGetViewers != canGetViewers)
    {
        m_canGetViewers = canGetViewers;
        emit canGetViewersChanged();
    }
}

bool Message::canGetMediaTimestampLinks() const
{
    return m_canGetMediaTimestampLinks;
}
void Message::setCanGetMediaTimestampLinks(bool canGetMediaTimestampLinks)
{
    if (m_canGetMediaTimestampLinks != canGetMediaTimestampLinks)
    {
        m_canGetMediaTimestampLinks = canGetMediaTimestampLinks;
        emit canGetMediaTimestampLinksChanged();
    }
}

bool Message::canReportReactions() const
{
    return m_canReportReactions;
}
void Message::setCanReportReactions(bool canReportReactions)
{
    if (m_canReportReactions != canReportReactions)
    {
        m_canReportReactions = canReportReactions;
        emit canReportReactionsChanged();
    }
}

bool Message::hasTimestampedMedia() const
{
    return m_hasTimestampedMedia;
}
void Message::setHasTimestampedMedia(bool hasTimestampedMedia)
{
    if (m_hasTimestampedMedia != hasTimestampedMedia)
    {
        m_hasTimestampedMedia = hasTimestampedMedia;
        emit hasTimestampedMediaChanged();
    }
}

bool Message::isChannelPost() const
{
    return m_isChannelPost;
}
void Message::setIsChannelPost(bool isChannelPost)
{
    if (m_isChannelPost != isChannelPost)
    {
        m_isChannelPost = isChannelPost;
        emit isChannelPostChanged();
    }
}

bool Message::isTopicMessage() const
{
    return m_isTopicMessage;
}
void Message::setIsTopicMessage(bool isTopicMessage)
{
    if (m_isTopicMessage != isTopicMessage)
    {
        m_isTopicMessage = isTopicMessage;
        emit isTopicMessageChanged();
    }
}

bool Message::containsUnreadMention() const
{
    return m_containsUnreadMention;
}
void Message::setContainsUnreadMention(bool containsUnreadMention)
{
    if (m_containsUnreadMention != containsUnreadMention)
    {
        m_containsUnreadMention = containsUnreadMention;
        emit containsUnreadMentionChanged();
    }
}

qint64 Message::date() const
{
    return m_date;
}
void Message::setDate(qint64 date)
{
    if (m_date != date)
    {
        m_date = date;
        emit dateChanged();
    }
}

int Message::editDate() const
{
    return m_editDate;
}
void Message::setEditDate(int editDate)
{
    if (m_editDate != editDate)
    {
        m_editDate = editDate;
        emit editDateChanged();
    }
}

QVariantMap Message::forwardInfo() const
{
    return m_forwardInfo;
}
void Message::setForwardInfo(const QVariantMap &forwardInfo)
{
    if (m_forwardInfo != forwardInfo)
    {
        m_forwardInfo = forwardInfo;
        emit forwardInfoChanged();
    }
}

QVariantMap Message::importInfo() const
{
    return m_importInfo;
}
void Message::setImportInfo(const QVariantMap &importInfo)
{
    if (m_importInfo != importInfo)
    {
        m_importInfo = importInfo;
        emit importInfoChanged();
    }
}

QVariantMap Message::interactionInfo() const
{
    return m_interactionInfo;
}
void Message::setInteractionInfo(const QVariantMap &interactionInfo)
{
    if (m_interactionInfo != interactionInfo)
    {
        m_interactionInfo = interactionInfo;
        emit interactionInfoChanged();
    }
}

QVariantList Message::unreadReactions() const
{
    return m_unreadReactions;
}
void Message::setUnreadReactions(const QVariantList &unreadReactions)
{
    if (m_unreadReactions != unreadReactions)
    {
        m_unreadReactions = unreadReactions;
        emit unreadReactionsChanged();
    }
}

QVariantMap Message::factCheck() const
{
    return m_factCheck;
}
void Message::setFactCheck(const QVariantMap &factCheck)
{
    if (m_factCheck != factCheck)
    {
        m_factCheck = factCheck;
        emit factCheckChanged();
    }
}

QVariantMap Message::replyTo() const
{
    return m_replyTo;
}
void Message::setReplyTo(const QVariantMap &replyTo)
{
    if (m_replyTo != replyTo)
    {
        m_replyTo = replyTo;
        emit replyToChanged();
    }
}

qint64 Message::messageThreadId() const
{
    return m_messageThreadId;
}
void Message::setMessageThreadId(qint64 messageThreadId)
{
    if (m_messageThreadId != messageThreadId)
    {
        m_messageThreadId = messageThreadId;
        emit messageThreadIdChanged();
    }
}

qint64 Message::savedMessagesTopicId() const
{
    return m_savedMessagesTopicId;
}
void Message::setSavedMessagesTopicId(qint64 savedMessagesTopicId)
{
    if (m_savedMessagesTopicId != savedMessagesTopicId)
    {
        m_savedMessagesTopicId = savedMessagesTopicId;
        emit savedMessagesTopicIdChanged();
    }
}

QVariantMap Message::selfDestructType() const
{
    return m_selfDestructType;
}
void Message::setSelfDestructType(const QVariantMap &selfDestructType)
{
    if (m_selfDestructType != selfDestructType)
    {
        m_selfDestructType = selfDestructType;
        emit selfDestructTypeChanged();
    }
}

double Message::selfDestructIn() const
{
    return m_selfDestructIn;
}
void Message::setSelfDestructIn(double selfDestructIn)
{
    if (m_selfDestructIn != selfDestructIn)
    {
        m_selfDestructIn = selfDestructIn;
        emit selfDestructInChanged();
    }
}

double Message::autoDeleteIn() const
{
    return m_autoDeleteIn;
}
void Message::setAutoDeleteIn(double autoDeleteIn)
{
    if (m_autoDeleteIn != autoDeleteIn)
    {
        m_autoDeleteIn = autoDeleteIn;
        emit autoDeleteInChanged();
    }
}

qint64 Message::viaBotUserId() const
{
    return m_viaBotUserId;
}
void Message::setViaBotUserId(qint64 viaBotUserId)
{
    if (m_viaBotUserId != viaBotUserId)
    {
        m_viaBotUserId = viaBotUserId;
        emit viaBotUserIdChanged();
    }
}

qint64 Message::senderBusinessBotUserId() const
{
    return m_senderBusinessBotUserId;
}
void Message::setSenderBusinessBotUserId(qint64 senderBusinessBotUserId)
{
    if (m_senderBusinessBotUserId != senderBusinessBotUserId)
    {
        m_senderBusinessBotUserId = senderBusinessBotUserId;
        emit senderBusinessBotUserIdChanged();
    }
}

int Message::senderBoostCount() const
{
    return m_senderBoostCount;
}
void Message::setSenderBoostCount(int senderBoostCount)
{
    if (m_senderBoostCount != senderBoostCount)
    {
        m_senderBoostCount = senderBoostCount;
        emit senderBoostCountChanged();
    }
}

QString Message::authorSignature() const
{
    return m_authorSignature;
}
void Message::setAuthorSignature(const QString &authorSignature)
{
    if (m_authorSignature != authorSignature)
    {
        m_authorSignature = authorSignature;
        emit authorSignatureChanged();
    }
}

qint64 Message::mediaAlbumId() const
{
    return m_mediaAlbumId;
}
void Message::setMediaAlbumId(qint64 mediaAlbumId)
{
    if (m_mediaAlbumId != mediaAlbumId)
    {
        m_mediaAlbumId = mediaAlbumId;
        emit mediaAlbumIdChanged();
    }
}

qint64 Message::effectId() const
{
    return m_effectId;
}
void Message::setEffectId(qint64 effectId)
{
    if (m_effectId != effectId)
    {
        m_effectId = effectId;
        emit effectIdChanged();
    }
}

QString Message::restrictionReason() const
{
    return m_restrictionReason;
}
void Message::setRestrictionReason(const QString &restrictionReason)
{
    if (m_restrictionReason != restrictionReason)
    {
        m_restrictionReason = restrictionReason;
        emit restrictionReasonChanged();
    }
}

QVariantMap Message::content() const
{
    return m_content;
}
void Message::setContent(const QVariantMap &content)
{
    if (m_content != content)
    {
        m_content = content;
        emit contentChanged();
    }
}

QVariantMap Message::replyMarkup() const
{
    return m_replyMarkup;
}
void Message::setReplyMarkup(const QVariantMap &replyMarkup)
{
    if (m_replyMarkup != replyMarkup)
    {
        m_replyMarkup = replyMarkup;
        emit replyMarkupChanged();
    }
}

void Message::setFromVariantMap(const QVariantMap &map)
{
    if (map.contains("id"))
        setId(map["id"].toLongLong());
    if (map.contains("sender_id"))
        setSenderId(map["sender_id"].toMap());
    if (map.contains("chat_id"))
        setChatId(map["chat_id"].toLongLong());
    if (map.contains("sending_state"))
        setSendingState(map["sending_state"].toMap());
    if (map.contains("scheduling_state"))
        setSchedulingState(map["scheduling_state"].toMap());
    if (map.contains("is_outgoing"))
        setIsOutgoing(map["is_outgoing"].toBool());
    if (map.contains("is_pinned"))
        setIsPinned(map["is_pinned"].toBool());
    if (map.contains("is_from_offline"))
        setIsFromOffline(map["is_from_offline"].toBool());
    if (map.contains("can_be_edited"))
        setCanBeEdited(map["can_be_edited"].toBool());
    if (map.contains("can_be_forwarded"))
        setCanBeForwarded(map["can_be_forwarded"].toBool());
    if (map.contains("can_be_replied_in_another_chat"))
        setCanBeRepliedInAnotherChat(map["can_be_replied_in_another_chat"].toBool());
    if (map.contains("can_be_saved"))
        setCanBeSaved(map["can_be_saved"].toBool());
    if (map.contains("can_be_deleted_only_for_self"))
        setCanBeDeletedOnlyForSelf(map["can_be_deleted_only_for_self"].toBool());
    if (map.contains("can_be_deleted_for_all_users"))
        setCanBeDeletedForAllUsers(map["can_be_deleted_for_all_users"].toBool());
    if (map.contains("can_get_added_reactions"))
        setCanGetAddedReactions(map["can_get_added_reactions"].toBool());
    if (map.contains("can_get_statistics"))
        setCanGetStatistics(map["can_get_statistics"].toBool());
    if (map.contains("can_get_message_thread"))
        setCanGetMessageThread(map["can_get_message_thread"].toBool());
    if (map.contains("can_get_read_date"))
        setCanGetReadDate(map["can_get_read_date"].toBool());
    if (map.contains("can_get_viewers"))
        setCanGetViewers(map["can_get_viewers"].toBool());
    if (map.contains("can_get_media_timestamp_links"))
        setCanGetMediaTimestampLinks(map["can_get_media_timestamp_links"].toBool());
    if (map.contains("can_report_reactions"))
        setCanReportReactions(map["can_report_reactions"].toBool());
    if (map.contains("has_timestamped_media"))
        setHasTimestampedMedia(map["has_timestamped_media"].toBool());
    if (map.contains("is_channel_post"))
        setIsChannelPost(map["is_channel_post"].toBool());
    if (map.contains("is_topic_message"))
        setIsTopicMessage(map["is_topic_message"].toBool());
    if (map.contains("contains_unread_mention"))
        setContainsUnreadMention(map["contains_unread_mention"].toBool());
    if (map.contains("date"))
        setDate(map["date"].toLongLong());
    if (map.contains("edit_date"))
        setEditDate(map["edit_date"].toLongLong());
    if (map.contains("forward_info"))
        setForwardInfo(map["forward_info"].toMap());
    if (map.contains("import_info"))
        setImportInfo(map["import_info"].toMap());
    if (map.contains("interaction_info"))
        setInteractionInfo(map["interaction_info"].toMap());
    if (map.contains("unread_reactions"))
        setUnreadReactions(map["unread_reactions"].toList());
    if (map.contains("fact_check"))
        setFactCheck(map["fact_check"].toMap());
    if (map.contains("reply_to"))
        setReplyTo(map["reply_to"].toMap());
    if (map.contains("message_thread_id"))
        setMessageThreadId(map["message_thread_id"].toLongLong());
    if (map.contains("saved_messages_topic_id"))
        setSavedMessagesTopicId(map["saved_messages_topic_id"].toLongLong());
    if (map.contains("self_destruct_type"))
        setSelfDestructType(map["self_destruct_type"].toMap());
    if (map.contains("self_destruct_in"))
        setSelfDestructIn(map["self_destruct_in"].toDouble());
    if (map.contains("auto_delete_in"))
        setAutoDeleteIn(map["auto_delete_in"].toDouble());
    if (map.contains("via_bot_user_id"))
        setViaBotUserId(map["via_bot_user_id"].toLongLong());
    if (map.contains("sender_business_bot_user_id"))
        setSenderBusinessBotUserId(map["sender_business_bot_user_id"].toLongLong());
    if (map.contains("sender_boost_count"))
        setSenderBoostCount(map["sender_boost_count"].toInt());
    if (map.contains("author_signature"))
        setAuthorSignature(map["author_signature"].toString());
    if (map.contains("media_album_id"))
        setMediaAlbumId(map["media_album_id"].toLongLong());
    if (map.contains("effect_id"))
        setEffectId(map["effect_id"].toLongLong());
    if (map.contains("restriction_reason"))
        setRestrictionReason(map["restriction_reason"].toString());
    if (map.contains("content"))
        setContent(map["content"].toMap());
    if (map.contains("reply_markup"))
        setReplyMarkup(map["reply_markup"].toMap());
}
