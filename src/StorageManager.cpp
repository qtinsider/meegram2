#include "StorageManager.hpp"

#include "BasicGroup.hpp"
#include "BasicGroupFullInfo.hpp"
#include "Chat.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "File.hpp"
#include "Message.hpp"
#include "Supergroup.hpp"
#include "SupergroupFullInfo.hpp"
#include "User.hpp"
#include "UserFullInfo.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <ranges>

StorageManager::StorageManager(QObject *parent)
    : QObject(parent)
    , m_client(new Client(this))
{
    connect(m_client, SIGNAL(result(const QVariantMap &)), this, SLOT(handleResult(const QVariantMap &)));
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

BasicGroup *StorageManager::getBasicGroup(qint64 groupId) const
{
    if (auto it = m_basicGroup.find(groupId); it != m_basicGroup.end())
    {
        return it->second;
    }

    return {};
}

BasicGroupFullInfo *StorageManager::getBasicGroupFullInfo(qint64 groupId) const
{
    if (auto it = m_basicGroupFullInfo.find(groupId); it != m_basicGroupFullInfo.end())
    {
        return it->second;
    }

    return {};
}

Chat *StorageManager::getChat(qint64 chatId) const
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        return it->second;
    }

    return {};
}

File *StorageManager::getFile(qint32 fileId) const
{
    if (auto it = m_files.find(fileId); it != m_files.end())
    {
        return it->second;
    }

    return {};
}

QVariant StorageManager::getOption(const QString &name) const
{
    if (auto it = m_options.find(name); it != m_options.end())
        return it.value();

    return {};
}

Supergroup *StorageManager::getSupergroup(qint64 groupId) const
{
    if (auto it = m_supergroup.find(groupId); it != m_supergroup.end())
    {
        return it->second;
    }

    return {};
}

SupergroupFullInfo *StorageManager::getSupergroupFullInfo(qint64 groupId) const
{
    if (auto it = m_supergroupFullInfo.find(groupId); it != m_supergroupFullInfo.end())
    {
        return it->second;
    }

    return {};
}

User *StorageManager::getUser(qint64 userId) const
{
    if (auto it = m_users.find(userId); it != m_users.end())
    {
        return it->second;
    }

    return {};
}

UserFullInfo *StorageManager::getUserFullInfo(qint64 userId) const
{
    if (auto it = m_userFullInfo.find(userId); it != m_userFullInfo.end())
    {
        return it->second;
    }

    return {};
}

QVariantList StorageManager::getChatFolders() const noexcept
{
    return m_chatFolders;
}

qint64 StorageManager::getMyId() const noexcept
{
    if (const auto myId = getOption("my_id"); not myId.isNull())
        return myId.toLongLong();

    return {};
}

