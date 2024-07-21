#include "MessageModel.hpp"

#include "BasicGroup.hpp"
#include "Chat.hpp"
#include "ChatModel.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "Message.hpp"
#include "Serialize.hpp"
#include "StorageManager.hpp"
#include "Supergroup.hpp"
#include "SupergroupFullInfo.hpp"
#include "User.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QTimer>

#include <algorithm>
#include <utility>

namespace detail {
QString getBasicGroupStatus(const BasicGroup *basicGroup, const QVariantMap &chat, Locale *locale) noexcept
{
    const auto statusType = basicGroup->status().value("@type").toByteArray();
    const auto count = basicGroup->memberCount();

    if (statusType == "chatMemberStatusBanned")
    {
        return locale->getString("YouWereKicked");
    }

    const auto memberString = locale->formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    const auto onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return memberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getChannelStatus(const Supergroup *supergroup, const QVariantMap &chat, StorageManager *store, Locale *locale) noexcept
{
    const auto isChannel = supergroup->isChannel();
    if (!isChannel)
    {
        return QString();
    }

    auto count = supergroup->memberCount();
    const auto username = supergroup->usernames();  // ???

    nlohmann::json json(username);
    qDebug() << QString::fromStdString(json.dump());

    if (count == 0)
    {
        const auto fullInfo = store->getSupergroupFullInfo(supergroup->id());
        count = fullInfo->memberCount();
    }

    if (count <= 0)
    {
        return !username.isEmpty() ? locale->getString("ChannelPublic") : locale->getString("ChannelPrivate");
    }

    const auto subscriberString = locale->formatPluralString("Subscribers", count);
    if (count <= 1)
    {
        return subscriberString;
    }

    const auto onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return subscriberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return subscriberString;
}

QString getSupergroupStatus(const Supergroup *supergroup, const QVariantMap &chat, StorageManager *store, Locale *locale) noexcept
{
    const auto statusType = supergroup->status().value("@type").toByteArray();
    const auto username = supergroup->usernames();
    const auto hasLocation = supergroup->hasLocation();

    nlohmann::json json(username);
    qDebug() << QString::fromStdString(json.dump());

    auto count = supergroup->memberCount();

    if (statusType == "chatMemberStatusBanned")
    {
        return locale->getString("YouWereKicked");
    }

    if (count == 0)
    {
        const auto fullInfo = store->getSupergroupFullInfo(supergroup->id());
        count = fullInfo->memberCount();
    }

    if (count <= 0)
    {
        if (hasLocation)
        {
            return locale->getString("MegaLocation");
        }

        return !username.isEmpty() ? locale->getString("MegaPublic") : locale->getString("MegaPrivate");
    }

    const auto memberString = locale->formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    const auto onlineCount = chat.value("online_member_count").toInt();
    if (onlineCount > 1)
    {
        return memberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getUserStatus(const User *user, Locale *locale) noexcept
{
    if (std::ranges::any_of(ServiceNotificationsUserIds, [user](auto id) { return id == user->id(); }))
    {
        return locale->getString("ServiceNotifications");
    }

    if (user->isSupport())
    {
        return locale->getString("SupportStatus");
    }

    const auto userType = user->type().value("@type").toByteArray();
    if (userType == "userTypeBot")
    {
        return locale->getString("Bot");
    }

    const auto statusType = user->status().value("@type").toByteArray();
    if (statusType == "userStatusEmpty")
    {
        return locale->getString("ALongTimeAgo");
    }
    else if (statusType == "userStatusLastMonth")
    {
        return locale->getString("WithinAMonth");
    }
    else if (statusType == "userStatusLastWeek")
    {
        return locale->getString("WithinAWeek");
    }
    else if (statusType == "userStatusOffline")
    {
        const auto was_online = user->status().value("was_online").toLongLong();
        if (was_online == 0)
        {
            return locale->getString("Invisible");
        }

        const auto wasOnline = QDateTime::fromMSecsSinceEpoch(was_online * 1000);
        const auto currentDate = QDate::currentDate();

        if (currentDate == wasOnline.date())  // TODAY
        {
            return locale->getString("LastSeenFormatted")
                .arg(locale->getString("TodayAtFormatted"))
                .arg(wasOnline.toString(locale->getString("formatterDay12H")));
        }
        else if (wasOnline.date().daysTo(currentDate) < 2)  // YESTERDAY
        {
            return locale->getString("LastSeenFormatted")
                .arg(locale->getString("YesterdayAtFormatted"))
                .arg(wasOnline.toString(locale->getString("formatterDay12H")));
        }

        // OTHER DAYS
        return locale->getString("LastSeenDateFormatted")
            .arg(locale->getString("formatDateAtTime")
                     .arg(wasOnline.toString(locale->getString("formatterYear")))
                     .arg(wasOnline.toString(locale->getString("formatterDay12H"))));
    }
    else if (statusType == "userStatusOnline")
    {
        return locale->getString("Online");
    }
    else if (statusType == "userStatusRecently")
    {
        return locale->getString("Lately");
    }

    return QString();
}

}  // namespace detail

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    setRoleNames(roleNames());
}

QObject *MessageModel::storageManager() const
{
    return m_storageManager;
}

void MessageModel::setStorageManager(QObject *storageManager)
{
    m_storageManager = qobject_cast<StorageManager *>(storageManager);
    m_client = qobject_cast<Client *>(m_storageManager->client());

    connect(m_client, SIGNAL(result(const QVariantMap &)), SLOT(handleResult(const QVariantMap &)));

    qDebug() << __PRETTY_FUNCTION__;

    statusChanged();

    loadMessages();
}

QObject *MessageModel::locale() const
{
    return m_locale;
}

void MessageModel::setLocale(QObject *locale)
{
    m_locale = qobject_cast<Locale *>(locale);
}

Chat *MessageModel::selectedChat() const
{
    return m_selectedChat;
}

void MessageModel::setSelectedChat(Chat *value)
{
    if (m_selectedChat != value)
    {
        m_selectedChat = value;
    }
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_messages.count();
}

bool MessageModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;

    if (m_messages.size() > 0)
        return !m_loading && m_selectedChat->lastMessage()->id() != *std::ranges::max_element(m_messageIds);

    return false;
}

void MessageModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    if (!m_loading)
    {
        if (auto max = std::ranges::max_element(m_messageIds); max != m_messageIds.end())
        {
            getChatHistory(*max, -MessageSliceLimit, MessageSliceLimit);
        }

        m_loading = true;

        emit loadingChanged();
    }
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!m_client)
        return QVariant();

    if (!index.isValid())
        return QVariant();

    const auto &message = m_messages.at(index.row());
    switch (role)
    {
        case IdRole:
            return message->id();
        case SenderRole: {
            if (message->isOutgoing())
                return QString();

            return Utils::getTitle(message, m_storageManager, m_locale);
        }
        case ChatIdRole:
            return message->chatId();
        case SendingStateRole:
            return message->sendingState();
        case SchedulingStateRole:
            return message->schedulingState();
        case IsOutgoingRole:
            return message->isOutgoing();
        case IsPinnedRole:
            return message->isPinned();
        case CanBeEditedRole:
            return message->canBeEdited();
        case CanBeForwardedRole:
            return message->canBeForwarded();
        case CanBeDeletedOnlyForSelfRole:
            return message->canBeDeletedOnlyForSelf();
        case CanBeDeletedForAllUsersRole:
            return message->canBeDeletedForAllUsers();
        case CanGetStatisticsRole:
            return message->canGetStatistics();
        case CanGetMessageThreadRole:
            return message->canGetMessageThread();
        case IsChannelPostRole:
            return message->isChannelPost();
        case ContainsUnreadMentionRole:
            return message->containsUnreadMention();
        case DateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(message->date() * 1000);

            return date.toString(m_locale->getString("formatterDay12H"));
        }
        case EditDateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(message->editDate() * 1000);

            return date.toString(m_locale->getString("formatterDay12H"));
        }
        case ForwardInfoRole:
            return message->forwardInfo();
        case InteractionInfoRole:
            return message->interactionInfo();
        case MessageThreadIdRole:
            return message->messageThreadId();
        case ViaBotUserIdRole:
            return message->viaBotUserId();
        case AuthorSignatureRole:
            return message->authorSignature();
        case MediaAlbumIdRole:
            return message->mediaAlbumId();
        case RestrictionReasonRole:
            return message->restrictionReason();
        case ContentRole:
            return message->content();
        case ReplyMarkupRole:
            return message->replyMarkup();

        case BubbleColorRole:
            return QVariant();
        case IsServiceMessageRole: {
            return Utils::isServiceMessage(message);
        }
        case SectionRole: {
            const auto date = QDateTime::fromMSecsSinceEpoch(message->date() * 1000);

            const auto days = date.daysTo(QDateTime::currentDateTime());

            if (days == 0)
                return m_locale->getString("Today");
            else if (days < 2)
                return m_locale->getString("Yesterday");

            return date.toString(m_locale->getString("chatFullDate"));
        }
        case ServiceMessageRole: {
            return Utils::getServiceMessageContent(message, m_storageManager, m_locale, true);
        }
    }
    return QVariant();
}

