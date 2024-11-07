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
#include <ranges>

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_client(StorageManager::instance().client())
    , m_locale(&Locale::instance())
    , m_storageManager(&StorageManager::instance())
    , m_list(std::make_unique<ChatList>(this))
{
    connect(&m_sortTimer, SIGNAL(timeout()), this, SLOT(sortChats()));
    connect(&m_loadingTimer, SIGNAL(timeout()), this, SLOT(loadChats()));

    connect(this, SIGNAL(listChanged()), this, SLOT(refresh()));

    connect(m_storageManager, SIGNAL(chatUpdated(qlonglong)), this, SLOT(handleChatItem(qlonglong)));
    connect(m_storageManager, SIGNAL(chatPositionUpdated(qlonglong)), this, SLOT(handleChatPosition(qlonglong)));

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
    if (!index.isValid() || index.row() >= static_cast<int>(m_chats.size()))
        return QVariant();

    auto chatPtr = m_chats.at(index.row()).lock();

    if (!chatPtr)
    {
        return QVariant();
    }

    switch (role)
    {
        case IdRole:
            return chatPtr->id();
        case TypeRole:
            return chatPtr->type();
        case TitleRole:
            return chatPtr->title();
        case PhotoRole:
            return QVariant::fromValue(chatPtr->photo());
        case LastMessage:
            return QVariant::fromValue(chatPtr->lastMessage());
        case IsPinnedRole:
            return getChatPosition(chatPtr.get(), m_list.get())->isPinned();
        case UnreadCountRole:
            return chatPtr->unreadCount();
        case UnreadMentionCountRole:
            return chatPtr->unreadMentionCount();
        case IsMutedRole:
            return chatPtr->isMuted();
        default:
            return QVariant();
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

ChatList *ChatModel::list() const
{
    return m_list.get();
}

void ChatModel::setList(ChatList *value)
{
    if (m_list.get() == value)
    {
        return;
    }

    if (m_list)
    {
        disconnect(m_list.get(), SIGNAL(listChanged()), this, SIGNAL(listChanged()));
    }

    m_list.reset(value);

    if (m_list)
    {
        connect(m_list.get(), SIGNAL(listChanged()), this, SIGNAL(listChanged()));
    }

    emit listChanged();
}

void ChatModel::toggleChatIsPinned(qlonglong chatId, bool isPinned)
{
    auto request = td::td_api::make_object<td::td_api::toggleChatIsPinned>();
    request->chat_list_ = Utils::toChatList(m_list);
    request->chat_id_ = chatId;
    request->is_pinned_ = isPinned;

    m_client->send(std::move(request));
}

void ChatModel::populate()
{
    m_chats.clear();

    const auto &chatIds = m_storageManager->getChatIds();

    m_chats.reserve(chatIds.size());

    for (const auto &id : chatIds)
    {
        const auto chat = m_storageManager->getChat(id);

        if (!chat)
        {
            continue;
        }

        if (std::ranges::any_of(chat->positions(), [&](const auto &position) { return *position->list() == *m_list; }))
        {
            m_chats.emplace_back(std::move(chat));
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

    std::ranges::sort(m_chats, std::greater{}, [&](auto &&chatPtr) {
        if (auto chat = chatPtr.lock())
        {
            if (auto pos = getChatPosition(chat.get(), m_list.get()))
            {
                return pos->order();
            }
        }
        return std::numeric_limits<qlonglong>::min();
    });

    emit layoutChanged();
}

void ChatModel::handleChatItem(qlonglong chatId)
{
    auto chats = m_chats | std::views::transform([](const auto &chat) { return chat.lock(); });

    auto it = std::ranges::find_if(chats, [chatId](const auto &chat) { return chat && chat->id() == chatId; });
    if (it != chats.end())
    {
        auto index = std::distance(chats.begin(), it);
        QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);
        emit dataChanged(modelIndex, modelIndex);
    }
}

void ChatModel::handleChatPosition(qlonglong chatId)
{
    auto chats = m_chats | std::views::transform([](const auto &chat) { return chat.lock(); });

    auto it = std::ranges::find_if(chats, [chatId](const auto &chat) { return chat && chat->id() == chatId; });
    if (it != chats.end())
    {
        // emit delayed event
        if (not m_sortTimer.isActive())
        {
            m_sortTimer.start();
        }
    }
}

ChatPosition *ChatModel::getChatPosition(Chat *chat, ChatList *list) const
{
    const auto &positions = chat->positions();

    if (auto it = std::ranges::find_if(positions, [&](const auto &position) { return *position->list() == *list; }); it != positions.end())
    {
        return it->get();
    }

    return nullptr;
}

void ChatModel::loadChats()
{
    auto request = td::td_api::make_object<td::td_api::loadChats>();
    request->chat_list_ = Utils::toChatList(m_list);
    request->limit_ = ChatSliceLimit;

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            if (td::td_api::move_object_as<td::td_api::error>(response)->code_ == 404)
            {
                m_loading = false;
                m_loadingTimer.stop();

                emit loadingChanged();
            }
        }
    });
}
