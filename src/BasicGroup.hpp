#pragma once

#include <td/telegram/td_api.h>

#include <QObject>

class BasicGroup : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong id READ id CONSTANT)
    Q_PROPERTY(int memberCount READ memberCount CONSTANT)
    Q_PROPERTY(Status status READ status CONSTANT)

public:
    explicit BasicGroup(td::td_api::object_ptr<td::td_api::basicGroup> group, QObject *parent = nullptr);

    enum Status { Creator, Administrator, Member, Restricted, Left, Banned };

    qlonglong id() const;
    int memberCount() const;
    Status status() const;

private:
    Status determineStatus(int statusId) const;

    qlonglong m_id;
    int m_memberCount;
    Status m_status;
};
