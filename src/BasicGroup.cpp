#include "BasicGroup.hpp"

BasicGroup::BasicGroup(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_memberCount(0)
    , m_isActive(false)
    , m_upgradedToSupergroupId(0)
{
}

qlonglong BasicGroup::id() const
{
    return m_id;
}

void BasicGroup::setId(qlonglong id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}

int BasicGroup::memberCount() const
{
    return m_memberCount;
}

void BasicGroup::setMemberCount(int memberCount)
{
    if (m_memberCount != memberCount)
    {
        m_memberCount = memberCount;
        emit memberCountChanged();
    }
}

QVariantMap BasicGroup::status() const
{
    return m_status;
}

void BasicGroup::setStatus(const QVariantMap &status)
{
    if (m_status != status)
    {
        m_status = status;
        emit statusChanged();
    }
}

bool BasicGroup::isActive() const
{
    return m_isActive;
}

void BasicGroup::setIsActive(bool isActive)
{
    if (m_isActive != isActive)
    {
        m_isActive = isActive;
        emit isActiveChanged();
    }
}

qlonglong BasicGroup::upgradedToSupergroupId() const
{
    return m_upgradedToSupergroupId;
}

void BasicGroup::setUpgradedToSupergroupId(qlonglong upgradedToSupergroupId)
{
    if (m_upgradedToSupergroupId != upgradedToSupergroupId)
    {
        m_upgradedToSupergroupId = upgradedToSupergroupId;
        emit upgradedToSupergroupIdChanged();
    }
}

void BasicGroup::setFromVariantMap(const QVariantMap &map)
{
    if (map.contains("id"))
    {
        setId(map.value("id").toLongLong());
    }
    if (map.contains("member_count"))
    {
        setMemberCount(map.value("member_count").toInt());
    }
    if (map.contains("status"))
    {
        setStatus(map.value("status").toMap());
    }
    if (map.contains("is_active"))
    {
        setIsActive(map.value("is_active").toBool());
    }
    if (map.contains("upgraded_to_supergroup_id"))
    {
        setUpgradedToSupergroupId(map.value("upgraded_to_supergroup_id").toLongLong());
    }
}
