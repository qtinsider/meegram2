#pragma once

#include <td/telegram/td_api.h>

#include <QVariant>

#include <memory>

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
    void handleNotificationGroup(int notificationGroupId, const QVariantMap &type, qlonglong chatId, qlonglong notificationSettingsChatId, bool isSilent,
                                 int totalCount, const QVariantList &addedNotifications, const QVariantList &removedNotificationIds);

    void handleNotification(int notificationGroupId, const QVariantMap &notification);

private:
    NotificationManager();

    std::shared_ptr<Client> m_client;
};
