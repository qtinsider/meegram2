#pragma once

#include "Client.hpp"

#include <td/telegram/td_api.h>

#include <QVariant>

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

    [[nodiscard]] Client *client() const noexcept;

    [[nodiscard]] std::vector<int64_t> getChatIds() const noexcept;

    [[nodiscard]] td::td_api::basicGroup *getBasicGroup(qlonglong groupId) noexcept;
    [[nodiscard]] td::td_api::basicGroupFullInfo *getBasicGroupFullInfo(qlonglong groupId) noexcept;
    [[nodiscard]] td::td_api::chat *getChat(qlonglong chatId) noexcept;
    [[nodiscard]] td::td_api::file *getFile(int fileId) noexcept;
    [[nodiscard]] QVariant getOption(const QString &name) const noexcept;
    [[nodiscard]] td::td_api::supergroup *getSupergroup(qlonglong groupId) noexcept;
    [[nodiscard]] td::td_api::supergroupFullInfo *getSupergroupFullInfo(qlonglong groupId) noexcept;
    [[nodiscard]] td::td_api::user *getUser(qlonglong userId) noexcept;
    [[nodiscard]] td::td_api::userFullInfo *getUserFullInfo(qlonglong userId) noexcept;

    [[nodiscard]] const std::vector<const td::td_api::chatFolderInfo *> &chatFolders() const noexcept;

    [[nodiscard]] qlonglong myId() const noexcept;

signals:
    void basicGroupUpdated(td::td_api::Object *object);
    void basicGroupFullInfoUpdated(td::td_api::Object *object);
    void chatsUpdated(td::td_api::Object *object);
    void filesUpdated(td::td_api::Object *object);
    void supergroupUpdated(td::td_api::Object *object);
    void supergroupFullInfoUpdated(td::td_api::Object *object);
    void usersUpdated(td::td_api::Object *object);
    void userFullInfoUpdated(td::td_api::Object *object);
    void chatFoldersUpdated(td::td_api::Object *object);

private slots:
    void handleResult(td::td_api::Object *object);

private:
    StorageManager();

    template <typename MapType, typename KeyType>
    constexpr auto getPointer(MapType &map, const KeyType &key) noexcept -> typename MapType::mapped_type::element_type *
    {
        if (auto it = map.find(key); it != map.end())
        {
            return it->second.get();
        }
        return nullptr;
    }

    void setChatPositions(qlonglong chatId, std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> &&positions) noexcept;

    QVariantMap m_options;

    std::unique_ptr<Client> m_client;

    std::vector<const td::td_api::chatFolderInfo *> m_chatFolders;

    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::basicGroup>> m_basicGroup;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::basicGroupFullInfo>> m_basicGroupFullInfo;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::chat>> m_chats;
    std::unordered_map<int32_t, td::td_api::object_ptr<td::td_api::file>> m_files;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::supergroup>> m_supergroup;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::supergroupFullInfo>> m_supergroupFullInfo;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::user>> m_users;
    std::unordered_map<int64_t, td::td_api::object_ptr<td::td_api::userFullInfo>> m_userFullInfo;
};
