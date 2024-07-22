#pragma once

#include "BasicGroup.hpp"
#include "BasicGroupFullInfo.hpp"
#include "Chat.hpp"
#include "Client.hpp"
#include "File.hpp"
#include "Message.hpp"
#include "Supergroup.hpp"
#include "SupergroupFullInfo.hpp"
#include "User.hpp"
#include "UserFullInfo.hpp"

#include <QObject>
#include <QVariant>

#include <memory>
#include <unordered_map>
#include <vector>

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

    std::unordered_map<qint64, std::unique_ptr<BasicGroup>> m_basicGroup;
    std::unordered_map<qint64, std::unique_ptr<BasicGroupFullInfo>> m_basicGroupFullInfo;
    std::unordered_map<qint64, std::unique_ptr<Chat>> m_chats;
    std::unordered_map<qint32, std::unique_ptr<File>> m_files;
    std::unordered_map<qint64, std::unique_ptr<Supergroup>> m_supergroup;
    std::unordered_map<qint64, std::unique_ptr<SupergroupFullInfo>> m_supergroupFullInfo;
    std::unordered_map<qint64, std::unique_ptr<User>> m_users;
    std::unordered_map<qint64, std::unique_ptr<UserFullInfo>> m_userFullInfo;
};
