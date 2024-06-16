#include "ChatModel.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"

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

    switch (const auto chat = m_storageManager->getChat(chatId); role)
    {
        case IdRole:
            return QString::number(chatId);
        case TypeRole:
            return detail::getChatType(chat);
        case TitleRole: {
            return getChatTitle(chat);
        }
        case PhotoRole: {
            if (auto chatPhoto = chat.value("photo").toMap();
                chatPhoto.value("small").toMap().value("local").toMap().value("is_downloading_completed").toBool())
            {
                return chatPhoto.value("small").toMap().value("local").toMap().value("path").toString();
            }

            return {};
        }
        case LastMessageSenderRole: {
            return getMessageSenderName(chat.value("last_message").toMap());
        }
        case LastMessageContentRole: {
            return getContent(chat.value("last_message").toMap());
        }
        case LastMessageDateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(chat.value("last_message").toMap().value("date").toLongLong() * 1000);

            auto days = date.daysTo(QDateTime::currentDateTime());

            if (days == 0)
                return date.toString(m_locale->getString("formatterDay12H"));
            else if (days < 7)
                return date.toString(m_locale->getString("formatterWeek"));

            return date.toString(m_locale->getString("formatterYear"));
        }
        case IsPinnedRole: {
            return isChatPinned(chat, m_list);
        }
        case UnreadCountRole: {
            return chat.value("unread_count").toInt();
        }
        case UnreadMentionCountRole: {
            return chat.value("unread_mention_count").toInt();
        }
        case IsMutedRole: {
            return isChatMuted(chat);
        }
    }

    return {};
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

    if (auto isMutedPrev = isChatMuted(chat); isMutedPrev == isMuted)
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

    for (auto id : m_storageManager->getChatIds())
    {
        for (auto chat = m_storageManager->getChat(id); const auto &position : chat.value("positions").toList())
        {
            if (chatListEquals(position.toMap().value("list").toMap(), m_list))
                m_chatIds.append(id);
        }
    }

    sortChats();

    if (!m_chatIds.isEmpty())
        fetchMore();
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

bool ChatModel::isBotUser(const QVariantMap &user) const noexcept
{
    auto userType = user.value("type").toMap();
    return userType.value("@type").toByteArray() == "userTypeBot";
}

bool ChatModel::isMeChat(const QVariantMap &chat) const noexcept
{
    auto type = chat.value("type").toMap();

    auto chatType = type.value("@type").toByteArray();
    if (chatType == "chatTypeSecret" || chatType == "chatTypePrivate")
    {
        return m_storageManager->getMyId() == type.value("user_id").toLongLong();
    }

    return false;
}

bool ChatModel::isMeUser(qint64 userId) const noexcept
{
    return m_storageManager->getMyId() == userId;
}

bool ChatModel::isUserOnline(const QVariantMap &user) const noexcept
{
    auto status = user.value("status").toMap();
    auto type = user.value("type").toMap();

    if (std::ranges::any_of(ServiceNotificationsUserIds, [user](qint64 userId) { return userId == user.value("id").toLongLong(); }))
    {
        return false;
    }

    return status.value("@type").toByteArray() == "userStatusOnline" && type.value("@type").toByteArray() != "userTypeBot";
}

bool ChatModel::isChannelChat(const QVariantMap &chat) const noexcept
{
    auto type = chat.value("type").toMap();

    auto chatType = type.value("@type").toByteArray();
    if (chatType == "chatTypeSupergroup")
        return type.value("is_channel").toBool();

    return false;
}

bool ChatModel::isSupergroup(const QVariantMap &chat) const noexcept
{
    auto type = chat.value("type").toMap();

    return type.value("@type").toByteArray() == "chatTypeSupergroup";
}

