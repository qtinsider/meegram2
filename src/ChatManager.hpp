#pragma once

#include <td/telegram/td_api.h>

#include <QObject>

#include <memory>

class Chat;
class BasicGroup;
class StorageManager;
class Supergroup;
class User;

class ChatInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString status READ status NOTIFY infoChanged)

public:
    explicit ChatInfo(Chat *chat, QObject *parent = nullptr);

    QString title() const noexcept;
    QString status() const noexcept;

    void setOnlineCount(int onlineCount);

    void setUser(std::shared_ptr<User> user);
    void setBasicGroup(std::shared_ptr<BasicGroup> group);
    void setSupergroup(std::shared_ptr<Supergroup> group);

signals:
    void infoChanged();

private:
    void initializeMembers();
    void updateStatus();

    QString formatStatus(int memberCount, const char *memberKey, const char *onlineKey) const;
    int getMemberCountWithFallback() const;
    bool isServiceNotification() const;
    QString formatUserStatus() const;
    QString formatOfflineStatus() const;

    int m_onlineCount{};

    QString m_title, m_status;

    Chat *m_chat{};
    StorageManager *m_storageManager{};

    qlonglong m_chatTypeId{};

    std::shared_ptr<User> m_user;
    std::shared_ptr<BasicGroup> m_basicGroup;
    std::shared_ptr<Supergroup> m_supergroup;
};
