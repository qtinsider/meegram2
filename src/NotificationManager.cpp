#include "NotificationManager.hpp"

#include "Client.hpp"
#include "StorageManager.hpp"
#include "TdManager.hpp"

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

    m_manager->setOption("notification_group_count_max", 5);
}

TdManager *NotificationManager::manager() const
{
    return m_manager;
}

void NotificationManager::setManager(TdManager *manager)
{
    m_manager = manager;
    m_client = m_manager->storageManager()->client();

    connect(m_client, SIGNAL(result(const QVariantMap &)), SLOT(handleResult(const QVariantMap &)));
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