bool ChatModel::isServiceMessage(const QVariantMap &message) noexcept
{
    auto contentType = message.value("content").toMap().value("@type").toByteArray();
    static const std::unordered_map<std::string, bool> serviceMap = {
        {"messageText", false},
        {"messageAnimation", false},
        {"messageAudio", false},
        {"messageDocument", false},
        {"messagePhoto", false},
        {"messageExpiredPhoto", true},
        {"messageSticker", false},
        {"messageVideo", false},
        {"messageExpiredVideo", true},
        {"messageVideoNote", false},
        {"messageVoiceNote", false},
        {"messageLocation", false},
        {"messageVenue", false},
        {"messageContact", false},
        {"messageDice", false},
        {"messageGame", false},
        {"messagePoll", false},
        {"messageInvoice", false},
        {"messageCall", false},
        {"messageBasicGroupChatCreate", true},
        {"messageSupergroupChatCreate", true},
        {"messageChatChangeTitle", true},
        {"messageChatChangePhoto", true},
        {"messageChatDeletePhoto", true},
        {"messageChatAddMembers", true},
        {"messageChatJoinByLink", true},
        {"messageChatDeleteMember", true},
        {"messageChatUpgradeTo", true},
        {"messageChatUpgradeFrom", true},
        {"messagePinMessage", true},
        {"messageScreenshotTaken", true},
        {"messageChatSetTtl", true},
        {"messageCustomServiceAction", true},
        {"messageGameScore", true},
        {"messagePaymentSuccessful", true},
        {"messagePaymentSuccessfulBot", true},
        {"messageContactRegistered", true},
        {"messageWebsiteConnected", true},
        {"messagePassportDataSent", true},
        {"messagePassportDataReceived", true},
        {"messageLiveLocationApproached", true},
        {"messageUnsupported", true},
    };

    if (auto it = serviceMap.find(contentType.constData()); it != serviceMap.end())
    {
        return it->second || message.value("ttl").toInt() > 0;
    }

    return {};
}

QString ChatModel::getAudioTitle(const QVariantMap &audio) const noexcept
{
    auto fileName = audio.value("file_name").toString();
    auto title = audio.value("title").toString();
    auto performer = audio.value("performer").toString();

    if (title.isEmpty() and performer.isEmpty())
        return fileName;

    QString result;
    result += performer.trimmed().isEmpty() ? "Unknown Artist" : performer;
    result += " - ";
    result += title.trimmed().isEmpty() ? "Unknown Track" : title;

    return result;
}

QString ChatModel::getCallContent(const QVariantMap &sender, const QVariantMap &content) const noexcept
{
    auto isVideo = content.value("is_video").toBool();
    auto discardReason = content.value("discard_reason").toMap();
    auto isMissed = discardReason.value("@type").toByteArray() == "callDiscardReasonMissed";
    auto isBusy = discardReason.value("@type").toByteArray() == "callDiscardReasonDeclined";

    if (isMeUser(sender.value("user_id").toInt()))
    {
        if (isMissed)
        {
            if (isVideo)
            {
                return m_locale->getString("CallMessageVideoOutgoingMissed");
            }

            return m_locale->getString("CallMessageOutgoingMissed");
        }

        if (isVideo)
        {
            return m_locale->getString("CallMessageVideoOutgoing");
        }

        return m_locale->getString("CallMessageOutgoing");
    }

    if (isMissed)
    {
        if (isVideo)
        {
            return m_locale->getString("CallMessageVideoIncomingMissed");
        }

        return m_locale->getString("CallMessageIncomingMissed");
    }
    else if (isBusy)
    {
        if (isVideo)
        {
            return m_locale->getString("CallMessageVideoIncomingDeclined");
        }

        return m_locale->getString("CallMessageIncomingDeclined");
    }

    if (isVideo)
    {
        return m_locale->getString("CallMessageVideoIncoming");
    }

    return m_locale->getString("CallMessageIncoming");
}

QString ChatModel::getChatTitle(const QVariantMap &chat) const noexcept
{
    if (const auto title = chat.value("title").toString(); !isMeChat(chat))
        return title.isEmpty() ? m_locale->getString("HiddenName") : title;

    return m_locale->getString("SavedMessages");
}

QString ChatModel::getMessageAuthor(const QVariantMap &message, bool openUser) const noexcept
{
    auto sender = message.value("sender_id").toMap();

    if (sender.value("@type").toByteArray() == "messageSenderUser")
        return openUser ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"userId://" +
                              sender.value("user_id").toString() + "\">" + getUserShortName(sender.value("user_id").toLongLong()) + "</a>"
                        : getUserShortName(sender.value("user_id").toLongLong());

    auto chat = m_storageManager->getChat(message.value("chat_id").toLongLong());

    return openUser ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"chatId://" +
                          sender.value("chat_id").toString() + "\">" + chat.value("title").toString() + "</a>"
                    : chat.value("title").toString();
}

