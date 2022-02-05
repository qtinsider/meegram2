#pragma once

#include <QObject>
#include <QVariant>

class NotificationManager : public QObject
{
    Q_OBJECT
public:
    NotificationManager(const NotificationManager &) = delete;
    NotificationManager &operator=(const NotificationManager &) = delete;

    static NotificationManager &getInstance();

private slots:
    void handleActiveNotifications(const QVariantList &groups);
    void handleNotificationGroup(int notificationGroupId, const QVariantMap &type, qint64 chatId, qint64 notificationSettingsChatId,
                                 bool isSilent, int totalCount, const QVariantList &addedNotifications,
                                 const QVariantList &removedNotificationIds);

    void handleNotification(int notificationGroupId, const QVariantMap &notification);

private:
    NotificationManager();
};
