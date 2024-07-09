#pragma once

#include <QObject>
#include <QVariant>

class BasicGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(qint32 memberCount READ memberCount WRITE setMemberCount NOTIFY memberCountChanged)
    Q_PROPERTY(QVariantMap status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)
    Q_PROPERTY(
        qint64 upgradedToSupergroupId READ upgradedToSupergroupId WRITE setUpgradedToSupergroupId NOTIFY upgradedToSupergroupIdChanged)

public:
    explicit BasicGroup(QObject *parent = nullptr);

    qint64 id() const;
    void setId(qint64 id);

    qint32 memberCount() const;
    void setMemberCount(qint32 memberCount);

    QVariantMap status() const;
    void setStatus(const QVariantMap &status);

    bool isActive() const;
    void setIsActive(bool isActive);

    qint64 upgradedToSupergroupId() const;
    void setUpgradedToSupergroupId(qint64 upgradedToSupergroupId);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void idChanged();
    void memberCountChanged();
    void statusChanged();
    void isActiveChanged();
    void upgradedToSupergroupIdChanged();

private:
    qint64 m_id;
    qint32 m_memberCount;
    QVariantMap m_status;
    bool m_isActive;
    qint64 m_upgradedToSupergroupId;
};
