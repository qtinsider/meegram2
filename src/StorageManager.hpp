#pragma once

#include <td/telegram/td_api.h>

#include <QObject>
#include <QVariant>

#include <memory>
#include <unordered_map>
#include <vector>

class Client;

class StorageManager : public QObject
{
    Q_OBJECT
public:
    explicit StorageManager(Client *client, QObject *parent = nullptr);

    Client *client() const noexcept;

    std::vector<qint64> getChatIds() const noexcept;

    const td::td_api::basicGroup *getBasicGroup(qint64 groupId) const;
    const td::td_api::basicGroupFullInfo *getBasicGroupFullInfo(qint64 groupId) const;
    const td::td_api::chat *getChat(qint64 chatId) const;
    const td::td_api::file *getFile(qint32 fileId) const;
    QVariant getOption(const QString &name) const;
    const td::td_api::supergroup *getSupergroup(qint64 groupId) const;
    const td::td_api::supergroupFullInfo *getSupergroupFullInfo(qint64 groupId) const;
    const td::td_api::user *getUser(qint64 userId) const;
    const td::td_api::userFullInfo *getUserFullInfo(qint64 userId) const;

    Q_INVOKABLE qint64 getMyId() const noexcept;

signals:
    void updateChatItem(qint64 chatId);
    void updateChatPosition(qint64 chatId);

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void setChatPositions(qint64 chatId, std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> &&positions) noexcept;

    Client *m_client{nullptr};

    QVariantMap m_options;

    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::basicGroup>> m_basicGroup;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::basicGroupFullInfo>> m_basicGroupFullInfo;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::chat>> m_chats;
    std::unordered_map<int32_t, td::td_api::object_ptr<td::td_api::file>> m_files;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::supergroup>> m_supergroup;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::supergroupFullInfo>> m_supergroupFullInfo;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::user>> m_users;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::userFullInfo>> m_userFullInfo;
};
