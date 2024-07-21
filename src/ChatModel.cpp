#include "ChatModel.hpp"

#include "Chat.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QStringList>

#include <algorithm>
#include <cstring>

namespace detail {

QString getChatType(const QVariantMap &type)
{
    static const std::unordered_map<const char *, QString> chatTypeMap = {{"chatTypePrivate", "private"},
                                                                          {"chatTypeSecret", "secret"},
                                                                          {"chatTypeBasicGroup", "group"},
                                                                          {"chatTypeSupergroup", "supergroup"}};

    const auto chatType = type.value("@type").toByteArray().constData();

    if (auto it = chatTypeMap.find(chatType); it != chatTypeMap.end())
    {
        if (std::strcmp(chatType, "chatTypeSupergroup") == 0 && type.value("is_channel").toBool())
        {
            return "channel";
        }
        return it->second;
    }

    return {};
}

}  // namespace detail

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_sortTimer(new QTimer(this))
    , m_loadingTimer(new QTimer(this))
{
    connect(this, SIGNAL(chatListChanged()), this, SLOT(refresh()));

    connect(m_sortTimer, SIGNAL(timeout()), this, SLOT(sortChats()));
    connect(m_loadingTimer, SIGNAL(timeout()), this, SLOT(loadChats()));

    m_sortTimer->setInterval(500);
    m_sortTimer->setSingleShot(true);

    m_loadingTimer->setInterval(500);

    setRoleNames(roleNames());
}

ChatModel::~ChatModel()
{
    delete m_sortTimer;
    delete m_loadingTimer;
}

QObject *ChatModel::locale() const
{
    return m_locale;
}

void ChatModel::setLocale(QObject *locale)
{
    m_locale = qobject_cast<Locale *>(locale);
}

QObject *ChatModel::storageManager() const
{
    return m_storageManager;
}

void ChatModel::setStorageManager(QObject *storageManager)
{
    m_storageManager = qobject_cast<StorageManager *>(storageManager);
    m_client = qobject_cast<Client *>(m_storageManager->client());

    connect(m_storageManager, SIGNAL(updateChatItem(qint64)), this, SLOT(handleChatItem(qint64)));
    connect(m_storageManager, SIGNAL(updateChatPosition(qint64)), this, SLOT(handleChatPosition(qint64)));
}

int ChatModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_count;
}

bool ChatModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;

    return m_count < m_chatIds.count();
}

void ChatModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    const auto itemsToFetch = qMin(ChatSliceLimit, m_chatIds.size() - m_count);

    if (itemsToFetch <= 0)
        return;

    beginInsertRows(QModelIndex(), m_count, m_count + itemsToFetch - 1);

    m_count += itemsToFetch;

    endInsertRows();

    emit countChanged();
}

QVariant ChatModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return {};

    const auto chatId = m_chatIds.at(index.row());
    const auto chat = m_storageManager->getChat(chatId);

    switch (role)
    {
        case IdRole:
            return QString::number(chatId);
        case TypeRole:
            return detail::getChatType(chat->type());
        case TitleRole:
            return Utils::getChatTitle(chatId, m_storageManager, m_locale, true);
        case PhotoRole: {
            const auto smallPhoto = chat->photo().value("small").toMap();
            const auto localPhoto = smallPhoto.value("local").toMap();

            if (localPhoto.value("is_downloading_completed").toBool())
            {
                return "image://chatPhoto/" + localPhoto.value("path").toString();
            }

            return "image://theme/icon-l-content-avatar-placeholder";
        }

        case LastMessageSenderRole:
            return Utils::getMessageSenderName(chat->lastMessage(), m_storageManager, m_locale);
        case LastMessageContentRole:
            return Utils::getContent(chat->lastMessage(), m_storageManager, m_locale);
        case LastMessageDateRole: {
            return Utils::getMessageDate(chat->lastMessage(), m_locale);
        }
        case IsPinnedRole:
            return Utils::isChatPinned(chatId, m_list, m_storageManager);
        case UnreadCountRole:
            return chat->unreadCount();
        case UnreadMentionCountRole:
            return chat->unreadMentionCount();
        case IsMutedRole:
            return Utils::isChatMuted(chatId, m_storageManager);
        default:
            return {};
    }
}

QHash<int, QByteArray> ChatModel::roleNames() const
{
    QHash<int, QByteArray> roles;

    roles[IdRole] = "id";
    roles[TypeRole] = "type";
    roles[TitleRole] = "title";
    roles[PhotoRole] = "photo";
    roles[LastMessageSenderRole] = "lastMessageSender";
    roles[LastMessageContentRole] = "lastMessageContent";
    roles[LastMessageDateRole] = "lastMessageDate";
    roles[IsPinnedRole] = "isPinned";
    roles[UnreadCountRole] = "unreadCount";
    roles[UnreadMentionCountRole] = "unreadMentionCount";
    roles[IsMutedRole] = "isMuted";

    return roles;
}

int ChatModel::count() const
{
    return m_count;
}

bool ChatModel::loading() const
{
    return m_loading;
}

TdApi::ChatList ChatModel::chatList() const
{
    return m_chatList;
}

void ChatModel::setChatList(TdApi::ChatList value)
{
    if (m_chatList != value)
    {
        m_chatList = value;
        emit chatListChanged();
    }
}

int ChatModel::chatFolderId() const
{
    return m_chatFolderId;
}

void ChatModel::setChatFolderId(int value)
{
    if (m_chatList == TdApi::ChatListFolder && m_chatFolderId != value)
    {
        m_chatFolderId = value;
        emit chatListChanged();
    }
}

