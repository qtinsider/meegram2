#include "ChatManager.hpp"

#include "BasicGroup.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Supergroup.hpp"
#include "Utils.hpp"

#include <QDateTime>

#include <algorithm>

ChatInfo::ChatInfo(Chat *chat, QObject *parent)
    : QObject(parent)
    , m_chat(chat)
    , m_storageManager(&StorageManager::instance())
{
    initializeMembers();
    updateStatus();
}

QString ChatInfo::title() const noexcept
{
    return Utils::getChatTitle(m_chat->id(), true);
}

QString ChatInfo::status() const noexcept
{
    return m_status;
}

void ChatInfo::setOnlineCount(int onlineCount)
{
    if (m_onlineCount != onlineCount)
    {
        m_onlineCount = onlineCount;
        updateStatus();
    }
}

void ChatInfo::setUser(std::shared_ptr<User> user)
{
    m_user = user;
    updateStatus();
}

void ChatInfo::setBasicGroup(std::shared_ptr<BasicGroup> group)
{
    m_basicGroup = group;
    updateStatus();
}

void ChatInfo::setSupergroup(std::shared_ptr<Supergroup> group)
{
    m_supergroup = group;
    updateStatus();
}

void ChatInfo::initializeMembers()
{
    const auto chatType = m_chat->type();
    const auto chatTypeId = m_chat->typeId();

    if (chatType == Chat::Private || chatType == Chat::Secret)
    {
        m_user = m_storageManager->getUser(chatTypeId);
    }

    if (chatType == Chat::BasicGroup)
    {
        m_basicGroup = m_storageManager->getBasicGroup(chatTypeId);
    }

    if (chatType == Chat::Supergroup || chatType == Chat::Channel)
    {
        m_supergroup = m_storageManager->getSupergroup(chatTypeId);
    }
}

void ChatInfo::updateStatus()
{
    QString newStatus;

    if (m_basicGroup)
    {
        if (m_basicGroup->status() == BasicGroup::Status::Banned)
            newStatus = QObject::tr("YouWereKicked");
        else
            newStatus = formatStatus(m_basicGroup->memberCount(), "Members", "OnlineCount");
    }
    else if (m_supergroup)
    {
        if (!m_supergroup->isChannel() && m_supergroup->status() == Supergroup::Status::Banned)
        {
            newStatus = QObject::tr("YouWereKicked");
        }
        else
        {
            int count = getMemberCountWithFallback();
            newStatus = (count <= 0) ? (m_supergroup->hasLocation()
                                            ? QObject::tr("MegaLocation")
                                            : (m_supergroup->activeUsernames().isEmpty() ? QObject::tr("MegaPrivate") : QObject::tr("MegaPublic")))
                                     : formatStatus(count, "Members", "OnlineCount");
        }
    }
    else if (m_user)
    {
        if (isServiceNotification())
        {
            newStatus = QObject::tr("ServiceNotifications");
        }
        else if (m_user->isSupport())
        {
            newStatus = QObject::tr("SupportStatus");
        }
        else if (m_user->type() == User::Type::Bot)
        {
            newStatus = QObject::tr("Bot");
        }
        else
        {
            newStatus = formatUserStatus();
        }
    }

    if (m_status != newStatus)
    {
        m_status = newStatus;
        emit infoChanged();
    }
}

int ChatInfo::getMemberCountWithFallback() const
{
    int count = m_supergroup->memberCount();
    if (count == 0)
    {
        if (const auto fullInfo = m_storageManager->getSupergroupFullInfo(m_supergroup->id()))
        {
            count = fullInfo->memberCount();
        }
    }

    return count;
}

QString ChatInfo::formatStatus(int memberCount, const char *memberKey, const char *onlineKey) const
{
    const auto memberString = Locale::instance().formatPluralString(memberKey, memberCount);
    if (memberCount <= 1)
        return memberString;

    if (m_onlineCount > 1)
    {
        return memberString + ", " + Locale::instance().formatPluralString(onlineKey, m_onlineCount);
    }

    return memberString;
}

bool ChatInfo::isServiceNotification() const
{
    return std::ranges::contains(ServiceNotificationsUserIds, m_user->id());
}

QString ChatInfo::formatUserStatus() const
{
    switch (m_user->status())
    {
        case User::Status::Empty:
            return QObject::tr("ALongTimeAgo");
        case User::Status::LastMonth:
            return QObject::tr("WithinAMonth");
        case User::Status::LastWeek:
            return QObject::tr("WithinAWeek");
        case User::Status::Offline:
            return formatOfflineStatus();
        case User::Status::Online:
            return QObject::tr("Online");
        case User::Status::Recently:
            return QObject::tr("Lately");
        default:
            return {};
    }
}

QString ChatInfo::formatOfflineStatus() const
{
    const auto wasOnline = m_user->wasOnline();
    if (wasOnline.isNull())
        return QObject::tr("Invisible");

    const auto currentDate = QDate::currentDate();
    if (currentDate == wasOnline.date())
    {
        return QObject::tr("LastSeenFormatted").arg(QObject::tr("TodayAtFormatted")).arg(wasOnline.toString(QObject::tr("formatterDay12H")));
    }
    else if (wasOnline.date().daysTo(currentDate) < 2)
    {
        return QObject::tr("LastSeenFormatted").arg(QObject::tr("YesterdayAtFormatted")).arg(wasOnline.toString(QObject::tr("formatterDay12H")));
    }

    return QObject::tr("LastSeenDateFormatted")
        .arg(QObject::tr("formatDateAtTime").arg(wasOnline.toString(QObject::tr("formatterYear"))).arg(wasOnline.toString(QObject::tr("formatterDay12H"))));
}
