#pragma once

#include "BasicGroup.hpp"
#include "Chat.hpp"
#include "ChatFolderModel.hpp"
#include "Client.hpp"
#include "File.hpp"
#include "Supergroup.hpp"
#include "SupergroupFullInfo.hpp"
#include "User.hpp"

#include <td/telegram/td_api.h>

#include <memory>
#include <unordered_map>

class StorageManager : public QObject
{
    Q_OBJECT
public:
    explicit StorageManager(std::shared_ptr<Client> client, QObject *parent = nullptr);

    [[nodiscard]] std::shared_ptr<Client> client() const noexcept;

    [[nodiscard]] std::vector<qlonglong> chatIds() const noexcept;

    [[nodiscard]] std::vector<std::shared_ptr<ChatFolderInfo>> chatFolders() const noexcept;

    [[nodiscard]] std::shared_ptr<BasicGroup> basicGroup(qlonglong groupId) const noexcept;
    [[nodiscard]] std::shared_ptr<Chat> chat(qlonglong chatId) const noexcept;
    [[nodiscard]] std::shared_ptr<File> file(int fileId) const noexcept;
    [[nodiscard]] std::shared_ptr<Supergroup> supergroup(qlonglong groupId) const noexcept;
    [[nodiscard]] std::shared_ptr<SupergroupFullInfo> supergroupFullInfo(qlonglong groupId) const noexcept;
    [[nodiscard]] std::shared_ptr<User> user(qlonglong userId) const noexcept;

signals:
    void chatFoldersUpdated();
    void basicGroupUpdated(qlonglong groupId);
    void basicGroupFullInfoUpdated(qlonglong groupId);
    void chatUpdated(qlonglong chatId);
    void chatPositionUpdated(qlonglong chatId);
    void fileUpdated(int fileId);
    void supergroupUpdated(qlonglong groupId);
    void supergroupFullInfoUpdated(qlonglong groupId);
    void userUpdated(qlonglong userId);
    void userFullInfoUpdated(qlonglong userId);

    void chatOnlineMemberCountUpdated(qlonglong chatId, int onlineMemberCount);

public slots:
    qlonglong myId() const noexcept;

    QVariant getOption(const QString &name) const noexcept;

    BasicGroup *getBasicGroup(qlonglong groupId) const noexcept;
    Chat *getChat(qlonglong chatId) const noexcept;
    File *getFile(int fileId) const noexcept;
    Supergroup *getSupergroup(qlonglong groupId) const noexcept;
    SupergroupFullInfo *getSupergroupFullInfo(qlonglong groupId) const noexcept;
    User *getUser(qlonglong userId) const noexcept;

private slots:
    void handleResult(td::td_api::Object *object);

private:
    QVariantMap m_options;

    std::shared_ptr<Client> m_client;

    std::vector<std::shared_ptr<ChatFolderInfo>> m_chatFolders;

    std::unordered_map<qlonglong, std::shared_ptr<BasicGroup>> m_basicGroup;
    std::unordered_map<qlonglong, std::shared_ptr<Chat>> m_chats;
    std::unordered_map<int, std::shared_ptr<File>> m_files;
    std::unordered_map<qlonglong, std::shared_ptr<Supergroup>> m_supergroup;
    std::unordered_map<qlonglong, std::shared_ptr<SupergroupFullInfo>> m_supergroupFullInfo;
    std::unordered_map<qlonglong, std::shared_ptr<User>> m_users;
};
