#include "BasicGroupFullInfo.hpp"

BasicGroupFullInfo::BasicGroupFullInfo(td::td_api::object_ptr<td::td_api::basicGroupFullInfo> groupInfo, QObject *parent)
    : QObject(parent)
    , m_creatorUserId(0)
    , m_canHideMembers(false)
    , m_canToggleAggressiveAntiSpam(false)
{
    if (groupInfo->photo_)
    {
        m_photo = QVariantMap();
    }

    m_description = QString::fromStdString(groupInfo->description_);
    m_creatorUserId = groupInfo->creator_user_id_;
    m_canHideMembers = groupInfo->can_hide_members_;
    m_canToggleAggressiveAntiSpam = groupInfo->can_toggle_aggressive_anti_spam_;

    for (const auto &member : groupInfo->members_)
    {
        // m_members.append(QVariant::fromValue(member));
    }

    if (groupInfo->invite_link_)
    {
        m_inviteLink = QVariantMap();
    }

    m_botCommands = QVariantList();
}

QVariantMap BasicGroupFullInfo::photo() const
{
    return m_photo;
}

QString BasicGroupFullInfo::description() const
{
    return m_description;
}

qlonglong BasicGroupFullInfo::creatorUserId() const
{
    return m_creatorUserId;
}

QVariantList BasicGroupFullInfo::members() const
{
    return m_members;
}

bool BasicGroupFullInfo::canHideMembers() const
{
    return m_canHideMembers;
}

bool BasicGroupFullInfo::canToggleAggressiveAntiSpam() const
{
    return m_canToggleAggressiveAntiSpam;
}

QVariantMap BasicGroupFullInfo::inviteLink() const
{
    return m_inviteLink;
}

QVariantList BasicGroupFullInfo::botCommands() const
{
    return m_botCommands;
}
