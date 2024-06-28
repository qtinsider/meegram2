#include "StorageManager.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Utils.hpp"

#include <QDebug>

#include <algorithm>

StorageManager::StorageManager(QObject *parent)
    : QObject(parent)
{
}

Client *StorageManager::client() const
{
    return m_client;
}

void StorageManager::setClient(Client *client)
{
    m_client = client;

    connect(m_client, SIGNAL(result(const QVariantMap &)), this, SLOT(handleResult(const QVariantMap &)));
}

QVector<qint64> StorageManager::getChatIds() const noexcept
{
    QVector<qint64> result;

    result.reserve(m_chats.size());

    std::ranges::transform(m_chats, std::back_inserter(result), [](const auto &value) { return value.first; });
    return result;
}

QVariantMap StorageManager::getBasicGroup(qint64 groupId) const
{
    if (auto it = m_basicGroup.find(groupId); it != m_basicGroup.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getBasicGroupFullInfo(qint64 groupId) const
{
    if (auto it = m_basicGroupFullInfo.find(groupId); it != m_basicGroupFullInfo.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getChat(qint64 chatId) const
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getFile(qint32 fileId) const
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

QVariantMap StorageManager::getSupergroup(qint64 groupId) const
{
    if (auto it = m_supergroup.find(groupId); it != m_supergroup.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getSupergroupFullInfo(qint64 groupId) const
{
    if (auto it = m_supergroupFullInfo.find(groupId); it != m_supergroupFullInfo.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getUser(qint64 userId) const
{
    if (auto it = m_users.find(userId); it != m_users.end())
    {
        return it->second;
    }

    return {};
}

QVariantMap StorageManager::getUserFullInfo(qint64 userId) const
{
    if (auto it = m_userFullInfo.find(userId); it != m_userFullInfo.end())
    {
        return it->second;
    }

    return {};
}

QVariantList StorageManager::getChatFilters() const noexcept
{
    return m_chatFilters;
}

qint64 StorageManager::getMyId() const
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
             const auto chat = object.value("chat").toMap();
             m_chats.emplace(chat.value("id").toLongLong(), chat);
         }},
        {"updateChatTitle",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("title", object.value("title").toString());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatPhoto",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("photo", object.value("photo").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatPermissions",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("permissions", object.value("permissions").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatLastMessage",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             const auto positions = object.value("positions").toList();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("last_message", object.value("last_message").toMap());
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
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("last_read_inbox_message_id", object.value("last_read_inbox_message_id").toLongLong());
                 it->second.insert("unread_count", object.value("unread_count").toInt());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatReadOutbox",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("last_read_outbox_message_id", object.value("last_read_outbox_message_id").toLongLong());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatActionBar",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("action_bar", object.value("action_bar").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatDraftMessage",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             const auto positions = object.value("positions").toList();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("draft_message", object.value("draft_message").toMap());
                 if (!positions.isEmpty())
                     emit updateChatPosition(chatId);
                 emit updateChatItem(chatId);
             }
             setChatPositions(chatId, positions);
         }},
        {"updateChatNotificationSettings",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("notification_settings", object.value("notification_settings").toMap());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatReplyMarkup",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("reply_markup_message_id", object.value("reply_markup_message_id").toLongLong());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatUnreadMentionCount",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("unread_mention_count", object.value("unread_mention_count").toInt());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatIsMarkedAsUnread",
         [this](const QVariantMap &object) {
             const auto chatId = object.value("chat_id").toLongLong();
             auto it = m_chats.find(chatId);
             if (it != m_chats.end())
             {
                 it->second.insert("is_marked_as_unread", object.value("is_marked_as_unread").toBool());
                 emit updateChatItem(chatId);
             }
         }},
        {"updateChatFilters",
         [this](const QVariantMap &object) {
             m_chatFilters = object.value("chat_filters").toList();
             qDebug() << m_chatFilters;
         }},
        {"updateUser",
         [this](const QVariantMap &object) {
             const auto user = object.value("user").toMap();
             m_users.emplace(user.value("id").toLongLong(), user);
         }},
        {"updateBasicGroup",
         [this](const QVariantMap &object) {
             const auto basicGroup = object.value("basic_group").toMap();
             m_basicGroup.emplace(basicGroup.value("id").toLongLong(), basicGroup);
         }},
        {"updateSupergroup",
         [this](const QVariantMap &object) {
             const auto supergroup = object.value("supergroup").toMap();
             m_supergroup.emplace(supergroup.value("id").toLongLong(), supergroup);
         }},
        {"updateUserFullInfo",
         [this](const QVariantMap &object) {
             const auto userId = object.value("user_id").toLongLong();
             m_userFullInfo.emplace(userId, object.value("user_full_info").toMap());
         }},
        {"updateBasicGroupFullInfo",
         [this](const QVariantMap &object) {
             const auto basicGroupId = object.value("basic_group_id").toLongLong();
             m_basicGroupFullInfo.emplace(basicGroupId, object.value("basic_group_full_info").toMap());
         }},
        {"updateSupergroupFullInfo",
         [this](const QVariantMap &object) {
             const auto supergroupId = object.value("supergroup_id").toLongLong();
             m_supergroupFullInfo.emplace(supergroupId, object.value("supergroup_full_info").toMap());
         }},
        {"updateFile",
         [this](const QVariantMap &object) {
             const auto file = object.value("file").toMap();
             m_files.emplace(file.value("id").toInt(), file);
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

    auto currentPositions = it->second.value("positions").toList();  // Use a local variable to hold the list

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
    it->second.insert("positions", currentPositions);
}
