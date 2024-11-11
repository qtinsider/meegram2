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
    switch (object->get_id())
    {
        case td::td_api::updateNewChat::ID: {
            auto update = static_cast<td::td_api::updateNewChat *>(object);
            auto chat = std::make_shared<Chat>(std::move(update->chat_));
            auto chatId = chat->id();
            m_chats.emplace(chatId, std::move(chat));
            emit chatUpdated(chatId);
            break;
        }
        case td::td_api::updateChatTitle::ID: {
            auto update = static_cast<td::td_api::updateChatTitle *>(object);
            auto chatId = update->chat_id_;
            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second->setTitle(update->title_);
                emit chatUpdated(chatId);
            }
            break;
        }
        case td::td_api::updateChatPhoto::ID: {
            auto update = static_cast<td::td_api::updateChatPhoto *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setPhoto(std::move(update->photo_));
                emit chatUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatLastMessage::ID: {
            auto update = static_cast<td::td_api::updateChatLastMessage *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setLastMessage(std::move(update->last_message_));
                it->second->setPositions(std::move(update->positions_));
                emit chatUpdated(update->chat_id_);
                emit chatPositionUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatPosition::ID: {
            auto update = static_cast<td::td_api::updateChatPosition *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> result;
                result.emplace_back(std::move(update->position_));
                it->second->setPositions(std::move(result));
                emit chatUpdated(update->chat_id_);
                emit chatPositionUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatReadInbox::ID: {
            auto update = static_cast<td::td_api::updateChatReadInbox *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setUnreadCount(update->unread_count_);
                it->second->setLastReadInboxMessageId(update->last_read_inbox_message_id_);
                emit chatUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatReadOutbox::ID: {
            auto update = static_cast<td::td_api::updateChatReadOutbox *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setLastReadOutboxMessageId(update->last_read_outbox_message_id_);
                emit chatUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatDraftMessage::ID: {
            auto update = static_cast<td::td_api::updateChatDraftMessage *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setPositions(std::move(update->positions_));
                emit chatUpdated(update->chat_id_);
                emit chatPositionUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatNotificationSettings::ID: {
            auto update = static_cast<td::td_api::updateChatNotificationSettings *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setNotificationSettings(std::move(update->notification_settings_));
                emit chatUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatUnreadMentionCount::ID: {
            auto update = static_cast<td::td_api::updateChatUnreadMentionCount *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setUnreadMentionCount(update->unread_mention_count_);
                emit chatUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateChatIsMarkedAsUnread::ID: {
            auto update = static_cast<td::td_api::updateChatIsMarkedAsUnread *>(object);
            if (auto it = m_chats.find(update->chat_id_); it != m_chats.end())
            {
                it->second->setIsMarkedAsUnread(update->is_marked_as_unread_);
                emit chatUpdated(update->chat_id_);
            }
            break;
        }
        case td::td_api::updateUser::ID: {
            auto update = static_cast<td::td_api::updateUser *>(object);
            auto user = std::make_shared<User>(std::move(update->user_));
            auto userId = user->id();
            m_users.insert_or_assign(userId, std::move(user));
            emit userUpdated(userId);
            break;
        }
        case td::td_api::updateUserStatus::ID: {
            auto update = static_cast<td::td_api::updateUserStatus *>(object);
            auto userId = update->user_id_;
            if (auto it = m_users.find(userId); it != m_users.end())
            {
                it->second->setStatus(std::move(update->status_));
                emit userUpdated(userId);
            }
            break;
        }
        case td::td_api::updateBasicGroup::ID: {
            auto update = static_cast<td::td_api::updateBasicGroup *>(object);
            auto group = std::make_shared<BasicGroup>(std::move(update->basic_group_));
            auto groupId = group->id();
            m_basicGroup.insert_or_assign(groupId, std::move(group));
            emit basicGroupUpdated(groupId);
            break;
        }
        case td::td_api::updateSupergroup::ID: {
            auto update = static_cast<td::td_api::updateSupergroup *>(object);
            auto supergroup = std::make_shared<Supergroup>(std::move(update->supergroup_));
            auto groupId = supergroup->id();
            m_supergroup.insert_or_assign(groupId, std::move(supergroup));
            emit supergroupUpdated(groupId);
            break;
        }
        case td::td_api::updateSupergroupFullInfo::ID: {
            auto update = static_cast<td::td_api::updateSupergroupFullInfo *>(object);
            auto supergroupId = update->supergroup_id_;
            auto supergroupFullInfo = std::make_shared<SupergroupFullInfo>(std::move(update->supergroup_full_info_));
            m_supergroupFullInfo.insert_or_assign(supergroupId, std::move(supergroupFullInfo));
            emit supergroupFullInfoUpdated(supergroupId);
            break;
        }
        case td::td_api::updateChatFolders::ID: {
            auto update = static_cast<td::td_api::updateChatFolders *>(object);
            m_chatFolders.clear();
            m_chatFolders.reserve(update->chat_folders_.size());
            std::transform(update->chat_folders_.begin(), update->chat_folders_.end(), std::back_inserter(m_chatFolders),
                           [](auto &&folder) { return std::make_shared<ChatFolderInfo>(std::move(folder)); });
            emit chatFoldersUpdated();
            break;
        }
        case td::td_api::updateFile::ID: {
            auto update = static_cast<td::td_api::updateFile *>(object);
            auto fileId = update->file_->id_;
            if (auto it = m_files.find(fileId); it != m_files.end())
            {
                it->second->setFile(std::move(update->file_));
            }
            else
            {
                m_files[fileId] = std::make_shared<File>(std::move(update->file_));
            }
            emit fileUpdated(fileId);
            break;
        }
        case td::td_api::updateOption::ID: {
            auto update = static_cast<td::td_api::updateOption *>(object);
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
            m_options.insert(QString::fromStdString(update->name_), optionValue(std::move(update->value_)));
            break;
        }
        default:
            break;
    }
}
