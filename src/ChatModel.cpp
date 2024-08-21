#include "ChatModel.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <td/telegram/td_api.h>

#include <QDateTime>
#include <QStringList>

#include <algorithm>

namespace detail {

QString chatTypeString(const td::td_api::object_ptr<td::td_api::ChatType> &type)
{
    switch (type->get_id())
    {
        case td::td_api::chatTypePrivate::ID:
            return "private";

        case td::td_api::chatTypeSecret::ID:
            return "secret";

        case td::td_api::chatTypeBasicGroup::ID:
            return "group";

        case td::td_api::chatTypeSupergroup::ID: {
            const auto *value = static_cast<const td::td_api::chatTypeSupergroup *>(type.get());
            return value->is_channel_ ? "channel" : "supergroup";
        }

        default:
            return {};
    }
}

}  // namespace detail

ChatModel::ChatModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_sortTimer(new QTimer(this))
    , m_loadingTimer(new QTimer(this))
{
    m_storageManager = &StorageManager::instance();

    m_client = m_storageManager->client();
    m_locale = m_storageManager->locale();

    connect(m_storageManager, SIGNAL(chatItemUpdated(qint64)), this, SLOT(handleChatItem(qint64)));
    connect(m_storageManager, SIGNAL(chatPositionUpdated(qint64)), this, SLOT(handleChatPosition(qint64)));

    connect(m_sortTimer, SIGNAL(timeout()), this, SLOT(sortChats()));
    connect(m_loadingTimer, SIGNAL(timeout()), this, SLOT(loadChats()));

    connect(this, SIGNAL(chatListChanged()), this, SLOT(refresh()));

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

    return m_count < static_cast<int>(m_chatIds.size());
}

void ChatModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    const auto itemsToFetch = std::min(ChatSliceLimit, static_cast<int>(m_chatIds.size()) - m_count);

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
            return detail::chatTypeString(chat->type_);
        case TitleRole:
            return Utils::getChatTitle(chatId, m_storageManager, m_locale, true);
        case PhotoRole: {
            if (const auto &chatPhoto = chat->photo_; chatPhoto)
            {
                if (const auto &smallPhoto = chatPhoto->small_; smallPhoto)
                {
                    if (const auto &local = smallPhoto->local_; local && local->is_downloading_completed_)
                    {
                        return QString::fromStdString("image://chatPhoto/" + local->path_);
                    }
                }
            }

            return "image://theme/icon-l-content-avatar-placeholder";
        }

        case LastMessageSenderRole:
            return Utils::getMessageSenderName(*chat->last_message_, m_storageManager, m_locale);
        case LastMessageContentRole:
            return Utils::getContent(*chat->last_message_, m_storageManager, m_locale);
        case LastMessageDateRole: {
            return Utils::getMessageDate(*chat->last_message_, m_locale);
        }
        case IsPinnedRole:
            return Utils::getChatPosition(chat, m_chatList)->is_pinned_;
        case UnreadCountRole:
            return chat->unread_count_;
        case UnreadMentionCountRole:
            return chat->unread_mention_count_;
        case IsMutedRole:
            return chat->notification_settings_->mute_for_ > 0;
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

    auto request = td::td_api::make_object<td::td_api::toggleChatIsPinned>();
    request->chat_list_ = Utils::toChatList(m_chatList);
    request->chat_id_ = data(modelIndex, IdRole).toLongLong();
    request->is_pinned_ = !data(modelIndex, IsPinnedRole).toBool();

    m_client->send(std::move(request), [this](auto &&value) {
        if (value->get_id() == td::td_api::ok::ID)
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

    auto newNotificationSettings = td::td_api::make_object<td::td_api::chatNotificationSettings>();
    newNotificationSettings->use_default_mute_for_ = false;
    newNotificationSettings->mute_for_ = muteFor;

    auto request = td::td_api::make_object<td::td_api::setChatNotificationSettings>();
    request->chat_id_ = chatId;
    request->notification_settings_ = std::move(newNotificationSettings);

    m_client->send(std::move(request), [this](auto &&value) {
        if (value->get_id() == td::td_api::ok::ID)
            QMetaObject::invokeMethod(this, "populate", Qt::QueuedConnection);
    });
}

void ChatModel::populate()
{
    m_chatIds.clear();

    ChatListComparator comparator;
    auto targetChatList = Utils::toChatList(m_chatList);

    m_chatIds.reserve(m_storageManager->getChatIds().size());  // Reserve memory to avoid multiple reallocations

    for (const auto &id : m_storageManager->getChatIds())
    {
        const auto chat = m_storageManager->getChat(id);

        if (std::ranges::any_of(chat->positions_, [&](const auto &position) { return comparator(position->list_, targetChatList); }))
        {
            m_chatIds.push_back(id);

            if (const auto &chatPhoto = chat->photo_; chatPhoto)
            {
                const auto &smallPhoto = chatPhoto->small_;  // No need to copy, use a reference
                if (smallPhoto && smallPhoto->local_ && !smallPhoto->local_->is_downloading_completed_)
                {
                    auto request = td::td_api::make_object<td::td_api::downloadFile>();
                    request->file_id_ = smallPhoto->id_;  // Access the id directly
                    request->priority_ = 1;

                    m_client->send(std::move(request), {});
                }
            }
        }
    }

    sortChats();

    if (!m_chatIds.empty())
    {
        fetchMore();
    }
}

void ChatModel::clear()
{
    beginResetModel();
    m_chatIds.clear();
    m_count = 0;
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

struct Sorter
{
    StorageManager *const store;
    const ChatList &chatList;

    bool operator()(auto a, auto b) const
    {
        const auto chatA = store->getChat(a);
        const auto chatB = store->getChat(b);

        if (!chatA || !chatB)
        {
            return false;
        }

        auto orderA = Utils::getChatPosition(chatA, chatList)->order_;
        auto orderB = Utils::getChatPosition(chatB, chatList)->order_;

        return orderA > orderB;
    }
};

void ChatModel::sortChats()
{
    emit layoutAboutToBeChanged();

    Sorter sorter{m_storageManager, m_chatList};

    std::ranges::sort(m_chatIds, sorter);

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
                m_loadingTimer->stop();

                emit loadingChanged();
            }
        }
    });
}
