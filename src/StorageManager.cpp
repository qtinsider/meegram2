#include "StorageManager.hpp"

#include "Client.hpp"
#include "Common.hpp"

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
    switch (const auto objectType = object.value("@type").toByteArray(); fnv::hashRuntime(objectType.constData()))
    {
        case fnv::hash("updateNewChat"): {
            m_chats.emplace(object.value("chat").toMap().value("id").toLongLong(), object.value("chat").toMap());

            break;
        }
        case fnv::hash("updateChatTitle"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(object.value("chat_id").toLongLong()); it != m_chats.end())
            {
                it->second.insert("title", object.value("title").toString());

                emit updateChatItem(chatId);
            }

            break;
        }
        case fnv::hash("updateChatPhoto"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("photo", object.value("photo").toMap());

                emit updateChatItem(chatId);
            }
            break;
        }
        case fnv::hash("updateChatPermissions"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("permissions", object.value("permissions").toMap());

                emit updateChatItem(chatId);
            }

            break;
        }
        case fnv::hash("updateChatLastMessage"): {
            const auto chatId = object.value("chat_id").toLongLong();
            const auto positions = object.value("positions").toList();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("last_message", object.value("last_message").toMap());

                if (not positions.isEmpty())
                    emit updateChatPosition(chatId);

                emit updateChatItem(chatId);
            }

            setChatPositions(chatId, positions);

            break;
        }
        case fnv::hash("updateChatPosition"): {
            setChatPositions(object.value("chat_id").toLongLong(), QVariantList() << object.value("position").toMap());

            break;
        }
        case fnv::hash("updateChatReadInbox"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("last_read_inbox_message_id", object.value("last_read_inbox_message_id").toLongLong());
                it->second.insert("unread_count", object.value("unread_count").toInt());

                emit updateChatItem(chatId);
            }

            break;
        }
        case fnv::hash("updateChatReadOutbox"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("last_read_outbox_message_id", object.value("last_read_outbox_message_id").toLongLong());

                emit updateChatItem(chatId);
            }
            break;
        }
        case fnv::hash("updateChatActionBar"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("action_bar", object.value("action_bar").toMap());

                emit updateChatItem(chatId);
            }

            break;
        }
        case fnv::hash("updateChatDraftMessage"): {
            const auto chatId = object.value("chat_id").toLongLong();
            const auto positions = object.value("positions").toList();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("draft_message", object.value("draft_message").toMap());

                if (not positions.isEmpty())
                    emit updateChatPosition(chatId);

                emit updateChatItem(chatId);
            }

            setChatPositions(chatId, positions);

            break;
        }
        case fnv::hash("updateChatNotificationSettings"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("notification_settings", object.value("notification_settings").toMap());

                emit updateChatItem(chatId);
            }
            break;
        }
        case fnv::hash("updateChatReplyMarkup"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("reply_markup_message_id", object.value("reply_markup_message_id").toLongLong());

                emit updateChatItem(chatId);
            }

            break;
        }
        case fnv::hash("updateChatUnreadMentionCount"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("unread_mention_count", object.value("unread_mention_count").toInt());

                emit updateChatItem(chatId);
            }

            break;
        }
        case fnv::hash("updateChatIsMarkedAsUnread"): {
            const auto chatId = object.value("chat_id").toLongLong();

            if (auto it = m_chats.find(chatId); it != m_chats.end())
            {
                it->second.insert("is_marked_as_unread", object.value("is_marked_as_unread").toBool());

                emit updateChatItem(chatId);
            }
            break;
        }

        case fnv::hash("updateChatFilters"): {
            m_chatFilters.clear();
            m_chatFilters.append(object.value("chat_filters").toList());

            qDebug() << m_chatFilters;

            break;
        }

        case fnv::hash("updateUser"): {
            m_users.emplace(object.value("user").toMap().value("id").toLongLong(), object.value("user").toMap());

            break;
        }
        case fnv::hash("updateBasicGroup"): {
            m_basicGroup.emplace(object.value("basic_group").toMap().value("id").toLongLong(), object.value("basic_group").toMap());

            break;
        }
        case fnv::hash("updateSupergroup"): {
            m_supergroup.emplace(object.value("supergroup").toMap().value("id").toLongLong(), object.value("supergroup").toMap());
            break;
        }
        case fnv::hash("updateUserFullInfo"): {
            m_userFullInfo.emplace(object.value("user_id").toLongLong(), object.value("user_full_info").toMap());
            break;
        }
        case fnv::hash("updateBasicGroupFullInfo"): {
            m_basicGroupFullInfo.emplace(object.value("basic_group_id").toLongLong(), object.value("basic_group_full_info").toMap());
            break;
        }
        case fnv::hash("updateSupergroupFullInfo"): {
            m_supergroupFullInfo.emplace(object.value("supergroup_id").toLongLong(), object.value("supergroup_full_info").toMap());
            break;
        }
        case fnv::hash("updateFile"): {
            m_files.emplace(object.value("file").toMap().value("id").toInt(), object.value("file").toMap());
            break;
        }

        case fnv::hash("updateOption"): {
            m_options.insert(object.value("name").toString(), object.value("value").toMap().value("value"));
            break;
        }
    }
}

void StorageManager::setChatPositions(qint64 chatId, const QVariantList &positions) noexcept
{
    if (auto it = m_chats.find(chatId); it != m_chats.end())
    {
        auto result = it->second.value("positions").toList();

        std::ranges::for_each(positions, [&result](const auto &position) {
            auto removeIt = std::ranges::find_if(result, [position](const auto &value) {
                auto chatListEquals = [](const QVariantMap &list1, const QVariantMap &list2) {
                    if (list1.value("@type") != list2.value("@type"))
                        return false;

                    auto listType = list1.value("@type").toByteArray();

                    switch (fnv::hashRuntime(listType.constData()))
                    {
                        case fnv::hash("chatListMain"):
                            return true;
                        case fnv::hash("chatListArchive"):
                            return true;
                        case fnv::hash("chatListFilter"): {
                            return list1.value("chat_filter_id").toByteArray() == list2.value("chat_filter_id").toByteArray();
                        }
                    }

                    return false;
                };

                return chatListEquals(value.toMap().value("list").toMap(), position.toMap().value("list").toMap());
            });

            if (removeIt != result.end())
            {
                result.erase(removeIt);
            }

            result.append(position);
        });

        it->second.insert("positions", QVariantList() << result);
    }
}