QString ChatModel::getUserShortName(qint64 userId) const noexcept
{
    auto user = m_storageManager->getUser(userId);

    auto type = user.value("type").toMap();
    auto firstName = user.value("first_name").toString();
    auto lastName = user.value("last_name").toString();

    auto userType = type.value("@type").toByteArray();
    if (userType == "userTypeBot" || userType == "userTypeRegular")
    {
        if (!firstName.isEmpty())
            return firstName;

        if (!lastName.isEmpty())
            return lastName;
    }
    if (userType == "userTypeDeleted" || userType == "userTypeUnknown")
        return m_locale->getString("HiddenName");

    return QString();
}

QString ChatModel::getMessageSenderName(const QVariantMap &message) const noexcept
{
    if (isServiceMessage(message))
        return QString();

    auto sender = message.value("sender_id").toMap();

    auto chat = m_storageManager->getChat(message.value("chat_id").toLongLong());

    auto chatType = chat.value("type").toMap().value("@type").toByteArray();
    switch (fnv::hashRuntime(chatType.constData()))
    {
        case fnv::hash("chatTypePrivate"):
        case fnv::hash("chatTypeSecret"): {
            return QString();
        }
        case fnv::hash("chatTypeBasicGroup"):
        case fnv::hash("chatTypeSupergroup"): {
            if (isChannelChat(chat))
                return QString();

            auto senderType = sender.value("@type").toByteArray();
            switch (fnv::hashRuntime(senderType.constData()))
            {
                case fnv::hash("messageSenderUser"): {
                    if (isMeUser(sender.value("user_id").toLongLong()))
                        return m_locale->getString("FromYou");

                    return getUserShortName(m_storageManager->getUser(sender.value("user_id").toLongLong()));
                }
                case fnv::hash("messageSenderChat"): {
                    return getChatTitle(chat);
                }
            }
        }
    }

    return QString();
}

QString ChatModel::getUserShortName(const QVariantMap &user) const noexcept
{
    auto type = user.value("type").toMap();
    auto firstName = user.value("first_name").toString();
    auto lastName = user.value("last_name").toString();

    auto userType = type.value("@type").toByteArray();
    if (userType == "userTypeBot" || userType == "userTypeRegular")
    {
        if (!firstName.isEmpty())
            return firstName;

        if (!lastName.isEmpty())
            return lastName;
    }
    if (userType == "userTypeDeleted" || userType == "userTypeUnknown")
        return m_locale->getString("HiddenName");

    return QString();
}

QVariantMap ChatModel::getChatPosition(const QVariantMap &chat, const QVariantMap &chatList) const noexcept
{
    auto positions = chat.value("positions").toList();

    auto chatListType = chatList.value("@type").toByteArray();
    switch (fnv::hashRuntime(chatListType.constData()))
    {
        case fnv::hash("chatListMain"): {
            if (auto it = std::ranges::find_if(positions,
                                               [](const auto &position) {
                                                   auto list = position.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListMain";
                                               });
                it != positions.end())
                return it->toMap();

            break;
        }
        case fnv::hash("chatListArchive"): {
            if (auto it = std::ranges::find_if(positions,
                                               [](const auto &position) {
                                                   auto list = position.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListArchive";
                                               });
                it != positions.end())
                return it->toMap();

            break;
        }
        case fnv::hash("chatListFilter"): {
            if (auto it = std::ranges::find_if(positions,
                                               [chatList](const auto &position) {
                                                   auto list = position.toMap().value("list").toMap();
                                                   return list.value("@type").toByteArray() == "chatListFilter" &&
                                                          list.value("chat_filter_id").toInt() == chatList.value("chat_filter_id").toInt();
                                               });
                it != positions.end())
                return it->toMap();

            break;
        }
    }

    return {};
}

bool ChatModel::isChatMuted(const QVariantMap &chat) const noexcept
{
    return getChatMuteFor(chat) > 0;
}

int ChatModel::getChatMuteFor(const QVariantMap &chat) const noexcept
{
    auto notificationSettings = chat.value("notification_settings").toMap();

    return notificationSettings.value("mute_for").toInt();
}