QHash<int, QByteArray> MessageModel::roleNames() const noexcept
{
    QHash<int, QByteArray> roles;
    roles[IdRole] = "id";
    roles[SenderRole] = "sender";
    roles[ChatIdRole] = "chatId";
    roles[SendingStateRole] = "sendingState";
    roles[SchedulingStateRole] = "schedulingState";
    roles[IsOutgoingRole] = "isOutgoing";
    roles[IsPinnedRole] = "isPinned";
    roles[CanBeEditedRole] = "canBeEdited";
    roles[CanBeForwardedRole] = "canBeForwarded";
    roles[CanBeDeletedOnlyForSelfRole] = "canBeDeletedOnlyForSelf";
    roles[CanBeDeletedForAllUsersRole] = "canBeDeletedForAllUsers";
    roles[CanGetStatisticsRole] = "canGetStatistics";
    roles[CanGetMessageThreadRole] = "canGetMessageThread";
    roles[IsChannelPostRole] = "isChannelPost";
    roles[ContainsUnreadMentionRole] = "containsUnreadMention";
    roles[DateRole] = "date";
    roles[EditDateRole] = "editDate";
    roles[ForwardInfoRole] = "forwardInfo";
    roles[InteractionInfoRole] = "interactionInfo";
    roles[MessageThreadIdRole] = "messageThreadId";
    roles[ViaBotUserIdRole] = "viaBotUserId";
    roles[AuthorSignatureRole] = "authorSignature";
    roles[MediaAlbumIdRole] = "mediaAlbumId";
    roles[RestrictionReasonRole] = "restrictionReason";
    roles[ContentRole] = "content";
    roles[ReplyMarkupRole] = "replyMarkup";
    // Custom
    roles[BubbleColorRole] = "bubbleColor";
    roles[IsServiceMessageRole] = "isServiceMessage";
    roles[SectionRole] = "section";
    roles[ServiceMessageRole] = "serviceMessage";
    return roles;
}

