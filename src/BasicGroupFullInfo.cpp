#include "BasicGroupFullInfo.hpp"

BasicGroupFullInfo::BasicGroupFullInfo(QObject *parent)
    : QObject(parent)
    , m_creatorUserId(0)
    , m_canHideMembers(false)
    , m_canToggleAggressiveAntiSpam(false)
{
}

QVariantMap BasicGroupFullInfo::photo() const
{
    return m_photo;
}

void BasicGroupFullInfo::setPhoto(const QVariantMap &photo)
{
    if (m_photo != photo)
    {
        m_photo = photo;
        emit photoChanged();
    }
}

QString BasicGroupFullInfo::description() const
{
    return m_description;
}

void BasicGroupFullInfo::setDescription(const QString &description)
{
    if (m_description != description)
    {
        m_description = description;
        emit descriptionChanged();
    }
}

qint64 BasicGroupFullInfo::creatorUserId() const
{
    return m_creatorUserId;
}

void BasicGroupFullInfo::setCreatorUserId(qint64 creatorUserId)
{
    if (m_creatorUserId != creatorUserId)
    {
        m_creatorUserId = creatorUserId;
        emit creatorUserIdChanged();
    }
}

QVariantList BasicGroupFullInfo::members() const
{
    return m_members;
}

void BasicGroupFullInfo::setMembers(const QVariantList &members)
{
    if (m_members != members)
    {
        m_members = members;
        emit membersChanged();
    }
}

bool BasicGroupFullInfo::canHideMembers() const
{
    return m_canHideMembers;
}

void BasicGroupFullInfo::setCanHideMembers(bool canHideMembers)
{
    if (m_canHideMembers != canHideMembers)
    {
        m_canHideMembers = canHideMembers;
        emit canHideMembersChanged();
    }
}

bool BasicGroupFullInfo::canToggleAggressiveAntiSpam() const
{
    return m_canToggleAggressiveAntiSpam;
}

void BasicGroupFullInfo::setCanToggleAggressiveAntiSpam(bool canToggleAggressiveAntiSpam)
{
    if (m_canToggleAggressiveAntiSpam != canToggleAggressiveAntiSpam)
    {
        m_canToggleAggressiveAntiSpam = canToggleAggressiveAntiSpam;
        emit canToggleAggressiveAntiSpamChanged();
    }
}

QVariantMap BasicGroupFullInfo::inviteLink() const
{
    return m_inviteLink;
}

void BasicGroupFullInfo::setInviteLink(const QVariantMap &inviteLink)
{
    if (m_inviteLink != inviteLink)
    {
        m_inviteLink = inviteLink;
        emit inviteLinkChanged();
    }
}

QVariantList BasicGroupFullInfo::botCommands() const
{
    return m_botCommands;
}

void BasicGroupFullInfo::setBotCommands(const QVariantList &botCommands)
{
    if (m_botCommands != botCommands)
    {
        m_botCommands = botCommands;
        emit botCommandsChanged();
    }
}

void BasicGroupFullInfo::setFromVariantMap(const QVariantMap &map)
{
    if (map.contains("photo"))
    {
        setPhoto(map.value("photo").toMap());
    }
    if (map.contains("description"))
    {
        setDescription(map.value("description").toString());
    }
    if (map.contains("creator_user_id"))
    {
        setCreatorUserId(map.value("creator_user_id").toLongLong());
    }
    if (map.contains("members"))
    {
        setMembers(map.value("members").toList());
    }
    if (map.contains("can_hide_members"))
    {
        setCanHideMembers(map.value("can_hide_members").toBool());
    }
    if (map.contains("can_toggle_aggressive_anti_spam"))
    {
        setCanToggleAggressiveAntiSpam(map.value("can_toggle_aggressive_anti_spam").toBool());
    }
    if (map.contains("invite_link"))
    {
        setInviteLink(map.value("invite_link").toMap());
    }
    if (map.contains("bot_commands"))
    {
        setBotCommands(map.value("bot_commands").toList());
    }
}