bool ChatModel::isChatPinned(const QVariantMap &chat, const QVariantMap &chatList) const noexcept
{
    auto position = getChatPosition(chat, chatList);

    return position.value("is_pinned").toBool();
}

qint64 ChatModel::getChatOrder(qint64 chatId, const QVariantMap &chatList) const noexcept
{
    auto position = getChatPosition(m_storageManager->getChat(chatId), chatList);

    return position.value("order").toLongLong();
}

bool ChatModel::chatListEquals(const QVariantMap &list1, const QVariantMap &list2) const noexcept
{
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
}

QString ChatModel::getServiceMessageContent(const QVariantMap &message, bool openUser) const noexcept
{
    const auto ttl = message.value("ttl").toInt();
    const auto sender = message.value("sender_id").toMap();
    const auto content = message.value("content").toMap();
    const auto isOutgoing = message.value("is_outgoing").toBool();

    const auto chat = m_storageManager->getChat(message.value("chat_id").toLongLong());

    const auto isChannel = isChannelChat(chat);

    const auto author = getMessageAuthor(message, openUser);

    const auto getUserName = [openUser, this](qlonglong userId) {
        return openUser ? "<a style=\"text-decoration: none; font-weight: bold; color: grey \" href=\"userId://" + QString::number(userId) +
                              "\">" + getUserShortName(userId) + "</a>"
                        : getUserShortName(userId);
    };

    const auto contentType = content.value("@type").toByteArray();
    if (ttl > 0)
    {
        switch (fnv::hashRuntime(contentType.constData()))
        {
            case fnv::hash("messagePhoto"): {
                if (isOutgoing)
                {
                    return m_locale->getString("ActionYouSendTTLPhoto");
                }

                return m_locale->getString("ActionSendTTLPhoto").replace("un1", author);
            }
            case fnv::hash("messageVideo"): {
                if (isOutgoing)
                {
                    return m_locale->getString("ActionYouSendTTLVideo");
                }

                return m_locale->getString("ActionSendTTLVideo").replace("un1", author);
            }
        }
    }

    switch (fnv::hashRuntime(contentType.constData()))
    {
        case fnv::hash("messageExpiredPhoto"): {
            return m_locale->getString("AttachPhotoExpired");
        }
        case fnv::hash("messageExpiredVideo"): {
            return m_locale->getString("AttachVideoExpired");
        }
        case fnv::hash("messageBasicGroupChatCreate"): {
            if (isOutgoing)
            {
                return m_locale->getString("ActionYouCreateGroup");
            }

            return m_locale->getString("ActionCreateGroup").replace("un1", author);
        }
        case fnv::hash("messageSupergroupChatCreate"): {
            if (isChannel)
            {
                return m_locale->getString("ActionCreateChannel");
            }

            return m_locale->getString("ActionCreateMega");
        }
        case fnv::hash("messageChatChangeTitle"): {
            auto title = content.value("title").toString();

            if (isChannel)
            {
                return m_locale->getString("ActionChannelChangedTitle").replace("un2", title);
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouChangedTitle").replace("un2", title);
            }

            return m_locale->getString("ActionChangedTitle").replace("un1", author).replace("un2", title);
        }
        case fnv::hash("messageChatChangePhoto"): {
            if (isChannel)
            {
                return m_locale->getString("ActionChannelChangedPhoto");
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouChangedPhoto");
            }

            return m_locale->getString("ActionChangedPhoto").replace("un1", author);
        }
        case fnv::hash("messageChatDeletePhoto"): {
            if (isChannel)
            {
                return m_locale->getString("ActionChannelRemovedPhoto");
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouRemovedPhoto");
            }

            return m_locale->getString("ActionRemovedPhoto").replace("un1", author);
        }
        case fnv::hash("messageChatAddMembers"): {
            auto singleMember = content.value("member_user_ids").toList().length() == 1;

            if (singleMember)
            {
                auto memberUserId = content.value("member_user_ids").toList()[0].toLongLong();
                if (sender.value("user_id") == memberUserId)
                {
                    if (isSupergroup(chat) && isChannel)
                    {
                        return m_locale->getString("ChannelJoined");
                    }

                    if (isSupergroup(chat) && !isChannel)
                    {
                        if (isMeUser(memberUserId))
                        {
                            return m_locale->getString("ChannelMegaJoined");
                        }

                        return m_locale->getString("ActionAddUserSelfMega").replace("un1", author);
                    }

                    if (isOutgoing)
                    {
                        return m_locale->getString("ActionAddUserSelfYou");
                    }

                    return m_locale->getString("ActionAddUserSelf").replace("un1", author);
                }

                if (isOutgoing)
                {
                    return m_locale->getString("ActionYouAddUser").replace("un2", getUserName(memberUserId));
                }

                if (isMeUser(memberUserId))
                {
                    if (isSupergroup(chat))
                    {
                        if (!isChannel)
                        {
                            return m_locale->getString("MegaAddedBy").replace("un1", author);
                        }

                        return m_locale->getString("ChannelAddedBy").replace("un1", author);
                    }

                    return m_locale->getString("ActionAddUserYou").replace("un1", author);
                }

                return m_locale->getString("ActionAddUser").replace("un1", author).replace("un2", getUserName(memberUserId));
            }

            QStringList result;
            for (const auto &userId : content.value("member_user_ids").toList())
            {
                result << getUserName(userId.toLongLong());
            }

            auto users = result.join(", ");

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouAddUser").arg(users);
            }

            return m_locale->getString("ActionAddUser").replace("un1", author).replace("un2", users);
        }
        case fnv::hash("messageChatJoinByLink"): {
            if (isOutgoing)
            {
                return m_locale->getString("ActionInviteYou");
            }

            return m_locale->getString("ActionInviteUser").replace("un1", author);
        }
        case fnv::hash("messageChatDeleteMember"): {
            if (content.value("user_id").toLongLong() == sender.value("user_id").toLongLong())
            {
                if (isOutgoing)
                {
                    return m_locale->getString("ActionYouLeftUser");
                }

                return m_locale->getString("ActionLeftUser").replace("un1", author);
            }

            if (isOutgoing)
            {
                return m_locale->getString("ActionYouKickUser").replace("un2", getUserName(content.value("user_id").toLongLong()));
            }
            else if (isMeUser(content.value("user_id").toLongLong()))
            {
                return m_locale->getString("ActionKickUserYou").replace("un1", author);
            }

            return m_locale->getString("ActionKickUser")
                .replace("un1", author)
                .replace("un2", getUserName(content.value("user_id").toLongLong()));
        }
        case fnv::hash("messageChatUpgradeTo"): {
            return m_locale->getString("ActionMigrateFromGroup");
        }
        case fnv::hash("messageChatUpgradeFrom"): {
            return m_locale->getString("ActionMigrateFromGroup");
        }
        case fnv::hash("messagePinMessage"): {
            return m_locale->getString("ActionPinned").replace("un1", author);
        }
        case fnv::hash("messageScreenshotTaken"): {
            if (isOutgoing)
            {
                return m_locale->getString("ActionTakeScreenshootYou");
            }

            return m_locale->getString("ActionTakeScreenshoot").replace("un1", author);
        }
        case fnv::hash("messageChatSetTtl"): {
            auto ttlString = m_locale->formatTtl(content.value("ttl").toInt());

            if (content.value("ttl").toInt() <= 0)
            {
                if (isOutgoing)
                    return m_locale->getString("MessageLifetimeYouRemoved");

                return m_locale->getString("MessageLifetimeRemoved").arg(getUserName(sender.value("user_id").toLongLong()));
            }

            if (isOutgoing)
                return m_locale->getString("MessageLifetimeChangedOutgoing").arg(ttlString);

            return m_locale->getString("MessageLifetimeChanged").arg(getUserName(sender.value("user_id").toLongLong())).arg(ttlString);
        }
        case fnv::hash("messageCustomServiceAction"): {
            return content.value("text").toString();
        }
        case fnv::hash("messageContactRegistered"): {
            return m_locale->getString("NotificationContactJoined").arg(getUserName(sender.value("user_id").toLongLong()));
        }
        case fnv::hash("messageWebsiteConnected"): {
            return m_locale->getString("ActionBotAllowed");
        }
        case fnv::hash("messageUnsupported"): {
            return m_locale->getString("UnsupportedMedia");
        }
    }

    return m_locale->getString("UnsupportedMedia");
}

