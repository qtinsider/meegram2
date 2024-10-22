#include "BasicGroup.hpp"

BasicGroup::BasicGroup(td::td_api::object_ptr<td::td_api::basicGroup> group, QObject *parent)
    : QObject(parent)
    , m_id(group->id_)
    , m_memberCount(group->member_count_)
    , m_isActive(group->is_active_)
    , m_upgradedToSupergroupId(group->upgraded_to_supergroup_id_)
    , m_status(determineStatus(group->status_->get_id()))
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

BasicGroup::Status BasicGroup::status() const
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

BasicGroup::Status BasicGroup::determineStatus(int statusId) const
{
    switch (statusId)
    {
        case td::td_api::chatMemberStatusCreator::ID:
            return Status::Creator;
        case td::td_api::chatMemberStatusAdministrator::ID:
            return Status::Administrator;
        case td::td_api::chatMemberStatusMember::ID:
            return Status::Member;
        case td::td_api::chatMemberStatusRestricted::ID:
            return Status::Restricted;
        case td::td_api::chatMemberStatusLeft::ID:
            return Status::Left;
        case td::td_api::chatMemberStatusBanned::ID:
            return Status::Banned;
        default:
            return {};
    }
}
