#include "StorageManager.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Utils.hpp"

#include <QDebug>

#include <algorithm>
#include <ranges>
#include <variant>

StorageManager::StorageManager(Client *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
}

Client *StorageManager::client() const noexcept
{
    return m_client;
}

std::vector<qint64> StorageManager::getChatIds() const noexcept
{
    auto ids = m_chats | std::views::keys;
    std::vector<qint64> result(ids.begin(), ids.end());
    return result;
}

const td::td_api::basicGroup *StorageManager::getBasicGroup(qint64 groupId) const
{
    if (auto it = m_basicGroup.find(groupId); it != m_basicGroup.end())
    {
        return it->second.get();
    }

    return nullptr;
}

const td::td_api::basicGroupFullInfo *StorageManager::getBasicGroupFullInfo(qint64 groupId) const
{
    if (auto it = m_basicGroupFullInfo.find(groupId); it != m_basicGroupFullInfo.end())
    {
        return it->second.get();
    }

    return nullptr;
}

const td::td_api::chat *StorageManager::getChat(qint64 chatId) const
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        return it->second.get();
    }

    return nullptr;
}

const td::td_api::file *StorageManager::getFile(qint32 fileId) const
{
    if (auto it = m_files.find(fileId); it != m_files.end())
    {
        return it->second.get();
    }

    return nullptr;
}

QVariant StorageManager::getOption(const QString &name) const
{
    if (auto it = m_options.find(name); it != m_options.end())
        return it.value();

    return QVariant();
}

const td::td_api::supergroup *StorageManager::getSupergroup(qint64 groupId) const
{
    if (auto it = m_supergroup.find(groupId); it != m_supergroup.end())
    {
        return it->second.get();
    }

    return nullptr;
}

const td::td_api::supergroupFullInfo *StorageManager::getSupergroupFullInfo(qint64 groupId) const
{
    if (auto it = m_supergroupFullInfo.find(groupId); it != m_supergroupFullInfo.end())
    {
        return it->second.get();
    }

    return nullptr;
}

const td::td_api::user *StorageManager::getUser(qint64 userId) const
{
    if (auto it = m_users.find(userId); it != m_users.end())
    {
        return it->second.get();
    }

    return nullptr;
}

const td::td_api::userFullInfo *StorageManager::getUserFullInfo(qint64 userId) const
{
    if (auto it = m_userFullInfo.find(userId); it != m_userFullInfo.end())
    {
        return it->second.get();
    }

    return nullptr;
}

qint64 StorageManager::getMyId() const noexcept
{
    if (const auto myId = getOption("my_id"); not myId.isNull())
        return myId.toLongLong();

    return 0;
}

void StorageManager::handleResult(td::td_api::Object *object)
{
    td::td_api::downcast_call(
        *object,
        detail::Overloaded{
            [this](td::td_api::updateNewChat &value) { m_chats.emplace(value.chat_->id_, std::move(value.chat_)); },
            [this](td::td_api::updateChatTitle &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->title_ = value.title_;
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatPhoto &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->photo_ = std::move(value.photo_);
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatPermissions &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->permissions_ = std::move(value.permissions_);
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatLastMessage &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->last_message_ = std::move(value.last_message_);
                    emit updateChatItem(value.chat_id_);
                }

                setChatPositions(value.chat_id_, std::move(value.positions_));
            },
            [this](td::td_api::updateChatPosition &value) {
                std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> result;
                result.emplace_back(std::move(value.position_));
                setChatPositions(value.chat_id_, std::move(result));
            },
            [this](td::td_api::updateChatReadInbox &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->last_read_inbox_message_id_ = value.last_read_inbox_message_id_;
                    it->second->unread_count_ = value.unread_count_;
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatReadOutbox &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->last_read_outbox_message_id_ = value.last_read_outbox_message_id_;
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatActionBar &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->action_bar_ = std::move(value.action_bar_);
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatDraftMessage &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->draft_message_ = std::move(value.draft_message_);
                    emit updateChatItem(value.chat_id_);
                }

                setChatPositions(value.chat_id_, std::move(value.positions_));
            },
            [this](td::td_api::updateChatNotificationSettings &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->notification_settings_ = std::move(value.notification_settings_);
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatReplyMarkup &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->reply_markup_message_id_ = value.reply_markup_message_id_;
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatUnreadMentionCount &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->unread_mention_count_ = value.unread_mention_count_;
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateChatIsMarkedAsUnread &value) {
                if (auto it = m_chats.find(value.chat_id_); it != m_chats.end())
                {
                    it->second->is_marked_as_unread_ = value.is_marked_as_unread_;
                    emit updateChatItem(value.chat_id_);
                }
            },
            [this](td::td_api::updateUser &value) { m_users.emplace(value.user_->id_, std::move(value.user_)); },
            [this](td::td_api::updateBasicGroup &value) { m_basicGroup.emplace(value.basic_group_->id_, std::move(value.basic_group_)); },
            [this](td::td_api::updateSupergroup &value) { m_supergroup.emplace(value.supergroup_->id_, std::move(value.supergroup_)); },
            [this](td::td_api::updateUserFullInfo &value) { m_userFullInfo.emplace(value.user_id_, std::move(value.user_full_info_)); },
            [this](td::td_api::updateBasicGroupFullInfo &value) {
                m_basicGroupFullInfo.emplace(value.basic_group_id_, std::move(value.basic_group_full_info_));
            },
            [this](td::td_api::updateSupergroupFullInfo &value) { m_supergroupFullInfo.emplace(value.supergroup_id_, std::move(value.supergroup_full_info_)); },
            [this](td::td_api::updateFile &value) { m_files.emplace(value.file_->id_, std::move(value.file_)); },
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

    emit updateChatPosition(chatId);
}
