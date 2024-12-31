#include "ChatManager.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDateTime>

#include <algorithm>
#include <ranges>

ChatInfoFormatter::ChatInfoFormatter(std::shared_ptr<Chat> chat, std::shared_ptr<Locale> locale, std::shared_ptr<StorageManager> storage)
    : m_chat(std::move(chat))
    , m_locale(std::move(locale))
    , m_storageManager(std::move(storage))
{
    initializeMembers();
    updateStatus();
}

QString ChatInfoFormatter::title() const noexcept
{
    return Utils::getChatTitle(m_chat, m_storageManager);
}

QString ChatInfoFormatter::status() const noexcept
{
    return m_status;
}

void ChatInfoFormatter::handleBasicGroupUpdate(qlonglong groupId) noexcept
{
    if (!m_chat || m_chat->typeId() != groupId)
        return;

    if (auto group = m_storageManager->basicGroup(groupId))
    {
        m_basicGroup = std::move(group);
        updateStatus();
    }
}

void ChatInfoFormatter::handleSupergroupUpdate(qlonglong groupId) noexcept
{
    if (!m_chat || m_chat->typeId() != groupId)
        return;

    if (auto group = m_storageManager->supergroup(groupId))
    {
        m_supergroup = std::move(group);
        updateStatus();
    }
}

void ChatInfoFormatter::handleUserUpdate(qlonglong userId) noexcept
{
    if (!m_chat || m_chat->typeId() != userId)
        return;

    if (auto user = m_storageManager->user(userId))
    {
        m_user = std::move(user);
        updateStatus();
    }
}

void ChatInfoFormatter::handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount) noexcept
{
    if (!m_chat || m_chat->id() != chatId)
        return;

    if (m_onlineMemberCount != onlineMemberCount)
    {
        m_onlineMemberCount = onlineMemberCount;
        updateStatus();
    }
}

void ChatInfoFormatter::initializeMembers() noexcept
{
    const auto chatType = m_chat->type();
    const auto chatTypeId = m_chat->typeId();

    if (chatType == Chat::Private || chatType == Chat::Secret)
    {
        m_user = m_storageManager->user(chatTypeId);

        connect(m_storageManager.get(), SIGNAL(userUpdated(qlonglong)), SLOT(handleUserUpdate(qlonglong)));
        connect(m_storageManager.get(), SIGNAL(chatOnlineMemberCountUpdated(qlonglong, int)), SLOT(handleChatOnlineMemberCount(qlonglong, int)));
    }

    if (chatType == Chat::BasicGroup)
    {
        m_basicGroup = m_storageManager->basicGroup(chatTypeId);

        connect(m_storageManager.get(), SIGNAL(basicGroupUpdated(qlonglong)), SLOT(handleBasicGroupUpdate(qlonglong)));
    }

    if (chatType == Chat::Supergroup || chatType == Chat::Channel)
    {
        m_supergroup = m_storageManager->supergroup(chatTypeId);

        connect(m_storageManager.get(), SIGNAL(supergroupUpdated(qlonglong)), SLOT(handleSupergroupUpdate(qlonglong)));
    }
}

void ChatInfoFormatter::updateStatus() noexcept
{
    QString newStatus;

    if (m_basicGroup)
    {
        if (m_basicGroup->status() == BasicGroup::Status::Banned)
            newStatus = tr("YouWereKicked");
        else
            newStatus = formatStatus(m_basicGroup->memberCount(), "Members", "OnlineCount");
    }
    else if (m_supergroup)
    {
        if (!m_supergroup->isChannel() && m_supergroup->status() == Supergroup::Status::Banned)
        {
            newStatus = tr("YouWereKicked");
        }
        else
        {
            int count = getMemberCountWithFallback();
            newStatus = (count <= 0) ? (m_supergroup->hasLocation() ? tr("MegaLocation")
                                                                    : (m_supergroup->activeUsernames().isEmpty() ? tr("MegaPrivate") : tr("MegaPublic")))
                                     : formatStatus(count, "Members", "OnlineCount");
        }
    }
    else if (m_user)
    {
        if (isServiceNotification())
        {
            newStatus = tr("ServiceNotifications");
        }
        else if (m_user->isSupport())
        {
            newStatus = tr("SupportStatus");
        }
        else if (m_user->type() == User::Type::Bot)
        {
            newStatus = tr("Bot");
        }
        else
        {
            newStatus = formatUserStatus();
        }
    }

    if (m_status != newStatus)
    {
        m_status = newStatus;
        emit statusChanged();
    }
}

int ChatInfoFormatter::getMemberCountWithFallback() const noexcept
{
    int count = m_supergroup->memberCount();
    if (count == 0)
    {
        if (const auto fullInfo = m_storageManager->supergroupFullInfo(m_supergroup->id()))
        {
            count = fullInfo->memberCount();
        }
    }

    return count;
}

