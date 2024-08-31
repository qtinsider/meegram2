#pragma once

#include <td/telegram/td_api.h>

#include <QObject>
#include <QVariant>

class Client;

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    static NotificationManager &instance();

    NotificationManager(const NotificationManager &) = delete;
    NotificationManager &operator=(const NotificationManager &) = delete;

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void handleActiveNotifications(const QVariantList &groups);
    void handleNotificationGroup(int notificationGroupId, const QVariantMap &type, qint64 chatId, qint64 notificationSettingsChatId, bool isSilent,
                                 int totalCount, const QVariantList &addedNotifications, const QVariantList &removedNotificationIds);

    void handleNotification(int notificationGroupId, const QVariantMap &notification);

private:
    NotificationManager();

    Client *m_client;
};