QString ChatModel::getContent(const QVariantMap &message) const noexcept
{
    auto content = message.value("content").toMap();
    auto sender = message.value("sender_id").toMap();

    auto textOneLine = [](QString text) { return text.replace("\n", " ").replace("\r", " "); };

    QString caption;
    if (!content.value("caption").toMap().value("text").toString().isEmpty())
        caption.append(": ").append(textOneLine(content.value("caption").toMap().value("text").toString()));

    if (message.value("ttl").toInt() > 0)
        return getServiceMessageContent(message);

    auto contentType = content.value("@type").toByteArray();
    switch (fnv::hashRuntime(contentType.constData()))
    {
        case fnv::hash("messageAnimation"): {
            return m_locale->getString("AttachGif").append(caption);
        }
        case fnv::hash("messageAudio"): {
            auto audio = content.value("audio").toMap();
            auto title = getAudioTitle(audio).isEmpty() ? m_locale->getString("AttachMusic") : getAudioTitle(audio);

            return title.append(caption);
        }
        case fnv::hash("messageBasicGroupChatCreate"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageCall"): {
            auto text = getCallContent(sender, content);

            if (auto duration = content.value("duration").toInt(); duration > 0)
            {
                return m_locale->getString("CallMessageWithDuration").arg(text).arg(m_locale->formatCallDuration(duration));
            }

            return text;
        }
        case fnv::hash("messageChatAddMembers"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatChangePhoto"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatChangeTitle"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatDeleteMember"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatDeletePhoto"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatJoinByLink"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatSetTtl"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatUpgradeFrom"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageChatUpgradeTo"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageContact"): {
            return m_locale->getString("AttachContact").append(caption);
        }
        case fnv::hash("messageContactRegistered"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageCustomServiceAction"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageDocument"): {
            auto document = content.value("document").toMap();
            auto fileName = document.value("file_name").toString();
            if (!fileName.isEmpty())
            {
                return fileName.append(caption);
            }

            return m_locale->getString("AttachDocument").append(caption);
        }
        case fnv::hash("messageExpiredPhoto"): {
            return m_locale->getString("AttachPhoto").append(caption);
        }
        case fnv::hash("messageExpiredVideo"): {
            return m_locale->getString("AttachVideo").append(caption);
        }
        case fnv::hash("messageGame"): {
            return m_locale->getString("AttachGame").append(caption);
        }
        case fnv::hash("messageGameScore"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageInvoice"): {
            auto title = content.value("title").toString();
            return title.append(caption);
        }
        case fnv::hash("messageLocation"): {
            return m_locale->getString("AttachLocation").arg(caption);
        }
        case fnv::hash("messagePassportDataReceived"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messagePassportDataSent"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messagePaymentSuccessful"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messagePaymentSuccessfulBot"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messagePhoto"): {
            return m_locale->getString("AttachPhoto").append(caption);
        }
        case fnv::hash("messagePoll"): {
            auto poll = content.value("poll").toMap();
            auto question = poll.value("question").toString();

            return QString::fromUtf8("\xf0\x9f\x93\x8a\x20").append(question);
        }
        case fnv::hash("messagePinMessage"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageScreenshotTaken"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageSticker"): {
            auto sticker = content.value("sticker").toMap();
            auto emoji = sticker.value("emoji").toString();

            return m_locale->getString("AttachSticker").append(": ").append(emoji);
        }
        case fnv::hash("messageSupergroupChatCreate"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageText"): {
            return textOneLine(content.value("text").toMap().value("text").toString());
        }
        case fnv::hash("messageUnsupported"): {
            return getServiceMessageContent(message);
        }
        case fnv::hash("messageVenue"): {
            return m_locale->getString("AttachLocation").append(caption);
        }
        case fnv::hash("messageVideo"): {
            return m_locale->getString("AttachVideo").append(caption);
        }
        case fnv::hash("messageVideoNote"): {
            return m_locale->getString("AttachRound").append(caption);
        }
        case fnv::hash("messageVoiceNote"): {
            return m_locale->getString("AttachAudio").append(caption);
        }
        case fnv::hash("messageWebsiteConnected"): {
            return getServiceMessageContent(message);
        }
    }

    return m_locale->getString("UnsupportedAttachment");
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

    std::ranges::sort(m_chatIds, [&](auto a, auto b) { return getChatOrder(a, m_list) > getChatOrder(b, m_list); });

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
