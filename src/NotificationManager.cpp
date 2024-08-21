#include "NotificationManager.hpp"

#include "StorageManager.hpp"

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
{
    m_client = StorageManager::instance().client();

    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

void NotificationManager::handleResult(td::td_api::Object *object)
{
}

void NotificationManager::handleActiveNotifications(const QVariantList &groups)
{
}

void NotificationManager::handleNotificationGroup(int notificationGroupId, const QVariantMap &type, qint64 chatId, qint64 notificationSettingsChatId,
                                                  bool isSilent, int totalCount, const QVariantList &addedNotifications,
                                                  const QVariantList &removedNotificationIds)
{
}

void NotificationManager::handleNotification(int notificationGroupId, const QVariantMap &notification)
{
}