void StorageManager::handleResult(const QVariantMap &object)
{
    static const std::unordered_map<std::string, std::function<void(const QVariantMap &)>> handlers = {
        {"updateNewChat",
         [this](const QVariantMap &object) {
             Chat *chat = new Chat(this);

             chat->setFromVariantMap(object.value("chat").toMap());

             m_chats.emplace(chat->id(), chat);
         }},
        {"updateChatTitle",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setTitle(object.value("title").toString());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatPhoto",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setPhoto(object.value("photo").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatPermissions",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setPermissions(object.value("permissions").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatLastMessage",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             const auto positions = object.value("positions").toList();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 Message *message = new Message(this);

                 message->setFromVariantMap(object.value("last_message").toMap());

                 it->second->setLastMessage(message);
                 if (!positions.isEmpty())
                     emit updateChatPosition(chatId);
                 emit updateChatItem(chatId);
             }
             setChatPositions(chatId, positions);
         }},
        {"updateChatPosition",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             const auto position = QVariantList() << object.value("position").toMap();
             setChatPositions(chatId, position);
         }},
        {"updateChatReadInbox",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setLastReadInboxMessageId(object.value("last_read_inbox_message_id").toLongLong());
                 it->second->setUnreadCount(object.value("unread_count").toInt());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatReadOutbox",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setLastReadOutboxMessageId(object.value("last_read_outbox_message_id").toLongLong());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatActionBar",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setActionBar(object.value("action_bar").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatDraftMessage",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             const auto positions = object.value("positions").toList();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 Message *draftMessage = new Message(this);

                 draftMessage->setFromVariantMap(object.value("draft_message").toMap());

                 it->second->setDraftMessage(draftMessage);
                 if (!positions.isEmpty())
                     emit updateChatPosition(chatId);
                 emit updateChatItem(chatId);
             }
             setChatPositions(chatId, positions);
         }},
        {"updateChatNotificationSettings",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setNotificationSettings(object.value("notification_settings").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatReplyMarkup",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setReplyMarkupMessageId(object.value("reply_markup_message_id").toLongLong());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatUnreadMentionCount",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setUnreadMentionCount(object.value("unread_mention_count").toInt());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatIsMarkedAsUnread",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();

             if (auto it = m_chats.find(chatId); it != m_chats.end())
             {
                 it->second->setIsMarkedAsUnread(object.value("is_marked_as_unread").toBool());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatFolders",
         [this](const QVariantMap &object) {
             m_chatFolders.append(object.value("chat_folders").toList());
             emit chatFoldersChanged();
         }},
        {"updateUser",
         [this](const QVariantMap &object) {
             User *user = new User(this);
             user->setFromVariantMap(object.value("user").toMap());

             m_users.emplace(user->id(), user);
         }},
        {"updateBasicGroup",
         [this](const QVariantMap &object) {
             BasicGroup *basicGroup = new BasicGroup(this);
             basicGroup->setFromVariantMap(object.value("basic_group").toMap());

             m_basicGroup.emplace(basicGroup->id(), basicGroup);
         }},
        {"updateSupergroup",
         [this](const QVariantMap &object) {
             Supergroup *supergroup = new Supergroup(this);
             supergroup->setFromVariantMap(object.value("supergroup").toMap());

             m_supergroup.emplace(supergroup->id(), supergroup);
         }},
        {"updateUserFullInfo",
         [this](const QVariantMap &object) {
             UserFullInfo *userFullInfo = new UserFullInfo(this);
             userFullInfo->setFromVariantMap(object.value("user_full_info").toMap());

             m_userFullInfo.emplace(object.value("user_id").toLongLong(), userFullInfo);
         }},
        {"updateBasicGroupFullInfo",
         [this](const QVariantMap &object) {
             BasicGroupFullInfo *basicGroupFullInfo = new BasicGroupFullInfo(this);
             basicGroupFullInfo->setFromVariantMap(object.value("basic_group_full_info").toMap());

             m_basicGroupFullInfo.emplace(object.value("basic_group_id").toLongLong(), basicGroupFullInfo);
         }},
        {"updateSupergroupFullInfo",
         [this](const QVariantMap &object) {
             SupergroupFullInfo *supergroupFullInfo = new SupergroupFullInfo(this);
             supergroupFullInfo->setFromVariantMap(object.value("supergroup_full_info").toMap());

             m_supergroupFullInfo.emplace(object.value("supergroup_id").toLongLong(), supergroupFullInfo);
         }},
        {"updateFile",
         [this](const QVariantMap &object) {
             File *file = new File(this);
             file->setFromVariantMap(object.value("file").toMap());

             m_files.emplace(file->id(), file);
         }},
        {"updateOption", [this](const QVariantMap &object) {
             m_options.insert(object.value("name").toString(), object.value("value").toMap().value("value"));
         }}};

    const auto objectType = object.value("@type").toString().toStdString();

    if (auto it = handlers.find(objectType); it != handlers.end())
    {
        it->second(object);
    }
}

void StorageManager::setChatPositions(qint64 chatId, const QVariantList &positions) noexcept
{
    auto it = m_chats.find(chatId);
    if (it == m_chats.end())
    {
        return;  // Early return if chatId is not found
    }

    auto currentPositions = it->second->positions();  // Use a local variable to hold the list

    // Update or append positions
    for (const auto &position : positions)
    {
        auto removeIt = std::ranges::find_if(currentPositions, [&](const auto &value) {
            return Utils::chatListEquals(value.toMap()["list"].toMap(), position.toMap()["list"].toMap());
        });

        if (removeIt != currentPositions.end())
        {
            *removeIt = position;  // Update existing position
        }
        else
        {
            currentPositions.push_back(position);  // Add new position
        }
    }

    // Update positions in m_chats
    it->second->setPositions(currentPositions);
}