int MessageModel::count() const noexcept
{
    return m_messages.count();
}

bool MessageModel::loading() const noexcept
{
    return m_loading;
}

bool MessageModel::loadingHistory() const noexcept
{
    return m_loadingHistory;
}

QString MessageModel::getChatSubtitle() const noexcept
{
    if (!m_client)
        return QString();

    static const std::unordered_map<std::string, std::function<QString(const QVariantMap &, const Chat *, StorageManager *, Locale *)>>
        chatTypeHandlers = {{"chatTypeBasicGroup",
                             [](const QVariantMap &type, const Chat *, StorageManager *store, Locale *locale) {
                                 return detail::getBasicGroupStatus(store->getBasicGroup(type.value("basic_group_id").toLongLong()), {},
                                                                    locale);
                             }},
                            {"chatTypePrivate",
                             [](const QVariantMap &type, const Chat *, StorageManager *store, Locale *locale) {
                                 return detail::getUserStatus(store->getUser(type.value("user_id").toLongLong()), locale);
                             }},
                            {"chatTypeSecret",
                             [](const QVariantMap &type, const Chat *, StorageManager *store, Locale *locale) {
                                 return detail::getUserStatus(store->getUser(type.value("user_id").toLongLong()), locale);
                             }},
                            {"chatTypeSupergroup", [](const QVariantMap &type, const Chat *, StorageManager *store, Locale *locale) {
                                 const auto supergroup = store->getSupergroup(type.value("supergroup_id").toLongLong());
                                 return supergroup->isChannel() ? detail::getChannelStatus(supergroup, {}, store, locale)
                                                                : detail::getSupergroupStatus(supergroup, {}, store, locale);
                             }}};

    const auto type = m_selectedChat->type();
    const auto chatType = m_selectedChat->type().value("@type").toString();

    if (auto it = chatTypeHandlers.find(chatType.toStdString()); it != chatTypeHandlers.end())
    {
        return it->second(type, {}, m_storageManager, m_locale);
    }

    return QString();
}

