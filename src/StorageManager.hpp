#pragma once

#include "BasicGroup.hpp"
#include "Chat.hpp"
#include "Client.hpp"
#include "File.hpp"
#include "Supergroup.hpp"
#include "SupergroupFullInfo.hpp"
#include "User.hpp"

#include <td/telegram/td_api.h>

#include <memory>
#include <unordered_map>
#include <vector>

class StorageManager : public QObject
{
    Q_OBJECT
public:
    static StorageManager &instance();

    StorageManager(const StorageManager &) = delete;
    StorageManager &operator=(const StorageManager &) = delete;

    [[nodiscard]] std::shared_ptr<Client> client() const noexcept;

    [[nodiscard]] std::vector<qlonglong> getChatIds() const noexcept;

    [[nodiscard]] std::vector<std::shared_ptr<ChatFolderInfo>> chatFolders() const noexcept;

    [[nodiscard]] std::shared_ptr<BasicGroup> getBasicGroup(qlonglong groupId) const noexcept;
    [[nodiscard]] std::shared_ptr<Chat> getChat(qlonglong chatId) const noexcept;
    [[nodiscard]] std::shared_ptr<File> getFile(int fileId) const noexcept;
    [[nodiscard]] std::shared_ptr<Supergroup> getSupergroup(qlonglong groupId) const noexcept;
    [[nodiscard]] std::shared_ptr<SupergroupFullInfo> getSupergroupFullInfo(qlonglong groupId) const noexcept;
    [[nodiscard]] std::shared_ptr<User> getUser(qlonglong userId) const noexcept;

    [[nodiscard]] QVariant getOption(const QString &name) const noexcept;

    [[nodiscard]] qlonglong myId() const noexcept;

signals:
    void basicGroupUpdated(qlonglong groupId);
    void basicGroupFullInfoUpdated(qlonglong groupId);
    void chatUpdated(qlonglong chatId);
    void chatPositionUpdated(qlonglong chatId);
    void fileUpdated(int fileId);
    void supergroupUpdated(qlonglong groupId);
    void supergroupFullInfoUpdated(qlonglong groupId);
    void userUpdated(qlonglong userId);
    void userFullInfoUpdated(qlonglong userId);
    void chatFoldersUpdated();

private slots:
    void handleResult(td::td_api::Object *object);

private:
    StorageManager();

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
