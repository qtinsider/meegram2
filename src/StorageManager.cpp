#include "StorageManager.hpp"

#include "Common.hpp"

#include <QDebug>

#include <algorithm>
#include <ranges>
#include <unordered_set>

StorageManager::StorageManager()
    : m_client(std::make_shared<Client>())
{
    connect(m_client.get(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

StorageManager &StorageManager::instance()
{
    static StorageManager staticObject;
    return staticObject;
}

std::shared_ptr<Client> StorageManager::client() const noexcept
{
    return m_client;
}

std::vector<qlonglong> StorageManager::getChatIds() const noexcept
{
    auto view = m_chats | std::views::keys;
    return std::vector(view.begin(), view.end());
}

std::shared_ptr<BasicGroup> StorageManager::getBasicGroup(qlonglong groupId) const noexcept
{
    if (auto it = m_basicGroup.find(groupId); it != m_basicGroup.end())
    {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<BasicGroupFullInfo> StorageManager::getBasicGroupFullInfo(qlonglong groupId) const noexcept
{
    if (auto it = m_basicGroupFullInfo.find(groupId); it != m_basicGroupFullInfo.end())
    {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<Chat> StorageManager::getChat(qlonglong chatId) const noexcept
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<File> StorageManager::getFile(int fileId) const noexcept
{
    if (auto it = m_files.find(fileId); it != m_files.end())
    {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<Supergroup> StorageManager::getSupergroup(qlonglong groupId) const noexcept
{
    if (auto it = m_supergroup.find(groupId); it != m_supergroup.end())
    {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<SupergroupFullInfo> StorageManager::getSupergroupFullInfo(qlonglong groupId) const noexcept
{
    if (auto it = m_supergroupFullInfo.find(groupId); it != m_supergroupFullInfo.end())
    {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<User> StorageManager::getUser(qlonglong userId) const noexcept
{
    if (auto it = m_users.find(userId); it != m_users.end())
    {
        return it->second;
    }

    return nullptr;
}

std::shared_ptr<UserFullInfo> StorageManager::getUserFullInfo(qlonglong userId) const noexcept
{
    if (auto it = m_userFullInfo.find(userId); it != m_userFullInfo.end())
    {
        return it->second;
    }

    return nullptr;
}

QVariant StorageManager::getOption(const QString &name) const noexcept
{
    if (auto it = m_options.find(name); it != m_options.end())
        return it.value();

    return QVariant();
}

std::vector<std::shared_ptr<ChatFolderInfo>> StorageManager::chatFolders() const noexcept
{
    return m_chatFolders;
}

qlonglong StorageManager::myId() const noexcept
{
    if (const auto value = getOption("my_id"); not value.isNull())
        return value.toLongLong();

    return 0;
}

void StorageManager::handleResult(td::td_api::Object *object)
{
    td::td_api::downcast_call(
        *object, detail::Overloaded{
                     [this](td::td_api::updateNewChat &value) {
                         auto chat = std::make_shared<Chat>(std::move(value.chat_));
                         auto chatId = chat->id();

                         m_chats.emplace(chatId, std::move(chat));

                         emit chatUpdated(chatId);
                     },
                     [this](td::td_api::updateChatTitle &value) {
                         auto chatId = value.chat_id_;

                         if (auto it = m_chats.find(chatId); it != m_chats.end())
                         {
                             it->second->setTitle(value.title_);

                             emit chatUpdated(chatId);
                         }
                     },
                     [this](td::td_api::updateChatPhoto &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setPhoto(std::move(value.photo_));
                             emit chatUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatLastMessage &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setLastMessage(std::move(value.last_message_));
                             it->second->setPositions(std::move(value.positions_));

                             emit chatUpdated(value.chat_id_);
                             emit chatPositionUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatPosition &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> result;
                             result.emplace_back(std::move(value.position_));

                             it->second->setPositions(std::move(result));

                             emit chatUpdated(value.chat_id_);
                             emit chatPositionUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatReadInbox &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setUnreadCount(value.unread_count_);
                             it->second->setLastReadInboxMessageId(value.last_read_inbox_message_id_);

                             emit chatUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatReadOutbox &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setLastReadOutboxMessageId(value.last_read_outbox_message_id_);

                             emit chatUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatDraftMessage &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setPositions(std::move(value.positions_));

                             emit chatUpdated(value.chat_id_);
                             emit chatPositionUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatNotificationSettings &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setNotificationSettings(std::move(value.notification_settings_));

                             emit chatUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatUnreadMentionCount &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setUnreadMentionCount(value.unread_mention_count_);

                             emit chatUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateChatIsMarkedAsUnread &value) {
                         if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                         {
                             it->second->setIsMarkedAsUnread(value.is_marked_as_unread_);

                             emit chatUpdated(value.chat_id_);
                         }
                     },
                     [this](td::td_api::updateUser &value) {
                         auto user = std::make_shared<User>(std::move(value.user_));
                         auto userId = user->id();

                         m_users.insert_or_assign(userId, std::move(user));

                         emit userUpdated(userId);
                     },
                     [this](td::td_api::updateUserStatus &value) {
                         auto userId = value.user_id_;

                         if (auto it = m_users.find(userId); it != m_users.end())
                         {
                             it->second->setStatus(std::move(value.status_));

                             emit userUpdated(userId);
                         }
                     },
                     [this](td::td_api::updateBasicGroup &value) {
                         auto group = std::make_shared<BasicGroup>(std::move(value.basic_group_));
                         auto groupId = group->id();

                         m_basicGroup.insert_or_assign(groupId, std::move(group));

                         emit basicGroupUpdated(groupId);
                     },
                     [this](td::td_api::updateSupergroup &value) {
                         auto supergroup = std::make_shared<Supergroup>(std::move(value.supergroup_));
                         auto groupId = supergroup->id();

                         m_supergroup.insert_or_assign(groupId, std::move(supergroup));

                         emit supergroupUpdated(groupId);
                     },
                     [this](td::td_api::updateUserFullInfo &value) {
                         auto userId = value.user_id_;
                         auto userFullInfo = std::make_shared<UserFullInfo>(std::move(value.user_full_info_));

                         m_userFullInfo.insert_or_assign(userId, std::move(userFullInfo));

                         emit userFullInfoUpdated(userId);
                     },
                     [this](td::td_api::updateBasicGroupFullInfo &value) {
                         auto basicGroupId = value.basic_group_id_;
                         auto basicGroupFullInfo = std::make_shared<BasicGroupFullInfo>(std::move(value.basic_group_full_info_));

                         m_basicGroupFullInfo.insert_or_assign(basicGroupId, std::move(basicGroupFullInfo));

                         emit basicGroupFullInfoUpdated(basicGroupId);
                     },
                     [this](td::td_api::updateSupergroupFullInfo &value) {
                         auto supergroupId = value.supergroup_id_;
                         auto supergroupFullInfo = std::make_shared<SupergroupFullInfo>(std::move(value.supergroup_full_info_));

                         m_supergroupFullInfo.insert_or_assign(supergroupId, std::move(supergroupFullInfo));

                         emit supergroupFullInfoUpdated(supergroupId);
                     },
                     [this](td::td_api::updateChatFolders &value) {
                         m_chatFolders = std::ranges::to<std::vector<std::shared_ptr<ChatFolderInfo>>>(
                             value.chat_folders_ | std::views::transform([](auto &&folder) { return std::make_shared<ChatFolderInfo>(std::move(folder)); }));

                         emit chatFoldersUpdated();
                     },
                     [this](td::td_api::updateFile &value) {
                         auto fileId = value.file_->id_;
                         if (auto it = m_files.find(fileId); it != m_files.end())
                         {
                             it->second->setFile(std::move(value.file_));
                         }
                         else
                         {
                             m_files[fileId] = std::make_shared<File>(std::move(value.file_));
                         }

                         emit fileUpdated(fileId);
                     },
                     [this](td::td_api::updateOption &value) {
                         auto optionValue = [](td::td_api::object_ptr<td::td_api::OptionValue> &&option) -> QVariant {
                             switch (option->get_id())
                             {
                                 case td::td_api::optionValueBoolean::ID:
                                     return QVariant::fromValue(static_cast<td::td_api::optionValueBoolean *>(option.get())->value_);
                                 case td::td_api::optionValueInteger::ID:
                                     return QVariant::fromValue(static_cast<td::td_api::optionValueInteger *>(option.get())->value_);
                                 case td::td_api::optionValueString::ID:
                                     return QVariant::fromValue(QString::fromStdString(static_cast<td::td_api::optionValueString *>(option.get())->value_));
                                 default:
                                     return QVariant();
                             }
                         };

                         m_options.insert(QString::fromStdString(value.name_), optionValue(std::move(value.value_)));
                     },
                     [](auto &) {}});
}
