#pragma once

#include <QObject>
#include <QVariant>
#include <QVector>
#include <unordered_map>

class Client;

class StorageManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(Client *client READ client WRITE setClient)
    Q_PROPERTY(QVariantList chatFolders READ getChatFolders NOTIFY chatFoldersChanged)

public:
    explicit StorageManager(QObject *parent = nullptr);

    Client *client() const noexcept;
    void setClient(Client *client) noexcept;

    QVector<qint64> getChatIds() const noexcept;

    Q_INVOKABLE QVariantMap getBasicGroup(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getBasicGroupFullInfo(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getChat(qint64 chatId) const;
    Q_INVOKABLE QVariantMap getFile(qint32 fileId) const;
    Q_INVOKABLE QVariant getOption(const QString &name) const;
    Q_INVOKABLE QVariantMap getSupergroup(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getSupergroupFullInfo(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getUser(qint64 userId) const;
    Q_INVOKABLE QVariantMap getUserFullInfo(qint64 userId) const;

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

    std::unordered_map<int, QVariantMap> m_files;
    std::unordered_map<qint64, QVariantMap> m_chats;
    std::unordered_map<qint64, QVariantMap> m_basicGroup;
    std::unordered_map<qint64, QVariantMap> m_basicGroupFullInfo;
    std::unordered_map<qint64, QVariantMap> m_supergroup;
    std::unordered_map<qint64, QVariantMap> m_supergroupFullInfo;
    std::unordered_map<qint64, QVariantMap> m_users;
    std::unordered_map<qint64, QVariantMap> m_userFullInfo;
};
