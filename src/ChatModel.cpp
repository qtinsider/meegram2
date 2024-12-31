#include "ChatModel.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Message.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDebug>

#include <td/telegram/td_api.h>

#include <algorithm>
#include <ranges>

ChatModel::ChatModel(std::unique_ptr<ChatList> list, std::shared_ptr<Locale> locale, std::shared_ptr<StorageManager> storage)
    : m_list(std::move(list))
    , m_locale(std::move(locale))
    , m_storageManager(std::move(storage))
{
    connect(&m_sortTimer, SIGNAL(timeout()), this, SLOT(sortChats()));
    connect(&m_loadingTimer, SIGNAL(timeout()), this, SLOT(loadChats()));

    m_sortTimer.setInterval(500);
    m_sortTimer.setSingleShot(true);

    m_loadingTimer.setInterval(500);

    connect(m_storageManager.get(), SIGNAL(chatUpdated(qlonglong)), SLOT(handleChatItem(qlonglong)));
    connect(m_storageManager.get(), SIGNAL(chatPositionUpdated(qlonglong)), SLOT(handleChatPosition(qlonglong)));

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

    int remainingItems = static_cast<int>(m_chats.size()) - m_count;

    const auto itemsToFetch = std::min(ChatSliceLimit, remainingItems);

    if (itemsToFetch <= 0)
        return;

    int startIndex = m_count;
    int endIndex = m_count + itemsToFetch - 1;

    beginInsertRows(QModelIndex(), startIndex, endIndex);
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
        return QVariant();

    switch (role)
    {
        case IdRole:
            return chatPtr->id();
        case TypeRole:
            return chatPtr->type();
        case TitleRole:
            return Utils::getChatTitle(chatPtr, m_storageManager);
        case DateRole:
            return Utils::getMessageDate(chatPtr->lastMessage());
        case PhotoRole:
            return QVariant::fromValue(chatPtr->photo());
        case LastMessageRole:
            return Utils::getContent(chatPtr->lastMessage(), m_storageManager, m_locale);
        case IsPinnedRole:
            return getChatPosition(chatPtr.get())->isPinned();
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
    roles[DateRole] = "date";
    roles[PhotoRole] = "photo";
    roles[LastMessageRole] = "lastMessage";
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

void ChatModel::populate()
{
    m_chats.clear();

    const auto &chatIds = m_storageManager->chatIds();

    m_chats.reserve(chatIds.size());

    for (const auto &id : chatIds)
    {
        auto chat = m_storageManager->chat(id);

        if (!chat)
            continue;

        if (std::ranges::any_of(chat->positions(), [&](const auto &pos) { return *pos->list() == *m_list; }))
        {
            m_chats.emplace_back(chat);
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
            if (auto pos = getChatPosition(chat.get()))
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
        if (!m_sortTimer.isActive())
        {
            m_sortTimer.start();
        }
    }
}

ChatPosition *ChatModel::getChatPosition(Chat *chat) const
{
    if (!chat)
        return nullptr;

    const auto &positions = chat->positions();

    if (auto it = std::ranges::find_if(positions, [&](const auto &pos) { return *pos->list() == *m_list; }); it != positions.end())
    {
        return it->get();
    }

    return nullptr;
}

void ChatModel::toggleChatIsPinned(qlonglong chatId, bool isPinned)
{
    auto request = td::td_api::make_object<td::td_api::toggleChatIsPinned>();
    request->chat_list_ = Utils::toChatList(m_list);
    request->chat_id_ = chatId;
    request->is_pinned_ = isPinned;

    auto client = m_storageManager->client();
    if (!client)
        return;

    client->send(std::move(request));
}

void ChatModel::loadChats()
{
    auto request = td::td_api::make_object<td::td_api::loadChats>();
    request->chat_list_ = Utils::toChatList(m_list);
    request->limit_ = ChatSliceLimit;

    auto client = m_storageManager->client();
    if (!client)
        return;

    client->send(std::move(request), [this](auto &&response) {
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
