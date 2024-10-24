#include "Supergroup.hpp"

Supergroup::Supergroup(td::td_api::object_ptr<td::td_api::supergroup> group, QObject *parent)
    : QObject(parent)
    , m_id(group->id_)
    , m_date(group->date_)
    , m_memberCount(group->member_count_)
    , m_boostLevel(group->boost_level_)
    // , m_hasLinkedChat(group->linked_chat_id_ != 0)
    // , m_hasLocation(group->location_ != nullptr)
    , m_signMessages(group->sign_messages_)
    , m_joinToSendMessages(group->join_to_send_messages_)
    , m_joinByRequest(group->join_by_request_)
    , m_isSlowModeEnabled(group->is_slow_mode_enabled_)
    , m_isChannel(group->is_channel_)
    , m_isBroadcastGroup(group->is_broadcast_group_)
    , m_isForum(group->is_forum_)
    , m_isVerified(group->is_verified_)
    , m_restrictionReason(QString::fromStdString(group->restriction_reason_))
    , m_isScam(group->is_scam_)
    , m_isFake(group->is_fake_)
    , m_hasActiveStories(group->has_active_stories_)
    , m_hasUnreadActiveStories(group->has_unread_active_stories_)
{
    m_usernames = QVariantMap();

    m_status = QVariantMap();
}

qlonglong Supergroup::id() const
{
    return m_id;
}

QVariantMap Supergroup::usernames() const
{
    return m_usernames;
}

int Supergroup::date() const
{
    return m_date;
}

QVariantMap Supergroup::status() const
{
    return m_status;
}

int Supergroup::memberCount() const
{
    return m_memberCount;
}

int Supergroup::boostLevel() const
{
    return m_boostLevel;
}

bool Supergroup::hasLinkedChat() const
{
    return m_hasLinkedChat;
}

bool Supergroup::hasLocation() const
{
    return m_hasLocation;
}

bool Supergroup::signMessages() const
{
    return m_signMessages;
}

bool Supergroup::joinToSendMessages() const
{
    return m_joinToSendMessages;
}

bool Supergroup::joinByRequest() const
{
    return m_joinByRequest;
}

bool Supergroup::isSlowModeEnabled() const
{
    return m_isSlowModeEnabled;
}

bool Supergroup::isChannel() const
{
    return m_isChannel;
}

bool Supergroup::isBroadcastGroup() const
{
    return m_isBroadcastGroup;
}

bool Supergroup::isForum() const
{
    return m_isForum;
}

bool Supergroup::isVerified() const
{
    return m_isVerified;
}

QString Supergroup::restrictionReason() const
{
    return m_restrictionReason;
}

bool Supergroup::isScam() const
{
    return m_isScam;
}

bool Supergroup::isFake() const
{
    return m_isFake;
}

bool Supergroup::hasActiveStories() const
{
    return m_hasActiveStories;
}

bool Supergroup::hasUnreadActiveStories() const
{
    return m_hasUnreadActiveStories;
}
