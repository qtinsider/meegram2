#pragma once

#include <td/telegram/td_api.h>

#include <QVariant>

class BasicGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong id READ id NOTIFY basicGroupChanged)
    Q_PROPERTY(int memberCount READ memberCount NOTIFY basicGroupChanged)
    Q_PROPERTY(QVariantMap status READ status NOTIFY basicGroupChanged)
    Q_PROPERTY(bool isActive READ isActive NOTIFY basicGroupChanged)
    Q_PROPERTY(qlonglong upgradedToSupergroupId READ upgradedToSupergroupId NOTIFY basicGroupChanged)

public:
    explicit BasicGroup(td::td_api::object_ptr<td::td_api::basicGroup> group, QObject *parent = nullptr);

    qlonglong id() const;
    int memberCount() const;
    QVariantMap status() const;
    bool isActive() const;
    qlonglong upgradedToSupergroupId() const;

signals:
    void basicGroupChanged();

private:
    qlonglong m_id;
    int m_memberCount;
    QVariantMap m_status;
    bool m_isActive;
    qlonglong m_upgradedToSupergroupId;
};
