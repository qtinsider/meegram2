#pragma once

#include <td/telegram/td_api.h>

#include <QObject>
#include <QVariant>

class BasicGroupFullInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap photo READ photo NOTIFY basicGroupFullInfoChanged)
    Q_PROPERTY(QString description READ description NOTIFY basicGroupFullInfoChanged)
    Q_PROPERTY(qlonglong creatorUserId READ creatorUserId NOTIFY basicGroupFullInfoChanged)
    Q_PROPERTY(QVariantList members READ members NOTIFY basicGroupFullInfoChanged)
    Q_PROPERTY(bool canHideMembers READ canHideMembers NOTIFY basicGroupFullInfoChanged)
    Q_PROPERTY(bool canToggleAggressiveAntiSpam READ canToggleAggressiveAntiSpam NOTIFY basicGroupFullInfoChanged)
    Q_PROPERTY(QVariantMap inviteLink READ inviteLink NOTIFY basicGroupFullInfoChanged)
    Q_PROPERTY(QVariantList botCommands READ botCommands NOTIFY basicGroupFullInfoChanged)

public:
    explicit BasicGroupFullInfo(td::td_api::object_ptr<td::td_api::basicGroupFullInfo> groupInfo, QObject *parent = nullptr);

    QVariantMap photo() const;
    QString description() const;
    qlonglong creatorUserId() const;
    QVariantList members() const;
    bool canHideMembers() const;
    bool canToggleAggressiveAntiSpam() const;
    QVariantMap inviteLink() const;
    QVariantList botCommands() const;

signals:
    void basicGroupFullInfoChanged();

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
