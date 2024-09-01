#include "StorageManager.hpp"

#include "Common.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <ranges>

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

td::td_api::basicGroup *StorageManager::getBasicGroup(qint64 groupId) noexcept
{
    return getPointer(m_basicGroup, groupId);
}

td::td_api::basicGroupFullInfo *StorageManager::getBasicGroupFullInfo(qint64 groupId) noexcept
{
    return getPointer(m_basicGroupFullInfo, groupId);
}

td::td_api::chat *StorageManager::getChat(qint64 chatId) noexcept
{
    return getPointer(m_chats, chatId);
}

td::td_api::file *StorageManager::getFile(qint32 fileId) noexcept
{
    return getPointer(m_files, fileId);
}

QVariant StorageManager::getOption(const QString &name) const noexcept
{
    if (auto it = m_options.find(name); it != m_options.end())
        return it.value();

    return QVariant();
}

td::td_api::supergroup *StorageManager::getSupergroup(qint64 groupId) noexcept
{
    return getPointer(m_supergroup, groupId);
}

td::td_api::supergroupFullInfo *StorageManager::getSupergroupFullInfo(qint64 groupId) noexcept
{
    return getPointer(m_supergroupFullInfo, groupId);
}

td::td_api::user *StorageManager::getUser(qint64 userId) noexcept
{
    return getPointer(m_users, userId);
}

td::td_api::userFullInfo *StorageManager::getUserFullInfo(qint64 userId) noexcept
{
    return getPointer(m_userFullInfo, userId);
}

const std::vector<const td::td_api::chatFolderInfo *> &StorageManager::chatFolders() const noexcept
{
    return m_chatFolders;
}

qint64 StorageManager::myId() const noexcept
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
                m_chats.emplace(value.chat_->id_, std::move(value.chat_));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateChatTitle &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, const auto &v) { chat->title_ = v.title_; });
            },
            [this, object](td::td_api::updateChatPhoto &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, auto &&v) { chat->photo_ = std::move(v.photo_); });
            },
            [this, object](td::td_api::updateChatPermissions &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, auto &&v) { chat->permissions_ = std::move(v.permissions_); });
            },
            [this, object](td::td_api::updateChatLastMessage &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->last_message_ = std::move(value.last_message_);
                    setChatPositions(value.chat_id_, std::move(value.positions_));
                    emit dataChanged(object);
                }
            },
            [this, object](td::td_api::updateChatPosition &value) {
                std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> result;
                result.emplace_back(std::move(value.position_));
                setChatPositions(value.chat_id_, std::move(result));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateChatReadInbox &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, const auto &v) {
                    chat->last_read_inbox_message_id_ = v.last_read_inbox_message_id_;
                    chat->unread_count_ = v.unread_count_;
                });
            },
            [this, object](td::td_api::updateChatReadOutbox &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, const auto &v) { chat->last_read_outbox_message_id_ = v.last_read_outbox_message_id_; });
            },
            [this, object](td::td_api::updateChatActionBar &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, auto &&v) { chat->action_bar_ = std::move(v.action_bar_); });
            },
            [this, object](td::td_api::updateChatDraftMessage &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->draft_message_ = std::move(value.draft_message_);
                    setChatPositions(value.chat_id_, std::move(value.positions_));
                    emit dataChanged(object);
                }
            },
            [this, object](td::td_api::updateChatNotificationSettings &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, auto &&v) { chat->notification_settings_ = std::move(v.notification_settings_); });
            },
            [this, object](td::td_api::updateChatReplyMarkup &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, const auto &v) { chat->reply_markup_message_id_ = v.reply_markup_message_id_; });
            },
            [this, object](td::td_api::updateChatUnreadMentionCount &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, const auto &v) { chat->unread_mention_count_ = v.unread_mention_count_; });
            },
            [this, object](td::td_api::updateChatIsMarkedAsUnread &value) {
                updateAndEmit(object, m_chats, value, [](auto &chat, const auto &v) { chat->is_marked_as_unread_ = v.is_marked_as_unread_; });
            },
            [this, object](td::td_api::updateUser &value) {
                m_users.emplace(value.user_->id_, std::move(value.user_));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateBasicGroup &value) {
                m_basicGroup.emplace(value.basic_group_->id_, std::move(value.basic_group_));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateSupergroup &value) {
                m_supergroup.emplace(value.supergroup_->id_, std::move(value.supergroup_));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateUserFullInfo &value) {
                m_userFullInfo.emplace(value.user_id_, std::move(value.user_full_info_));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateBasicGroupFullInfo &value) {
                m_basicGroupFullInfo.emplace(value.basic_group_id_, std::move(value.basic_group_full_info_));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateSupergroupFullInfo &value) {
                m_supergroupFullInfo.emplace(value.supergroup_id_, std::move(value.supergroup_full_info_));
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateChatFolders &value) {
                m_chatFolders.reserve(value.chat_folders_.size());
                std::ranges::transform(value.chat_folders_, std::back_inserter(m_chatFolders), [](const auto &chatFolder) { return chatFolder.get(); });
                emit chatFoldersChanged();
                emit dataChanged(object);
            },
            [this, object](td::td_api::updateFile &value) {
                m_files.emplace(value.file_->id_, std::move(value.file_));
                emit dataChanged(object);
            },
            [this](td::td_api::updateOption &) {},
            [](auto &) {}});
}

void StorageManager::setChatPositions(qint64 chatId, std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> &&positions) noexcept
{
    auto it = m_chats.find(chatId);
    if (it == m_chats.end())
    {
        return;  // Early return if chatId is not found
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