QString ChatInfoFormatter::formatStatus(int memberCount, const char *memberKey, const char *onlineKey) const noexcept
{
    const auto memberString = m_locale->formatPluralString(memberKey, memberCount);
    if (memberCount <= 1)
        return memberString;

    if (m_onlineMemberCount > 1)
    {
        return memberString + ", " + m_locale->formatPluralString(onlineKey, m_onlineMemberCount);
    }

    return memberString;
}

bool ChatInfoFormatter::isServiceNotification() const noexcept
{
    return std::ranges::contains(ServiceNotificationsUserIds, m_user->id());
}

QString ChatInfoFormatter::formatUserStatus() const noexcept
{
    switch (m_user->status())
    {
        case User::Status::Empty:
            return tr("ALongTimeAgo");
        case User::Status::LastMonth:
            return tr("WithinAMonth");
        case User::Status::LastWeek:
            return tr("WithinAWeek");
        case User::Status::Offline:
            return formatOfflineStatus();
        case User::Status::Online:
            return tr("Online");
        case User::Status::Recently:
            return tr("Lately");
        default:
            return {};
    }
}

QString ChatInfoFormatter::formatOfflineStatus() const noexcept
{
    const auto wasOnline = m_user->wasOnline();
    if (wasOnline.isNull())
        return tr("Invisible");

    const auto currentDate = QDate::currentDate();
    if (currentDate == wasOnline.date())
    {
        return tr("LastSeenFormatted").arg(tr("TodayAtFormatted")).arg(wasOnline.toString(tr("formatterDay12H")));
    }
    else if (wasOnline.date().daysTo(currentDate) < 2)
    {
        return tr("LastSeenFormatted").arg(tr("YesterdayAtFormatted")).arg(wasOnline.toString(tr("formatterDay12H")));
    }

    return tr("LastSeenDateFormatted").arg(tr("formatDateAtTime").arg(wasOnline.toString(tr("formatterYear"))).arg(wasOnline.toString(tr("formatterDay12H"))));
}

ChatManager::ChatManager(std::shared_ptr<StorageManager> storageManager, std::shared_ptr<Locale> locale)
    : m_client(storageManager->client())
    , m_locale(std::move(locale))
    , m_storage(std::move(storageManager))
    , m_mainModel(std::make_unique<ChatModel>(std::make_unique<ChatList>(ChatList::Main), m_locale, m_storage))
    , m_archivedModel(std::make_unique<ChatModel>(std::make_unique<ChatList>(ChatList::Archive), m_locale, m_storage))
    , m_folderModel(std::make_unique<ChatFolderModel>())
{
    updateFolderModels();

    connect(m_storage.get(), SIGNAL(chatFoldersUpdated()), SLOT(onChatFoldersUpdated()));
}

QObject *ChatManager::folderModel() const noexcept
{
    return m_folderModel.get();
}

QObject *ChatManager::mainModel() const noexcept
{
    return m_mainModel.get();
}

QObject *ChatManager::archivedModel() const noexcept
{
    return m_archivedModel.get();
}

QList<QObject *> ChatManager::folderModels() const noexcept
{
    QList<QObject *> models;
    models.reserve(m_folderModels.size());
    for (const auto &model : m_folderModels)
    {
        models.append(model.get());
    }

    return models;
}

Chat *ChatManager::selectedChat() const noexcept
{
    return m_selectedChat.get();
}

QObject *ChatManager::chatInfoFormatter() const noexcept
{
    return m_infoFormatter.get();
}

QObject *ChatManager::messageModel() const noexcept
{
    return m_messageModel.get();
}

void ChatManager::openChat(qlonglong chatId) noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::openChat>(chatId));

    auto chat = m_storage->chat(chatId);
    if (!chat)
        return;

    m_selectedChat = std::move(chat);

    m_messageModel = std::make_unique<MessageModel>(m_selectedChat, m_locale, m_storage);
    m_infoFormatter = std::make_unique<ChatInfoFormatter>(m_selectedChat, m_locale, m_storage);

    emit selectedChatChanged();
}

void ChatManager::closeChat(qlonglong chatId) noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::closeChat>(chatId));

    m_infoFormatter = nullptr;
    m_messageModel = nullptr;
    m_selectedChat = nullptr;
}

void ChatManager::onChatFoldersUpdated() noexcept
{
    updateFolderModels();
    emit folderModelsChanged();
}

void ChatManager::updateFolderModels() noexcept
{
    const auto &chatFolders = m_storage->chatFolders();

    std::ranges::for_each(chatFolders, [this](const auto &folder) {
        m_folderModels.emplace_back(std::make_unique<ChatModel>(std::make_unique<ChatList>(ChatList::Folder, folder->id()), m_locale, m_storage));
    });

    m_folderModel->setItems(chatFolders);
}
