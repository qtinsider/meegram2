#pragma once

#include <td/telegram/td_api.h>

#include <QObject>

class SupergroupFullInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int memberCount READ memberCount CONSTANT)

public:
    explicit SupergroupFullInfo(td::td_api::object_ptr<td::td_api::supergroupFullInfo> groupFullInfo, QObject *parent = nullptr);

    int memberCount() const;

private:
    int m_memberCount;
};
