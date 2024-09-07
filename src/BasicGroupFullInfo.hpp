#pragma once

#include <QVariant>

class BasicGroupFullInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap photo READ photo WRITE setPhoto NOTIFY photoChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(qlonglong creatorUserId READ creatorUserId WRITE setCreatorUserId NOTIFY creatorUserIdChanged)
    Q_PROPERTY(QVariantList members READ members WRITE setMembers NOTIFY membersChanged)
    Q_PROPERTY(bool canHideMembers READ canHideMembers WRITE setCanHideMembers NOTIFY canHideMembersChanged)
    Q_PROPERTY(bool canToggleAggressiveAntiSpam READ canToggleAggressiveAntiSpam WRITE setCanToggleAggressiveAntiSpam NOTIFY
                   canToggleAggressiveAntiSpamChanged)
    Q_PROPERTY(QVariantMap inviteLink READ inviteLink WRITE setInviteLink NOTIFY inviteLinkChanged)
    Q_PROPERTY(QVariantList botCommands READ botCommands WRITE setBotCommands NOTIFY botCommandsChanged)

public:
    explicit BasicGroupFullInfo(QObject *parent = nullptr);

    QVariantMap photo() const;
    void setPhoto(const QVariantMap &photo);

    QString description() const;
    void setDescription(const QString &description);

    qlonglong creatorUserId() const;
    void setCreatorUserId(qlonglong creatorUserId);

    QVariantList members() const;
    void setMembers(const QVariantList &members);

    bool canHideMembers() const;
    void setCanHideMembers(bool canHideMembers);

    bool canToggleAggressiveAntiSpam() const;
    void setCanToggleAggressiveAntiSpam(bool canToggleAggressiveAntiSpam);

    QVariantMap inviteLink() const;
    void setInviteLink(const QVariantMap &inviteLink);

    QVariantList botCommands() const;
    void setBotCommands(const QVariantList &botCommands);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void photoChanged();
    void descriptionChanged();
    void creatorUserIdChanged();
    void membersChanged();
    void canHideMembersChanged();
    void canToggleAggressiveAntiSpamChanged();
    void inviteLinkChanged();
    void botCommandsChanged();

private:
    QVariantMap m_photo;
    QString m_description;
    qlonglong m_creatorUserId;
    QVariantList m_members;
    bool m_canHideMembers;
    bool m_canToggleAggressiveAntiSpam;
    QVariantMap m_inviteLink;
    QVariantList m_botCommands;
};