QString MessageModel::getChatTitle() const noexcept
{
    if (!m_client)
        return QString();

    if (const auto title = m_selectedChat->title(); !Utils::isMeChat(m_selectedChat, m_storageManager))
        return title.isEmpty() ? m_locale->getString("HiddenName") : title;

    return m_locale->getString("SavedMessages");
}

QString MessageModel::getChatPhoto() const noexcept
{
    if (!m_client)
        return QString();

    const auto smallPhoto = m_selectedChat->photo().value("small").toMap();
    const auto localPhoto = smallPhoto.value("local").toMap();

    if (localPhoto.value("is_downloading_completed").toBool())
    {
        return "image://chatPhoto/" + localPhoto.value("path").toString();
    }

    return "image://theme/icon-l-content-avatar-placeholder";
}

QString MessageModel::getFormattedText(const QVariantMap &formattedText, const QVariantMap &options) noexcept
{
    if (!m_client)
        return QString();

    return Utils::getFormattedText(formattedText, m_storageManager, m_locale, options);
}

void MessageModel::loadHistory() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (auto min = std::ranges::min_element(m_messageIds); min != m_messageIds.end() && !m_loadingHistory)
    {
        m_loadingHistory = true;

        getChatHistory(*min, 0, MessageSliceLimit);

        emit loadingChanged();
    }
}

void MessageModel::openChat() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_client)
        return;

    QVariantMap request;
    request.insert("@type", "openChat");
    request.insert("chat_id", m_selectedChat->id());

    m_client->send(request);
}

void MessageModel::closeChat() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_client)
        return;

    QVariantMap request;
    request.insert("@type", "closeChat");
    request.insert("chat_id", m_selectedChat->id());

    m_client->send(request);
}

void MessageModel::getChatHistory(qint64 fromMessageId, qint32 offset, qint32 limit)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_client)
        return;

    QVariantMap request;
    request.insert("@type", "getChatHistory");
    request.insert("chat_id", m_selectedChat->id());
    request.insert("from_message_id", fromMessageId);
    request.insert("offset", offset);
    request.insert("limit", limit);
    request.insert("only_local", false);

    m_client->send(request, [this](const auto &messages) { handleMessages(messages); });
}

void MessageModel::sendMessage(const QString &message, qint64 replyToMessageId)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_client)
        return;

    QVariantMap formattedText, inputMessageContent;
    formattedText.insert("@type", "formattedText");
    formattedText.insert("text", message);

    inputMessageContent.insert("@type", "inputMessageText");
    inputMessageContent.insert("text", formattedText);

    QVariantMap request;
    request.insert("@type", "sendMessage");
    request.insert("chat_id", m_selectedChat->id());

    if (replyToMessageId != 0)
    {
        request.insert("reply_to_message_id", replyToMessageId);
    }

    request.insert("input_message_content", inputMessageContent);

    m_client->send(request);
}

void MessageModel::viewMessages(const QVariantList &messageIds)
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_client)
        return;

    QVariantMap request;
    request.insert("@type", "viewMessages");
    request.insert("chat_id", m_selectedChat->id());
    request.insert("message_thread_id", 0);
    request.insert("message_ids", messageIds);
    request.insert("force_read", true);

    m_client->send(request);
}

void MessageModel::deleteMessage(qint64 messageId, bool revoke) noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_client)
        return;

    QVariantMap request;
    request.insert("@type", "deleteMessages");
    request.insert("chat_id", m_selectedChat->id());
    request.insert("message_ids", QVariantList() << messageId);
    request.insert("revoke", revoke);

    m_client->send(request);
}

void MessageModel::refresh() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (m_messages.isEmpty())
        return;

    m_loading = true;
    m_loadingHistory = true;

    beginResetModel();
    m_messages.clear();
    m_messageIds.clear();
    endResetModel();

    emit countChanged();
}

