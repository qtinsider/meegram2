#include "StorageManager.hpp"

#include "Common.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <ranges>
#include <unordered_set>

StorageManager::StorageManager()
    : m_client(std::make_unique<Client>())
{
    connect(m_client.get(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

StorageManager &StorageManager::instance()
{
    static StorageManager staticObject;
    return staticObject;
}

Client *StorageManager::client() const noexcept
{
    return m_client.get();
}

std::vector<int64_t> StorageManager::getChatIds() const noexcept
{
    auto view = m_chats | std::views::keys;
    return std::vector(view.begin(), view.end());
}

td::td_api::basicGroup *StorageManager::getBasicGroup(qlonglong groupId) noexcept
{
    return getPointer(m_basicGroup, groupId);
}

td::td_api::basicGroupFullInfo *StorageManager::getBasicGroupFullInfo(qlonglong groupId) noexcept
{
    return getPointer(m_basicGroupFullInfo, groupId);
}

td::td_api::chat *StorageManager::getChat(qlonglong chatId) noexcept
{
    return getPointer(m_chats, chatId);
}

td::td_api::file *StorageManager::getFile(int fileId) noexcept
{
    return getPointer(m_files, fileId);
}

QVariant StorageManager::getOption(const QString &name) const noexcept
{
    if (auto it = m_options.find(name); it != m_options.end())
        return it.value();

    return QVariant();
}

td::td_api::supergroup *StorageManager::getSupergroup(qlonglong groupId) noexcept
{
    return getPointer(m_supergroup, groupId);
}

td::td_api::supergroupFullInfo *StorageManager::getSupergroupFullInfo(qlonglong groupId) noexcept
{
    return getPointer(m_supergroupFullInfo, groupId);
}

td::td_api::user *StorageManager::getUser(qlonglong userId) noexcept
{
    return getPointer(m_users, userId);
}

td::td_api::userFullInfo *StorageManager::getUserFullInfo(qlonglong userId) noexcept
{
    return getPointer(m_userFullInfo, userId);
}

const std::vector<const td::td_api::chatFolderInfo *> &StorageManager::chatFolders() const noexcept
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
        *object,
        detail::Overloaded{
            [this, object](td::td_api::updateNewChat &value) {
                m_chats.insert_or_assign(value.chat_->id_, std::move(value.chat_));
                emit chatsUpdated(object);
            },
            [this, object](td::td_api::updateChatTitle &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->title_ = value.title_;

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatPhoto &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->photo_ = std::move(value.photo_);
                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatPermissions &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->permissions_ = std::move(value.permissions_);
                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatLastMessage &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->last_message_ = std::move(value.last_message_);

                    setChatPositions(value.chat_id_, std::move(value.positions_));

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatPosition &value) {
                std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> result;
                result.emplace_back(std::move(value.position_));

                setChatPositions(value.chat_id_, std::move(result));

                emit chatsUpdated(object);
            },
            [this, object](td::td_api::updateChatReadInbox &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->last_read_inbox_message_id_ = value.last_read_inbox_message_id_;
                    it->second->unread_count_ = value.unread_count_;

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatReadOutbox &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->last_read_outbox_message_id_ = value.last_read_outbox_message_id_;

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatActionBar &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->action_bar_ = std::move(value.action_bar_);

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatDraftMessage &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->draft_message_ = std::move(value.draft_message_);

                    setChatPositions(value.chat_id_, std::move(value.positions_));

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatNotificationSettings &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->notification_settings_ = std::move(value.notification_settings_);

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatReplyMarkup &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->reply_markup_message_id_ = value.reply_markup_message_id_;

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatUnreadMentionCount &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->unread_mention_count_ = value.unread_mention_count_;

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateChatIsMarkedAsUnread &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->is_marked_as_unread_ = value.is_marked_as_unread_;

                    emit chatsUpdated(object);
                }
            },
            [this, object](td::td_api::updateUser &value) {
                auto userId = value.user_->id_;

                m_users.insert_or_assign(userId, std::move(value.user_));
                emit usersUpdated(userId, object);
            },
            [this, object](td::td_api::updateBasicGroup &value) {
                auto groupId = value.basic_group_->id_;

                m_basicGroup.insert_or_assign(groupId, std::move(value.basic_group_));
                emit basicGroupUpdated(groupId, object);
            },
            [this, object](td::td_api::updateSupergroup &value) {
                auto groupId = value.supergroup_->id_;

                m_supergroup.insert_or_assign(groupId, std::move(value.supergroup_));
                emit supergroupUpdated(groupId, object);
            },
            [this, object](td::td_api::updateUserFullInfo &value) {
                auto userId = value.user_id_;

                m_userFullInfo.insert_or_assign(userId, std::move(value.user_full_info_));
                emit userFullInfoUpdated(userId, object);
            },
            [this, object](td::td_api::updateBasicGroupFullInfo &value) {
                auto groupId = value.basic_group_id_;

                m_basicGroupFullInfo.insert_or_assign(groupId, std::move(value.basic_group_full_info_));
                emit basicGroupFullInfoUpdated(groupId, object);
            },
            [this, object](td::td_api::updateSupergroupFullInfo &value) {
                auto groupId = value.supergroup_id_;

                m_supergroupFullInfo.insert_or_assign(groupId, std::move(value.supergroup_full_info_));
                emit supergroupFullInfoUpdated(groupId, object);
            },
            [this, object](td::td_api::updateChatFolders &value) {
                m_chatFolders.reserve(value.chat_folders_.size());
                std::ranges::transform(value.chat_folders_, std::back_inserter(m_chatFolders), [](const auto &chatFolder) { return chatFolder.get(); });

                emit chatFoldersUpdated(object);
            },
            [this, object](td::td_api::updateFile &value) {
                auto fileId = value.file_->id_;

                m_files.insert_or_assign(fileId, std::move(value.file_));

                emit fileUpdated(fileId, object);
            },
            [this](td::td_api::updateOption &value) {
                auto optionValue = [](td::td_api::object_ptr<td::td_api::OptionValue> &&option) -> QVariant {
                    switch (option->get_id()) {
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

void StorageManager::setChatPositions(qlonglong chatId, std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> &&positions) noexcept
{
    auto it = m_chats.find(chatId);
    if (it == m_chats.end() || positions.empty())
    {
        return;
    }

    auto &currentPositions = it->second->positions_;

    // Reserve capacity if known or estimated
    currentPositions.reserve(currentPositions.size() + positions.size());

    ChatListComparator comparator;

    for (auto &&position : positions)
    {
        // Remove existing positions that match the new position
        std::erase_if(currentPositions, [&](const auto &value) { return comparator(value->list_, position->list_); });

        // Add new position
        currentPositions.emplace_back(std::move(position));
    }
}
