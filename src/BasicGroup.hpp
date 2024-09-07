#pragma once

#include <QVariant>

class BasicGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(int memberCount READ memberCount WRITE setMemberCount NOTIFY memberCountChanged)
    Q_PROPERTY(QVariantMap status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(bool isActive READ isActive WRITE setIsActive NOTIFY isActiveChanged)
    Q_PROPERTY(
        qlonglong upgradedToSupergroupId READ upgradedToSupergroupId WRITE setUpgradedToSupergroupId NOTIFY upgradedToSupergroupIdChanged)

public:
    explicit BasicGroup(QObject *parent = nullptr);

    qlonglong id() const;
    void setId(qlonglong id);

    int memberCount() const;
    void setMemberCount(int memberCount);

    QVariantMap status() const;
    void setStatus(const QVariantMap &status);

    bool isActive() const;
    void setIsActive(bool isActive);

    qlonglong upgradedToSupergroupId() const;
    void setUpgradedToSupergroupId(qlonglong upgradedToSupergroupId);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void idChanged();
    void memberCountChanged();
    void statusChanged();
    void isActiveChanged();
    void upgradedToSupergroupIdChanged();

private:
    qlonglong m_id;
    int m_memberCount;
    QVariantMap m_status;
    bool m_isActive;
    qlonglong m_upgradedToSupergroupId;
};
