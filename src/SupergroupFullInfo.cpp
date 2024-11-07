#include "SupergroupFullInfo.hpp"

SupergroupFullInfo::SupergroupFullInfo(td::td_api::object_ptr<td::td_api::supergroupFullInfo> groupFullInfo, QObject *parent)
    : QObject(parent)
{
    m_memberCount = groupFullInfo->member_count_;
}

int SupergroupFullInfo::memberCount() const
{
    return m_memberCount;
}
