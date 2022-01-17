#include "NotificationManager.hpp"

#include "TdApi.hpp"

NotificationManager::NotificationManager()
{
    connect(&TdApi::getInstance(), SIGNAL(updateActiveNotifications(QVariantList)), this, SLOT(handleActiveNotifications(QVariantList)));
    connect(
        &TdApi::getInstance(),
        SIGNAL(updateNotificationGroup(int, const QVariantMap &, qint64, qint64, bool, int, const QVariantList &, const QVariantList &)),
        this,
        SLOT(handleNotificationGroup(int, const QVariantMap &, qint64, qint64, bool, int, const QVariantList &, const QVariantList &)));

    connect(&TdApi::getInstance(), SIGNAL(updateNotification(qint32, const QVariantMap &)), this,
            SLOT(handleNotification(qint32, const QVariantMap &)));
}

NotificationManager &NotificationManager::getInstance()
{
    static NotificationManager staticObject;
    return staticObject;
}

void NotificationManager::handleActiveNotifications(const QVariantList &groups)
{
}

void NotificationManager::handleNotificationGroup(int notificationGroupId, const QVariantMap &type, qint64 chatId,
                                                  qint64 notificationSettingsChatId, bool isSilent, int totalCount,
                                                  const QVariantList &addedNotifications, const QVariantList &removedNotificationIds)
{
}

void NotificationManager::handleNotification(qint32 notificationGroupId, const QVariantMap &notification)
{
}
