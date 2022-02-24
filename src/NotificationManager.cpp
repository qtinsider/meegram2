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

    connect(&TdApi::getInstance(), SIGNAL(updateNotification(int, const QVariantMap &)), this,
            SLOT(handleNotification(int, const QVariantMap &)));

    TdApi::getInstance().setOption("notification_group_count_max", 5);
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

void NotificationManager::handleNotification(int notificationGroupId, const QVariantMap &notification)
{
}
