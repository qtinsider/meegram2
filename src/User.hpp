#pragma once

#include <td/telegram/td_api.h>

#include <QDateTime>
#include <QObject>
#include <QStringList>

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong id READ id CONSTANT)
    Q_PROPERTY(QString firstName READ firstName CONSTANT)
    Q_PROPERTY(QString lastName READ lastName CONSTANT)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool isSupport READ isSupport CONSTANT)
    Q_PROPERTY(Type type READ type CONSTANT)

    Q_ENUMS(Type)
public:
    explicit User(td::td_api::object_ptr<td::td_api::user> user, QObject *parent = nullptr);

    enum Status { Empty, Online, Offline, Recently, LastWeek, LastMonth };
    enum Type { Bot, Regular, Deleted, Unknown };

    qlonglong id() const;
    QString firstName() const;
    QString lastName() const;
    Status status() const;
    bool isSupport() const;
    Type type() const;

    QDateTime wasOnline() const;
    QStringList activeUsernames() const;

    void setStatus(td::td_api::object_ptr<td::td_api::UserStatus> status);

signals:
    void statusChanged();

private:
    Type determineType(const td::td_api::object_ptr<td::td_api::UserType> &type) const;

    qlonglong m_id;
    QString m_firstName;
    QString m_lastName;
    Status m_status;
    bool m_isSupport;
    Type m_type;

    QDateTime m_wasOnline;
    QStringList m_activeUsernames;
};
