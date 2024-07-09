#include "NotificationManager.hpp"

#include "Client.hpp"
#include "StorageManager.hpp"

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
{
    //connect(&TdApi::getInstance(), SIGNAL(updateActiveNotifications(QVariantList)), this, SLOT(handleActiveNotifications(QVariantList)));
    //connect(
    //    &TdApi::getInstance(),
    //    SIGNAL(updateNotificationGroup(int, const QVariantMap &, qint64, qint64, bool, int, const QVariantList &, const QVariantList &)),
    //    this,
    //    SLOT(handleNotificationGroup(int, const QVariantMap &, qint64, qint64, bool, int, const QVariantList &, const QVariantList &)));

    //connect(&TdApi::getInstance(), SIGNAL(updateNotification(int, const QVariantMap &)), this,
    //        SLOT(handleNotification(int, const QVariantMap &)));

    // m_client->send("notification_group_count_max", 5);
}

Client *NotificationManager::client() const noexcept
{
    return m_client;
}

void NotificationManager::setClient(Client *client) noexcept
{
    m_client = client;

    connect(m_client, SIGNAL(result(const QVariantMap &)), this, SLOT(handleResult(const QVariantMap &)));
}

void NotificationManager::handleActiveNotifications(const QVariantList &groups)
{
}

void NotificationManager::handleNotificationGroup(int notificationGroupId, const QVariantMap &type, qint64 chatId,
                                                  qint64 notificationSettingsChatId, bool isSilent, int totalCount,
                                                  const QVariantList &addedNotifications, const QVariantList &removedNotificationIds)
{
}

void NotificationManager::handleNotification(int notificationGroupId, const QVariantMap &notification)
{
}