void MessageModel::handleResult(const QVariantMap &object)
{
    static const std::unordered_map<std::string, std::function<void(const QVariantMap &)>> handlers = {
        {"updateNewMessage", [this](const QVariantMap &obj) { handleNewMessage(obj.value("message").toMap()); }},
        {"updateMessageSendSucceeded",
         [this](const QVariantMap &obj) {
             handleMessageSendSucceeded(obj.value("message").toMap(), obj.value("old_message_id").toLongLong());
         }},
        {"updateMessageSendFailed",
         [this](const QVariantMap &obj) {
             handleMessageSendFailed(obj.value("message").toMap(), obj.value("old_message_id").toLongLong(),
                                     obj.value("error_code").toInt(), obj.value("error_message").toString());
         }},
        {"updateMessageContent",
         [this](const QVariantMap &obj) {
             handleMessageContent(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(),
                                  obj.value("new_content").toMap());
         }},
        {"updateMessageEdited",
         [this](const QVariantMap &obj) {
             handleMessageEdited(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(), obj.value("edit_date").toInt(),
                                 obj.value("reply_markup").toMap());
         }},
        {"updateMessageIsPinned",
         [this](const QVariantMap &obj) {
             handleMessageIsPinned(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(),
                                   obj.value("is_pinned").toBool());
         }},
        {"updateMessageInteractionInfo",
         [this](const QVariantMap &obj) {
             handleMessageInteractionInfo(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(),
                                          obj.value("interaction_info").toMap());
         }},
    };

    const auto objectType = object.value("@type").toString().toStdString();

    if (const auto it = handlers.find(objectType); it != handlers.end())
    {
        it->second(object);
    }
}

void MessageModel::handleNewMessage(const QVariantMap &message)
{
    if (m_selectedChat->id() != message.value("chat_id").toLongLong())
        return;

    if (auto lastMessageId = m_selectedChat->lastMessage()->id(); m_messageIds.contains(lastMessageId))
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        Message *msg = new Message(this);
        msg->setFromVariantMap(message);

        m_messages.append(msg);
        m_messageIds.insert(msg->id());
        endInsertRows();

        viewMessages(QVariantList() << message.value("id").toLongLong());

        emit countChanged();
    }
}

void MessageModel::handleMessageSendSucceeded(const QVariantMap &message, qint64 oldMessageId)
{
}

void MessageModel::handleMessageSendFailed(const QVariantMap &message, qint64 oldMessageId, int errorCode, const QString &errorMessage)
{
}

void MessageModel::handleMessageContent(qint64 chatId, qint64 messageId, const QVariantMap &newContent)
{
    if (chatId != m_selectedChat->id())
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto *message) { return message->id() == messageId; });

    if (it != m_messages.end())
    {
        (*it)->setContent(newContent);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleMessageEdited(qint64 chatId, qint64 messageId, int editDate, const QVariantMap &replyMarkup)
{
    if (chatId != m_selectedChat->id())
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto *message) { return message->id() == messageId; });

    if (it != m_messages.end())
    {
        (*it)->setEditDate(editDate);
        (*it)->setReplyMarkup(replyMarkup);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleMessageIsPinned(qint64 chatId, qint64 messageId, bool isPinned)
{
    if (chatId != m_selectedChat->id())
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto *message) { return message->id() == messageId; });

    if (it != m_messages.end())
    {
        (*it)->setIsPinned(isPinned);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleMessageInteractionInfo(qint64 chatId, qint64 messageId, const QVariantMap &interactionInfo)
{
    if (chatId != m_selectedChat->id())
        return;

    auto it = std::ranges::find_if(m_messages, [messageId](const auto *message) { return message->id() == messageId; });

    if (it != m_messages.end())
    {
        (*it)->setInteractionInfo(interactionInfo);

        auto index = std::distance(m_messages.begin(), it);
        itemChanged(index);
    }
}

void MessageModel::handleDeleteMessages(qint64 chatId, const QVariantList &messageIds, bool isPermanent, bool fromCache)
{
    if (chatId != m_selectedChat->id())
        return;

    QListIterator<QVariant> it(messageIds);
    while (it.hasNext())
    {
        auto result = std::ranges::find_if(m_messages, [&](const auto *message) { return message->id() == it.next().toLongLong(); });

        if (result != m_messages.end())
        {
            auto index = std::distance(m_messages.begin(), result);

            beginRemoveRows(QModelIndex(), index, index);
            m_messages.removeAt(index);
            endRemoveRows();
        }
    }
}

void MessageModel::handleChatOnlineMemberCount(qint64 chatId, int onlineMemberCount)
{
    // if (chatId == m_chatId.toLongLong())
    // {
    //     m_chat.insert("online_member_count", onlineMemberCount);

    //     emit chatIdChanged();
    //     emit statusChanged();
    // }
}

void MessageModel::handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount)
{
    if (chatId == m_selectedChat->id())
    {
        m_selectedChat->setLastReadInboxMessageId(lastReadInboxMessageId);
        m_selectedChat->setUnreadCount(unreadCount);

        emit selectedChatChanged();
    }
}