bool ChatModel::isPinned(int index) const noexcept
{
    return data(createIndex(index, 0), IsPinnedRole).toBool();
}

bool ChatModel::isMuted(int index) const noexcept
{
    return data(createIndex(index, 0), IsMutedRole).toBool();
}

void ChatModel::toggleChatIsPinned(int index)
{
    QModelIndex modelIndex = createIndex(index, 0);

    QVariantMap result;
    result.insert("@type", "toggleChatIsPinned");
    result.insert("chat_list", m_list);
    result.insert("chat_id", data(modelIndex, IdRole).toLongLong());
    result.insert("is_pinned", !data(modelIndex, IsPinnedRole).toBool());

    m_client->send(result, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "ok")
            QMetaObject::invokeMethod(this, "populate", Qt::QueuedConnection);
    });
}

void ChatModel::toggleChatNotificationSettings(int index)
{
    QModelIndex modelIndex = createIndex(index, 0);

    const auto chatId = data(modelIndex, IdRole).toLongLong();
    const auto isMuted = !data(modelIndex, IsMutedRole).toBool();

    if (const auto isMutedPrev = Utils::isChatMuted(chatId, m_storageManager); isMutedPrev == isMuted)
        return;

    const auto muteFor = isMuted ? MutedValueMax : MutedValueMin;
    QVariantMap newNotificationSettings;
    newNotificationSettings.insert("use_default_mute_for", false);
    newNotificationSettings.insert("mute_for", muteFor);

    QVariantMap result;
    result.insert("@type", "setChatNotificationSettings");
    result.insert("chat_id", chatId);
    result.insert("notification_settings", newNotificationSettings);

    m_client->send(result, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "ok")
            QMetaObject::invokeMethod(this, "populate", Qt::QueuedConnection);
    });
}

void ChatModel::populate()
{
    m_chatIds.clear();

    const auto chatIds = m_storageManager->getChatIds();
    for (const auto &id : chatIds)
    {
        const auto chat = m_storageManager->getChat(id);

        for (const auto &position : chat->positions())
        {
            if (Utils::chatListEquals(position.toMap().value("list").toMap(), m_list))
            {
                m_chatIds.append(id);
                break;  // No need to check further positions for this chat
            }
        }

        if (const auto chatPhoto = chat->photo(); !chatPhoto.isEmpty())
        {
            const auto smallPhoto = chatPhoto.value("small").toMap();
            const auto localPhoto = smallPhoto.value("local").toMap();

            if (!localPhoto.value("is_downloading_completed").toBool())
            {
                QVariantMap request;
                request.insert("@type", "downloadFile");
                request.insert("file_id", smallPhoto.value("id").toInt());
                request.insert("priority", 1);

                m_client->send(request);
            }
        }
    }

    sortChats();

    if (!m_chatIds.isEmpty())
    {
        fetchMore();
    }
}

void ChatModel::clear()
{
    beginResetModel();
    m_chatIds.clear();
    m_count = 0;
    m_list.clear();
    endResetModel();

    emit countChanged();
}

void ChatModel::refresh()
{
    m_loading = true;

    clear();

    m_loadingTimer->start();

    emit loadingChanged();
}

void ChatModel::sortChats()
{
    emit layoutAboutToBeChanged();

    std::ranges::sort(m_chatIds, [&](auto a, auto b) {
        return Utils::getChatOrder(a, m_list, m_storageManager) > Utils::getChatOrder(b, m_list, m_storageManager);
    });

    emit layoutChanged();
}

void ChatModel::handleChatItem(qint64 chatId)
{
    if (auto it = std::ranges::find(m_chatIds, chatId); it != m_chatIds.end())
    {
        auto index = std::distance(m_chatIds.begin(), it);
        QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}

void ChatModel::handleChatPosition(qint64 chatId)
{
    if (auto it = std::ranges::find(m_chatIds, chatId); it != m_chatIds.end())
    {
        // emit delayed event
        if (not m_sortTimer->isActive())
            m_sortTimer->start();
    }
}

void ChatModel::handleChatPhoto(const QVariantMap &file)
{
    if (file.value("local").toMap().value("is_downloading_completed").toBool())
    {
        auto it = std::ranges::find_if(m_chatIds, [file, this](qint64 chatId) {
            auto chat = m_storageManager->getChat(chatId);
            return chat->photo().value("small").toMap().value("id").toInt() == file.value("id").toInt();
        });

        if (it != m_chatIds.end())
        {
            QVariantMap chatPhoto;
            chatPhoto.insert("small", file);

            m_storageManager->getChat(*it)->setPhoto(chatPhoto);

            auto index = std::distance(m_chatIds.begin(), it);
            QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

            emit dataChanged(modelIndex, modelIndex);
        }
    }
}

void ChatModel::loadChats()
{
    switch (m_chatList)
    {
        case TdApi::ChatListMain:
            m_list.insert("@type", "chatListMain");
            break;
        case TdApi::ChatListArchive:
            m_list.insert("@type", "chatListArchive");
            break;
        case TdApi::ChatListFolder:
            m_list.insert("@type", "chatListFolder");
            m_list.insert("chat_folder_id", m_chatFolderId);
            break;
    }

    QVariantMap request;
    request.insert("@type", "loadChats");
    request.insert("chat_list", m_list);
    request.insert("limit", ChatSliceLimit);

    m_client->send(request, [this](const auto &value) {
        if (value.value("code").toInt() == 404)
        {
            m_loading = false;
            m_loadingTimer->stop();

            emit loadingChanged();
        }
    });
}
