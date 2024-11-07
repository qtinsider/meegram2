#include "Supergroup.hpp"

Supergroup::Supergroup(td::td_api::object_ptr<td::td_api::supergroup> group, QObject *parent)
    : QObject(parent)
    , m_id(group->id_)
    , m_status(determineStatus(group->status_->get_id()))
    , m_memberCount(group->member_count_)
    , m_hasLocation(group->has_location_)
    , m_isChannel(group->is_channel_)
{
    if (group->usernames_)
    {
        for (auto &activeUsername : group->usernames_->active_usernames_)
        {
            m_activeUsernames.append(QString::fromStdString(activeUsername));
        }
    }
}

qlonglong Supergroup::id() const
{
    return m_id;
}

Supergroup::Status Supergroup::status() const
{
    return m_status;
}

int Supergroup::memberCount() const
{
    return m_memberCount;
}

bool Supergroup::hasLocation() const
{
    return m_hasLocation;
}

bool Supergroup::isChannel() const
{
    return m_isChannel;
}

QStringList Supergroup::activeUsernames() const
{
    return m_activeUsernames;
}

Supergroup::Status Supergroup::determineStatus(int statusId) const
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