void MessageModel::handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId)
{
    if (chatId != m_selectedChat->id())
    {
        m_selectedChat->setLastReadOutboxMessageId(lastReadOutboxMessageId);
        emit selectedChatChanged();
    }
}

void MessageModel::handleMessages(const QVariantMap &messages)
{
    // const auto list = messages.value("messages").toList();

    // QVariantList result;
    // std::ranges::copy_if(list, std::back_inserter(result), [this](const auto &message) {
    //     return !m_messageIds.contains(message.toMap().value("id").toLongLong()) && message.toMap().value("chat_id").toString() == m_chatId;
    // });

    // if (result.isEmpty())
    // {
    //     m_loadingHistory ? m_loadingHistory = false : m_loading = false;
    //     emit loadingChanged();
    // }
    // else
    // {
    //     std::sort(result.begin(), result.end(),
    //               [](const auto &a, const auto &b) { return a.toMap().value("id").toLongLong() < b.toMap().value("id").toLongLong(); });

    //     insertMessages(result);
    // }

    // emit countChanged();
}

void MessageModel::insertMessages(const QVariantList &messages) noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    // if (m_loadingHistory)
    // {
    //     m_loadingHistory = false;

    //     int offset = 0;

    //     beginInsertRows(QModelIndex(), 0, messages.count() - 1);

    //     std::ranges::for_each(messages, [this, &offset](const auto &message) {
    //         m_messages.insert(offset, message.toMap());
    //         m_messageIds.emplace(message.toMap().value("id").toLongLong());
    //         ++offset;
    //     });

    //     endInsertRows();

    //     if (offset > 0)
    //         emit moreHistoriesLoaded(offset);
    // }

    // if (m_loading)
    // {
    //     m_loading = false;

    //     beginInsertRows(QModelIndex(), rowCount(), rowCount() + messages.count() - 1);

    //     std::ranges::for_each(messages, [this](const auto &message) {
    //         m_messages.append(message.toMap());
    //         m_messageIds.emplace(message.toMap().value("id").toLongLong());
    //     });

    //     endInsertRows();

    //     QVariantList messageIds;

    //     std::ranges::for_each(messages, [this, &messageIds](const auto &message) {
    //         if (m_chat->unreadCount() > 0)
    //         {
    //             auto id = message.toMap().value("id").toLongLong();
    //             if (!message.toMap().value("is_outgoing").toBool() && id > m_chat->lastReadInboxMessageId())
    //             {
    //                 messageIds.append(id);
    //             }
    //         }
    //     });

    //     if (messageIds.size() > 0)
    //         viewMessages(messageIds);
    // }

    // emit loadingChanged();
}

void MessageModel::loadMessages() noexcept
{
    qDebug() << __PRETTY_FUNCTION__;

    if (!m_selectedChat)
        return;

    const auto unread = m_selectedChat->unreadCount() > 0;

    const auto fromMessageId = unread ? m_selectedChat->lastReadInboxMessageId() : m_selectedChat->lastMessage()->id();

    const auto offset = unread ? -1 - MessageSliceLimit : 0;
    const auto limit = unread ? 2 * MessageSliceLimit : MessageSliceLimit;

    getChatHistory(fromMessageId, offset, limit);
}

void MessageModel::itemChanged(int64_t index)
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}
