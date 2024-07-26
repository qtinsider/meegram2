#include "StorageManager.hpp"

#include "Common.hpp"
#include "Utils.hpp"

#include <QDebug>

#include <algorithm>
#include <ranges>

StorageManager::StorageManager(Client *client, Locale *locale, QObject *parent)
    : QObject(parent)
    , m_client(client)
    , m_locale(locale)
{
    connect(m_client, SIGNAL(result(const QVariantMap &)), this, SLOT(handleResult(const QVariantMap &)));
}

Client *StorageManager::client() const noexcept
{
    return m_client;
}

Locale *StorageManager::locale() const noexcept
{
    return m_locale;
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
        return it->second.get();
    }

    return nullptr;
}

BasicGroupFullInfo *StorageManager::getBasicGroupFullInfo(qint64 groupId) const
{
    if (auto it = m_basicGroupFullInfo.find(groupId); it != m_basicGroupFullInfo.end())
    {
        return it->second.get();
    }

    return nullptr;
}

Chat *StorageManager::getChat(qint64 chatId) const
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        return it->second.get();
    }

    return nullptr;
}

File *StorageManager::getFile(qint32 fileId) const
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

Supergroup *StorageManager::getSupergroup(qint64 groupId) const
{
    if (auto it = m_supergroup.find(groupId); it != m_supergroup.end())
    {
        return it->second.get();
    }

    return nullptr;
}

SupergroupFullInfo *StorageManager::getSupergroupFullInfo(qint64 groupId) const
{
    if (auto it = m_supergroupFullInfo.find(groupId); it != m_supergroupFullInfo.end())
    {
        return it->second.get();
    }

    return nullptr;
}

User *StorageManager::getUser(qint64 userId) const
{
    if (auto it = m_users.find(userId); it != m_users.end())
    {
        return it->second.get();
    }

    return nullptr;
}

UserFullInfo *StorageManager::getUserFullInfo(qint64 userId) const
{
    if (auto it = m_userFullInfo.find(userId); it != m_userFullInfo.end())
    {
        return it->second.get();
    }

    return nullptr;
}

const QVariantList &StorageManager::chatFolders() const noexcept
{
    return m_chatFolders;
}

const QVariantList &StorageManager::countries() const noexcept
{
    return m_countries;
}

const QVariantList &StorageManager::languagePackInfo() const noexcept
{
    return m_languagePackInfo;
}

void StorageManager::setCountries(const QVariantList &value) noexcept
{
    if (m_countries != value)
    {
        m_countries = value;

        emit countriesChanged();
    }
}

void StorageManager::setLanguagePackInfo(const QVariantList &value) noexcept
{
    if (m_languagePackInfo != value)
    {
        m_languagePackInfo = value;

        emit languagePackInfoChanged();
    }
}

qint64 StorageManager::getMyId() const noexcept
{
    if (const auto myId = getOption("my_id"); not myId.isNull())
        return myId.toLongLong();

    return 0;
}

void StorageManager::handleResult(const QVariantMap &object)
{
    static const std::unordered_map<QString, std::function<void(const QVariantMap &)>> handlers = {
        {"updateNewChat",
         [this](const QVariantMap &object) {
             auto chat = std::make_unique<Chat>();
             chat->setFromVariantMap(object.value("chat").toMap());
             m_chats.emplace(chat->id(), std::move(chat));
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
                 auto message = std::make_unique<Message>();
                 message->setFromVariantMap(object.value("last_message").toMap());
                 it->second->setLastMessage(message.release());  // Release ownership
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
                 auto draftMessage = std::make_unique<Message>();
                 draftMessage->setFromVariantMap(object.value("draft_message").toMap());
                 it->second->setDraftMessage(draftMessage.release());  // Release ownership
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
             auto user = std::make_unique<User>();
             user->setFromVariantMap(object.value("user").toMap());
             m_users.emplace(user->id(), std::move(user));
         }},
        {"updateBasicGroup",
         [this](const QVariantMap &object) {
             auto basicGroup = std::make_unique<BasicGroup>();
             basicGroup->setFromVariantMap(object.value("basic_group").toMap());
             m_basicGroup.emplace(basicGroup->id(), std::move(basicGroup));
         }},
        {"updateSupergroup",
         [this](const QVariantMap &object) {
             auto supergroup = std::make_unique<Supergroup>();
             supergroup->setFromVariantMap(object.value("supergroup").toMap());
             m_supergroup.emplace(supergroup->id(), std::move(supergroup));
         }},
        {"updateUserFullInfo",
         [this](const QVariantMap &object) {
             auto userFullInfo = std::make_unique<UserFullInfo>();
             userFullInfo->setFromVariantMap(object.value("user_full_info").toMap());
             m_userFullInfo.emplace(object.value("user_id").toLongLong(), std::move(userFullInfo));
         }},
        {"updateBasicGroupFullInfo",
         [this](const QVariantMap &object) {
             auto basicGroupFullInfo = std::make_unique<BasicGroupFullInfo>();
             basicGroupFullInfo->setFromVariantMap(object.value("basic_group_full_info").toMap());
             m_basicGroupFullInfo.emplace(object.value("basic_group_id").toLongLong(), std::move(basicGroupFullInfo));
         }},
        {"updateSupergroupFullInfo",
         [this](const QVariantMap &object) {
             auto supergroupFullInfo = std::make_unique<SupergroupFullInfo>();
             supergroupFullInfo->setFromVariantMap(object.value("supergroup_full_info").toMap());
             m_supergroupFullInfo.emplace(object.value("supergroup_id").toLongLong(), std::move(supergroupFullInfo));
         }},
        {"updateFile",
         [this](const QVariantMap &object) {
             auto file = std::make_unique<File>();
             file->setFromVariantMap(object.value("file").toMap());
             m_files.emplace(file->id(), std::move(file));
         }},
        {"updateOption",
         [this](const QVariantMap &object) { m_options.insert(object.value("name").toString(), object.value("value").toMap().value("value")); }}};

    if (auto it = handlers.find(object.value("@type").toString()); it != handlers.end())
    {
        it->second(object);
    }
}

void StorageManager::setChatPositions(qint64 chatId, const QVariantList &positions) noexcept
{
    if (auto it = m_chats.find(chatId); it == m_chats.end())
    {
        return;  // Early return if chatId is not found
    }
    else
    {
        auto currentPositions = it->second->positions();

        for (const auto &position : positions)
        {
            auto removeIt = std::ranges::find_if(
                currentPositions, [&](const auto &value) { return Utils::chatListEquals(value.toMap()["list"].toMap(), position.toMap()["list"].toMap()); });

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
}
