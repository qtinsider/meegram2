#include "User.hpp"

User::User(td::td_api::object_ptr<td::td_api::user> userFullInfo, QObject *parent)
    : QObject(parent)
    , m_id(userFullInfo->id_)
    , m_firstName(QString::fromStdString(userFullInfo->first_name_))
    , m_lastName(QString::fromStdString(userFullInfo->last_name_))
    , m_isSupport(userFullInfo->is_support_)
    , m_type(determineType(userFullInfo->type_))
{
    if (userFullInfo->usernames_)
    {
        for (const auto &activeUsername : userFullInfo->usernames_->active_usernames_)
        {
            m_activeUsernames.append(QString::fromStdString(activeUsername));
        }
    }

    setStatus(std::move(userFullInfo->status_));
}

qlonglong User::id() const
{
    return m_id;
}

QString User::firstName() const
{
    return m_firstName;
}

QString User::lastName() const
{
    return m_lastName;
}

User::Status User::status() const
{
    return m_status;
}

bool User::isSupport() const
{
    return m_isSupport;
}

User::Type User::type() const
{
    return m_type;
}

QDateTime User::wasOnline() const
{
    return m_wasOnline;
}

QStringList User::activeUsernames() const
{
    return m_activeUsernames;
}

void User::setStatus(td::td_api::object_ptr<td::td_api::UserStatus> status)
{
    if (!status)
    {
        m_status = Status::Empty;
        emit statusChanged();
        return;
    }

    switch (status->get_id())
    {
        case td::td_api::userStatusEmpty::ID:
            m_status = Status::Empty;
            break;
        case td::td_api::userStatusOnline::ID:
            m_status = Status::Online;
            break;
        case td::td_api::userStatusOffline::ID: {
            const auto *offlineStatus = static_cast<const td::td_api::userStatusOffline *>(status.get());
            m_wasOnline = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(offlineStatus->was_online_) * 1000);
            m_status = Status::Offline;
            break;
        }
        case td::td_api::userStatusRecently::ID:
            m_status = Status::Recently;
            break;
        case td::td_api::userStatusLastWeek::ID:
            m_status = Status::LastWeek;
            break;
        case td::td_api::userStatusLastMonth::ID:
            m_status = Status::LastMonth;
            break;
        default:
            m_status = Status::Empty;
            break;
    }

    emit statusChanged();
}

User::Type User::determineType(const td::td_api::object_ptr<td::td_api::UserType> &type) const
{
    if (!type)
        return Type::Unknown;

    switch (type->get_id())
    {
        case td::td_api::userTypeBot::ID:
            return Type::Bot;
        case td::td_api::userTypeDeleted::ID:
            return Type::Deleted;
        case td::td_api::userTypeRegular::ID:
            return Type::Regular;
        case td::td_api::userTypeUnknown::ID:
            return Type::Unknown;
        default:
            return Type::Unknown;
    }
}
