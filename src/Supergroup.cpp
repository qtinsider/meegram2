#include "Supergroup.hpp"

Supergroup::Supergroup(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_date(0)
    , m_memberCount(0)
    , m_boostLevel(0)
    , m_hasLinkedChat(false)
    , m_hasLocation(false)
    , m_signMessages(false)
    , m_joinToSendMessages(false)
    , m_joinByRequest(false)
    , m_isSlowModeEnabled(false)
    , m_isChannel(false)
    , m_isBroadcastGroup(false)
    , m_isForum(false)
    , m_isVerified(false)
    , m_restrictionReason("")
    , m_isScam(false)
    , m_isFake(false)
    , m_hasActiveStories(false)
    , m_hasUnreadActiveStories(false)
{
}

qlonglong Supergroup::id() const
{
    return m_id;
}

void Supergroup::setId(qlonglong id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}

QVariantMap Supergroup::usernames() const
{
    return m_usernames;
}

void Supergroup::setUsernames(const QVariantMap &usernames)
{
    if (m_usernames != usernames)
    {
        m_usernames = usernames;
        emit usernamesChanged();
    }
}

int Supergroup::date() const
{
    return m_date;
}

void Supergroup::setDate(int date)
{
    if (m_date != date)
    {
        m_date = date;
        emit dateChanged();
    }
}

QVariantMap Supergroup::status() const
{
    return m_status;
}

void Supergroup::setStatus(const QVariantMap &status)
{
    if (m_status != status)
    {
        m_status = status;
        emit statusChanged();
    }
}

int Supergroup::memberCount() const
{
    return m_memberCount;
}

void Supergroup::setMemberCount(int memberCount)
{
    if (m_memberCount != memberCount)
    {
        m_memberCount = memberCount;
        emit memberCountChanged();
    }
}

int Supergroup::boostLevel() const
{
    return m_boostLevel;
}

void Supergroup::setBoostLevel(int boostLevel)
{
    if (m_boostLevel != boostLevel)
    {
        m_boostLevel = boostLevel;
        emit boostLevelChanged();
    }
}

bool Supergroup::hasLinkedChat() const
{
    return m_hasLinkedChat;
}

void Supergroup::setHasLinkedChat(bool hasLinkedChat)
{
    if (m_hasLinkedChat != hasLinkedChat)
    {
        m_hasLinkedChat = hasLinkedChat;
        emit hasLinkedChatChanged();
    }
}

bool Supergroup::hasLocation() const
{
    return m_hasLocation;
}

void Supergroup::setHasLocation(bool hasLocation)
{
    if (m_hasLocation != hasLocation)
    {
        m_hasLocation = hasLocation;
        emit hasLocationChanged();
    }
}

bool Supergroup::signMessages() const
{
    return m_signMessages;
}

void Supergroup::setSignMessages(bool signMessages)
{
    if (m_signMessages != signMessages)
    {
        m_signMessages = signMessages;
        emit signMessagesChanged();
    }
}

bool Supergroup::joinToSendMessages() const
{
    return m_joinToSendMessages;
}

void Supergroup::setJoinToSendMessages(bool joinToSendMessages)
{
    if (m_joinToSendMessages != joinToSendMessages)
    {
        m_joinToSendMessages = joinToSendMessages;
        emit joinToSendMessagesChanged();
    }
}

bool Supergroup::joinByRequest() const
{
    return m_joinByRequest;
}

void Supergroup::setJoinByRequest(bool joinByRequest)
{
    if (m_joinByRequest != joinByRequest)
    {
        m_joinByRequest = joinByRequest;
        emit joinByRequestChanged();
    }
}

bool Supergroup::isSlowModeEnabled() const
{
    return m_isSlowModeEnabled;
}

void Supergroup::setIsSlowModeEnabled(bool isSlowModeEnabled)
{
    if (m_isSlowModeEnabled != isSlowModeEnabled)
    {
        m_isSlowModeEnabled = isSlowModeEnabled;
        emit isSlowModeEnabledChanged();
    }
}

bool Supergroup::isChannel() const
{
    return m_isChannel;
}

void Supergroup::setIsChannel(bool isChannel)
{
    if (m_isChannel != isChannel)
    {
        m_isChannel = isChannel;
        emit isChannelChanged();
    }
}

bool Supergroup::isBroadcastGroup() const
{
    return m_isBroadcastGroup;
}

void Supergroup::setIsBroadcastGroup(bool isBroadcastGroup)
{
    if (m_isBroadcastGroup != isBroadcastGroup)
    {
        m_isBroadcastGroup = isBroadcastGroup;
        emit isBroadcastGroupChanged();
    }
}

bool Supergroup::isForum() const
{
    return m_isForum;
}

void Supergroup::setIsForum(bool isForum)
{
    if (m_isForum != isForum)
    {
        m_isForum = isForum;
        emit isForumChanged();
    }
}

bool Supergroup::isVerified() const
{
    return m_isVerified;
}

void Supergroup::setIsVerified(bool isVerified)
{
    if (m_isVerified != isVerified)
    {
        m_isVerified = isVerified;
        emit isVerifiedChanged();
    }
}

QString Supergroup::restrictionReason() const
{
    return m_restrictionReason;
}

void Supergroup::setRestrictionReason(const QString &restrictionReason)
{
    if (m_restrictionReason != restrictionReason)
    {
        m_restrictionReason = restrictionReason;
        emit restrictionReasonChanged();
    }
}

bool Supergroup::isScam() const
{
    return m_isScam;
}

void Supergroup::setIsScam(bool isScam)
{
    if (m_isScam != isScam)
    {
        m_isScam = isScam;
        emit isScamChanged();
    }
}

bool Supergroup::isFake() const
{
    return m_isFake;
}

void Supergroup::setIsFake(bool isFake)
{
    if (m_isFake != isFake)
    {
        m_isFake = isFake;
        emit isFakeChanged();
    }
}

bool Supergroup::hasActiveStories() const
{
    return m_hasActiveStories;
}

void Supergroup::setHasActiveStories(bool hasActiveStories)
{
    if (m_hasActiveStories != hasActiveStories)
    {
        m_hasActiveStories = hasActiveStories;
        emit hasActiveStoriesChanged();
    }
}

bool Supergroup::hasUnreadActiveStories() const
{
    return m_hasUnreadActiveStories;
}

void Supergroup::setHasUnreadActiveStories(bool hasUnreadActiveStories)
{
    if (m_hasUnreadActiveStories != hasUnreadActiveStories)
    {
        m_hasUnreadActiveStories = hasUnreadActiveStories;
        emit hasUnreadActiveStoriesChanged();
    }
}

void Supergroup::setFromVariantMap(const QVariantMap &map)
{
    if (map.contains("id"))
        setId(map["id"].toLongLong());
    if (map.contains("usernames"))
        setUsernames(map["usernames"].toMap());
    if (map.contains("date"))
        setDate(map["date"].toInt());
    if (map.contains("status"))
        setStatus(map["status"].toMap());
    if (map.contains("member_count"))
        setMemberCount(map["member_count"].toInt());
    if (map.contains("boost_level"))
        setBoostLevel(map["boost_level"].toInt());
    if (map.contains("has_linked_chat"))
        setHasLinkedChat(map["has_linked_chat"].toBool());
    if (map.contains("has_location"))
        setHasLocation(map["has_location"].toBool());
    if (map.contains("sign_messages"))
        setSignMessages(map["sign_messages"].toBool());
    if (map.contains("join_to_send_messages"))
        setJoinToSendMessages(map["join_to_send_messages"].toBool());
    if (map.contains("join_by_request"))
        setJoinByRequest(map["join_by_request"].toBool());
    if (map.contains("is_slow_mode_enabled"))
        setIsSlowModeEnabled(map["is_slow_mode_enabled"].toBool());
    if (map.contains("is_channel"))
        setIsChannel(map["is_channel"].toBool());
    if (map.contains("is_broadcast_group"))
        setIsBroadcastGroup(map["is_broadcast_group"].toBool());
    if (map.contains("is_forum"))
        setIsForum(map["is_forum"].toBool());
    if (map.contains("is_verified"))
        setIsVerified(map["is_verified"].toBool());
    if (map.contains("restriction_reason"))
        setRestrictionReason(map["restriction_reason"].toString());
    if (map.contains("is_scam"))
        setIsScam(map["is_scam"].toBool());
    if (map.contains("is_fake"))
        setIsFake(map["is_fake"].toBool());
    if (map.contains("has_active_stories"))
        setHasActiveStories(map["has_active_stories"].toBool());
    if (map.contains("has_unread_active_stories"))
        setHasUnreadActiveStories(map["has_unread_active_stories"].toBool());
}
