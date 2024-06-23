#include "ChatModel.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QDebug>
#include <QStringList>

#include <algorithm>

namespace detail {

QString getChatType(const QVariantMap &chat)
{
    const auto type = chat.value("type").toMap();
    const auto chatType = type.value("@type").toByteArray();

    switch (fnv::hashRuntime(chatType.constData()))
    {
        case fnv::hash("chatTypePrivate"):
            return "private";
        case fnv::hash("chatTypeSecret"):
            return "secret";
        case fnv::hash("chatTypeBasicGroup"):
            return "group";
        case fnv::hash("chatTypeSupergroup"): {
            if (type.value("is_channel").toBool())
                return "channel";

            return "supergroup";
        }
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

    m_sortTimer->setInterval(1000);
    m_sortTimer->setSingleShot(true);

    m_loadingTimer->setInterval(500);

    setRoleNames(roleNames());
}

ChatModel::~ChatModel()
{
    delete m_sortTimer;
    delete m_loadingTimer;
}

TdManager *ChatModel::manager() const
{
    return m_manager;
}

void ChatModel::setManager(TdManager *manager)
{
    m_manager = manager;

    m_storageManager = m_manager->storageManager();
    m_client = m_manager->storageManager()->client();
    m_locale = m_manager->locale();

    connect(m_storageManager, SIGNAL(updateChatItem(qint64)), SLOT(handleChatItem(qint64)));
    connect(m_storageManager, SIGNAL(updateChatPosition(qint64)), SLOT(handleChatPosition(qint64)));
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
            return detail::getChatType(chat);
        case TitleRole:
            return Utils::getChatTitle(chatId, m_storageManager, m_locale);
        case PhotoRole: {
            const auto chatPhoto = chat.value("photo").toMap();
            const auto localPhoto = chatPhoto.value("small").toMap().value("local").toMap();
            if (localPhoto.value("is_downloading_completed").toBool())
                return localPhoto.value("path").toString();
            return {};
        }
        case LastMessageSenderRole:
            return Utils::getMessageSenderName(chat.value("last_message").toMap(), m_storageManager, m_locale);
        case LastMessageContentRole:
            return Utils::getContent(chat.value("last_message").toMap(), m_storageManager, m_locale);
        case LastMessageDateRole: {
            const auto lastMessage = chat.value("last_message").toMap();
            const auto date = QDateTime::fromMSecsSinceEpoch(lastMessage.value("date").toLongLong() * 1000);
            const auto days = date.daysTo(QDateTime::currentDateTime());

            if (days == 0)
                return date.toString(m_locale->getString("formatterDay12H"));
            else if (days < 7)
                return date.toString(m_locale->getString("formatterWeek"));
            return date.toString(m_locale->getString("formatterYear"));
        }
        case IsPinnedRole:
            return Utils::isChatPinned(chatId, m_list, m_storageManager);
        case UnreadCountRole:
            return chat.value("unread_count").toInt();
        case UnreadMentionCountRole:
            return chat.value("unread_mention_count").toInt();
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

int ChatModel::chatFilterId() const
{
    return m_chatFilterId;
}

void ChatModel::setChatFilterId(int value)
{
    if (m_chatList == TdApi::ChatListFilter && m_chatFilterId != value)
    {
        m_chatFilterId = value;
        emit chatListChanged();
    }
}

QVariant ChatModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);

    QVariantMap result;
    result.insert("id", data(modelIndex, IdRole));
    result.insert("type", data(modelIndex, TypeRole));
    result.insert("title", data(modelIndex, TitleRole));
    result.insert("photo", data(modelIndex, PhotoRole));
    result.insert("lastMessageSender", data(modelIndex, LastMessageSenderRole));
    result.insert("lastMessageContent", data(modelIndex, LastMessageContentRole));
    result.insert("lastMessageDate", data(modelIndex, LastMessageDateRole));
    result.insert("isPinned", data(modelIndex, IsPinnedRole));
    result.insert("unreadMentionCount", data(modelIndex, UnreadMentionCountRole));
    result.insert("unreadCount", data(modelIndex, UnreadCountRole));
    result.insert("isMuted", data(modelIndex, IsMutedRole));

    return result;
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

    m_manager->sendRequest(result, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "ok")
            QMetaObject::invokeMethod(this, "populate", Qt::QueuedConnection);
    });
}

void ChatModel::toggleChatNotificationSettings(int index)
{
    QModelIndex modelIndex = createIndex(index, 0);

    auto chatId = data(modelIndex, IdRole).toLongLong();
    auto isMuted = !data(modelIndex, IsMutedRole).toBool();

    auto chat = m_storageManager->getChat(chatId);

    if (auto isMutedPrev = Utils::isChatMuted(chatId, m_storageManager); isMutedPrev == isMuted)
        return;

    auto muteFor = isMuted ? MutedValueMax : MutedValueMin;
    QVariantMap newNotificationSettings;
    newNotificationSettings.insert("use_default_mute_for", false);
    newNotificationSettings.insert("mute_for", muteFor);

    QVariantMap result;
    result.insert("@type", "setChatNotificationSettings");
    result.insert("chat_id", chatId);
    result.insert("notification_settings", newNotificationSettings);

    m_manager->sendRequest(result, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "ok")
            QMetaObject::invokeMethod(this, "populate", Qt::QueuedConnection);
    });
}

void ChatModel::classBegin()
{
}

void ChatModel::componentComplete()
{
    refresh();
}

void ChatModel::populate()
{
    m_chatIds.clear();

    const auto chatIds = m_storageManager->getChatIds();
    for (const auto &id : chatIds)
    {
        const auto chat = m_storageManager->getChat(id);
        const auto positions = chat.value("positions").toList();

        for (const auto &pos : positions)
        {
            const auto positionMap = pos.toMap();
            if (Utils::chatListEquals(positionMap.value("list").toMap(), m_list))
            {
                m_chatIds.append(id);
                break;  // No need to check further positions for this chat
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
    auto it = std::ranges::find_if(m_chatIds, [chatId](qint64 id) { return id == chatId; });

    if (it != m_chatIds.end())
    {
        auto index = std::distance(m_chatIds.begin(), it);
        QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

        emit dataChanged(modelIndex, modelIndex);
    }
}

void ChatModel::handleChatPosition(qint64 chatId)
{
    auto it = std::ranges::find_if(m_chatIds, [chatId](qint64 id) { return id == chatId; });

    if (it != m_chatIds.end())
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
            return chat.value("photo").toMap().value("small").toMap().value("id").toInt() == file.value("id").toInt();
        });

        if (it != m_chatIds.end())
        {
            QVariantMap chatPhoto;
            chatPhoto.insert("small", file);

            auto chat = m_storageManager->getChat(*it);
            chat.insert("photo", chatPhoto);

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
        case TdApi::ChatListFilter:
            m_list.insert("@type", "chatListFilter");
            m_list.insert("chat_filter_id", m_chatFilterId);
            break;
    }

    QVariantMap request;
    request.insert("@type", "loadChats");
    request.insert("chat_list", m_list);
    request.insert("limit", ChatSliceLimit);

    m_manager->sendRequest(request, [this](const auto &value) {
        if (value.value("code").toInt() == 404)
        {
            m_loading = false;
            m_loadingTimer->stop();

            emit loadingChanged();
        }
    });
}
