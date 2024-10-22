#include "BasicGroup.hpp"

BasicGroup::BasicGroup(td::td_api::object_ptr<td::td_api::basicGroup> group, QObject *parent)
    : QObject(parent)
    , m_id(group->id_)
    , m_memberCount(group->member_count_)
    , m_isActive(group->is_active_)
    , m_upgradedToSupergroupId(group->upgraded_to_supergroup_id_)
{
}

qlonglong BasicGroup::id() const
{
    return m_id;
}


int BasicGroup::memberCount() const
{
    return m_memberCount;
}

QVariantMap BasicGroup::status() const
{
    return m_status;
}

bool BasicGroup::isActive() const
{
    return m_isActive;
}

qlonglong BasicGroup::upgradedToSupergroupId() const
{
    return m_upgradedToSupergroupId;
}
