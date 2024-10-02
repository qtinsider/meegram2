#include "ChatModel.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "Message.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDebug>

#include <td/telegram/td_api.h>

#include <algorithm>

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_client(StorageManager::instance().client())
    , m_locale(&Locale::instance())
    , m_storageManager(&StorageManager::instance())
{
    connect(&m_sortTimer, SIGNAL(timeout()), this, SLOT(sortChats()));
    connect(&m_loadingTimer, SIGNAL(timeout()), this, SLOT(loadChats()));

    connect(this, SIGNAL(chatListChanged()), this, SLOT(refresh()));

    m_sortTimer.setInterval(500);
    m_sortTimer.setSingleShot(true);

    m_loadingTimer.setInterval(500);

    setRoleNames(roleNames());
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

    return m_count < static_cast<int>(m_chats.size());
}

void ChatModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    const auto itemsToFetch = std::min(ChatSliceLimit, static_cast<int>(m_chats.size()) - m_count);

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

    const auto &chat = m_chats.at(index.row());

    switch (role)
    {
        case IdRole:
            return QVariant::fromValue(chat->id());
        case TypeRole:
            return chat->type();
        case TitleRole:
            return chat->title();
        case PhotoRole:
            return QVariant::fromValue(chat->photo());
        case LastMessage:
            return QVariant::fromValue(chat->lastMessage());
        case IsPinnedRole:
            return chat->isPinned();
        case UnreadCountRole:
            return chat->unreadCount();
        case UnreadMentionCountRole:
            return chat->unreadMentionCount();
        case IsMutedRole:
            return chat->isMuted();
        case ChatRole:
            return QVariant::fromValue(chat.get());
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
    roles[LastMessage] = "lastMessage";
    roles[IsPinnedRole] = "isPinned";
    roles[UnreadCountRole] = "unreadCount";
    roles[UnreadMentionCountRole] = "unreadMentionCount";
    roles[IsMutedRole] = "isMuted";
    roles[ChatRole] = "selectedChat";

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
    return m_chatList.type;
}

void ChatModel::setChatList(TdApi::ChatList value)
{
    if (m_chatList.type != value)
    {
        m_chatList.type = value;
        emit chatListChanged();
    }
}

int ChatModel::chatFolderId() const
{
    return m_chatList.folderId;
}

void ChatModel::setChatFolderId(int value)
{
    if (m_chatList.type == TdApi::ChatListFolder && m_chatList.folderId != value)
    {
        m_chatList.folderId = value;
        emit chatListChanged();
    }
}

Chat *ChatModel::get(int index) const
{
    // Assuming ChatRole returns a Chat* stored in QVariant
    return data(createIndex(index, 0), ChatRole).value<Chat *>();
}

void ChatModel::toggleChatIsPinned(qlonglong chatId, bool isPinned)
{
    auto request = td::td_api::make_object<td::td_api::toggleChatIsPinned>();
    request->chat_list_ = Utils::toChatList(m_chatList);
    request->chat_id_ = chatId;
    request->is_pinned_ = isPinned;

    m_client->send(std::move(request));
}

void ChatModel::toggleChatNotificationSettings(qlonglong chatId, bool isMuted)
{
    const auto muteFor = isMuted ? MutedValueMin : MutedValueMax;

    auto notificationSettings = td::td_api::make_object<td::td_api::chatNotificationSettings>();
    notificationSettings->use_default_mute_for_ = false;
    notificationSettings->mute_for_ = muteFor;

    m_client->send(td::td_api::make_object<td::td_api::setChatNotificationSettings>(chatId, std::move(notificationSettings)));
}

void ChatModel::populate()
{
    m_chats.clear();

    ChatListComparator comparator;
    auto targetChatList = Utils::toChatList(m_chatList);

    const auto &chatIds = m_storageManager->getChatIds();

    m_chats.reserve(chatIds.size());  // Reserve memory to avoid multiple reallocations

    for (const auto &id : chatIds)
    {
        const auto chat = m_storageManager->getChat(id);

        if (std::ranges::any_of(chat->positions_, [&](const auto &position) { return comparator(position->list_, targetChatList); }))
        {
            auto newChat = std::make_unique<Chat>(id, m_chatList);

            connect(newChat.get(), SIGNAL(chatItemUpdated(qlonglong)), this, SLOT(handleChatItem(qlonglong)));
            connect(newChat.get(), SIGNAL(chatPositionUpdated(qlonglong)), this, SLOT(handleChatPosition(qlonglong)));

            m_chats.emplace_back(std::move(newChat));
        }
    }

    if (!m_chats.empty())
    {
        sortChats();
        fetchMore();
    }
}

void ChatModel::clear()
{
    beginResetModel();
    m_chats.clear();
    m_count = 0;
    endResetModel();

    emit countChanged();
}

void ChatModel::refresh()
{
    m_loading = true;

    clear();

    m_loadingTimer.start();

    emit loadingChanged();
}

void ChatModel::sortChats()
{
    emit layoutAboutToBeChanged();

    std::ranges::sort(m_chats, std::ranges::greater{}, &Chat::getOrder);

    emit layoutChanged();
}

void ChatModel::handleChatItem(qlonglong chatId)
{
    if (auto it = std::ranges::find(m_chats, chatId, &Chat::id); it != m_chats.end())
    {
        auto index = std::distance(m_chats.begin(), it);
        QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}

void ChatModel::handleChatPosition(qlonglong chatId)
{
    if (auto it = std::ranges::find(m_chats, chatId, &Chat::id); it != m_chats.end())
    {
        // emit delayed event
        if (not m_sortTimer.isActive())
        {
            m_sortTimer.start();
        }
    }
}

void ChatModel::loadChats()
{
    auto request = td::td_api::make_object<td::td_api::loadChats>();
    request->chat_list_ = Utils::toChatList(m_chatList);
    request->limit_ = ChatSliceLimit;

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            if (td::move_tl_object_as<td::td_api::error>(response)->code_ == 404)
            {
                m_loading = false;
                m_loadingTimer.stop();

                emit loadingChanged();
            }
        }
    });
}
