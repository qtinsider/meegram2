#pragma once

#include <QObject>
#include <QVariant>

#include <unordered_map>
#include <vector>

class BasicGroup;
class BasicGroupFullInfo;
class Chat;
class File;
class Supergroup;
class SupergroupFullInfo;
class User;
class UserFullInfo;

class Client;

class StorageManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList chatFolders READ getChatFolders NOTIFY chatFoldersChanged)

public:
    explicit StorageManager(QObject *parent = nullptr);

    Client *client() const noexcept;

    std::vector<qint64> getChatIds() const noexcept;

    Q_INVOKABLE BasicGroup *getBasicGroup(qint64 groupId) const;
    Q_INVOKABLE BasicGroupFullInfo *getBasicGroupFullInfo(qint64 groupId) const;
    Q_INVOKABLE Chat *getChat(qint64 chatId) const;
    Q_INVOKABLE File *getFile(qint32 fileId) const;
    Q_INVOKABLE QVariant getOption(const QString &name) const;
    Q_INVOKABLE Supergroup *getSupergroup(qint64 groupId) const;
    Q_INVOKABLE SupergroupFullInfo *getSupergroupFullInfo(qint64 groupId) const;
    Q_INVOKABLE User *getUser(qint64 userId) const;
    Q_INVOKABLE UserFullInfo *getUserFullInfo(qint64 userId) const;

    QVariantList getChatFolders() const noexcept;

    Q_INVOKABLE qint64 getMyId() const noexcept;

signals:
    void updateChatItem(qint64 chatId);
    void updateChatPosition(qint64 chatId);

    void chatFoldersChanged();

private slots:
    void handleResult(const QVariantMap &object);

private:
    void setChatPositions(qint64 chatId, const QVariantList &positions) noexcept;

    Client *m_client{nullptr};

    QVariantMap m_options;
    QVariantList m_chatFolders;

    std::unordered_map<qint64, BasicGroup *> m_basicGroup;
    std::unordered_map<qint64, BasicGroupFullInfo *> m_basicGroupFullInfo;
    std::unordered_map<qint64, Chat *> m_chats;
    std::unordered_map<int, File *> m_files;
    std::unordered_map<qint64, Supergroup *> m_supergroup;
    std::unordered_map<qint64, SupergroupFullInfo *> m_supergroupFullInfo;
    std::unordered_map<qint64, User *> m_users;
    std::unordered_map<qint64, UserFullInfo *> m_userFullInfo;
};
