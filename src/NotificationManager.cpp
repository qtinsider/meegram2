#include "NotificationManager.hpp"

#include "StorageManager.hpp"

NotificationManager::NotificationManager()
    : m_client(StorageManager::instance().client())
{
    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

NotificationManager &NotificationManager::instance()
{
    static NotificationManager staticObject;
    return staticObject;
}

void NotificationManager::handleResult(td::td_api::Object *object)
{
}

void NotificationManager::handleActiveNotifications(const QVariantList &groups)
{
}

void NotificationManager::handleNotificationGroup(int notificationGroupId, const QVariantMap &type, qlonglong chatId, qlonglong notificationSettingsChatId,
                                                  bool isSilent, int totalCount, const QVariantList &addedNotifications,
                                                  const QVariantList &removedNotificationIds)
{
}

void NotificationManager::handleNotification(int notificationGroupId, const QVariantMap &notification)
{
}
