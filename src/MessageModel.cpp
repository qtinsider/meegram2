#include "MessageModel.hpp"

#include "ChatModel.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QTimer>

#include <algorithm>
#include <utility>

namespace {
QString getBasicGroupStatus(const td::td_api::basicGroup *basicGroup, int onlineCount, Locale *locale) noexcept
{
    auto count = basicGroup->member_count_;

    if (basicGroup->status_->get_id() == td::td_api::chatMemberStatusBanned::ID)
    {
        return locale->getString("YouWereKicked");
    }

    const auto memberString = locale->formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    if (onlineCount > 1)
    {
        return memberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getChannelStatus(const td::td_api::supergroup *supergroup, int onlineCount, StorageManager *store, Locale *locale) noexcept
{
    if (!supergroup->is_channel_)
    {
        return QString();
    }

    auto count = supergroup->member_count_;
    const auto usernames = supergroup->usernames_;  // ???

    if (count == 0)
    {
        count = store->getSupergroupFullInfo(supergroup->id_)->member_count_;
    }

    if (count <= 0)
    {
        return usernames ? locale->getString("ChannelPublic") : locale->getString("ChannelPrivate");
    }

    const auto subscriberString = locale->formatPluralString("Subscribers", count);
    if (count <= 1)
    {
        return subscriberString;
    }

    if (onlineCount > 1)
    {
        return subscriberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return subscriberString;
}

QString getSupergroupStatus(const td::td_api::supergroup *supergroup, int onlineCount, StorageManager *store, Locale *locale) noexcept
{
    const auto usernames = supergroup->usernames_;
    const auto hasLocation = supergroup->has_location_;

    auto count = supergroup->member_count_;

    if (supergroup->status_->get_id() == td::td_api::chatMemberStatusBanned::ID)
    {
        return locale->getString("YouWereKicked");
    }

    if (count == 0)
    {
        const auto fullInfo = store->getSupergroupFullInfo(supergroup->id_);
        count = fullInfo->member_count_;
    }

    if (count <= 0)
    {
        if (hasLocation)
        {
            return locale->getString("MegaLocation");
        }

        return usernames ? locale->getString("MegaPublic") : locale->getString("MegaPrivate");
    }

    const auto memberString = locale->formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    if (onlineCount > 1)
    {
        return memberString + ", " + locale->formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getUserStatus(const td::td_api::user *user, Locale *locale) noexcept
{
    if (std::ranges::any_of(ServiceNotificationsUserIds, [user](auto id) { return id == user->id_; }))
    {
        return locale->getString("ServiceNotifications");
    }

    if (user->is_support_)
    {
        return locale->getString("SupportStatus");
    }

    if (user->type_->get_id() == td::td_api::userTypeBot::ID)
    {
        return locale->getString("Bot");
    }

    switch (user->status_->get_id())
    {
        case td::td_api::userStatusEmpty::ID: {
            return locale->getString("ALongTimeAgo");
        }
        case td::td_api::userStatusLastMonth::ID: {
            return locale->getString("WithinAMonth");
        }
        case td::td_api::userStatusLastWeek::ID: {
            return locale->getString("WithinAWeek");
        }
        case td::td_api::userStatusOffline::ID: {
            auto was_online = static_cast<const td::td_api::userStatusOffline &>(*user->status_).was_online_;
            if (was_online == 0)
            {
                return locale->getString("Invisible");
            }

            auto wasOnline = QDateTime::fromMSecsSinceEpoch(was_online * 1000);
            auto currentDate = QDate::currentDate();

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

        case td::td_api::userStatusOnline::ID: {
            return locale->getString("Online");
        }
        case td::td_api::userStatusRecently::ID: {
            return locale->getString("Lately");
        }
        default:
            return QString();
    }
}

}  // namespace

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
{
    setRoleNames(roleNames());
}

StorageManager *MessageModel::storageManager() const
{
    return m_storageManager;
}

void MessageModel::setStorageManager(StorageManager *storageManager)
{
    m_storageManager = qobject_cast<StorageManager *>(storageManager);
    m_client = qobject_cast<Client *>(m_storageManager->client());

    connect(m_client, SIGNAL(result(const QVariantMap &)), SLOT(handleResult(const QVariantMap &)));
}

Locale *MessageModel::locale() const
{
    return m_locale;
}

void MessageModel::setLocale(Locale *locale)
{
    m_locale = qobject_cast<Locale *>(locale);
}

Chat *MessageModel::selectedChat() const
{
    return m_selectedChat;
}

void MessageModel::setSelectedChat(const td::td_api::chat *value)
{
    if (m_selectedChat != value)
    {
        m_selectedChat = value;

        emit selectedChatChanged();
    }
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_messages.size();
}

bool MessageModel::canFetchMore(const QModelIndex &parent) const
{
    if (parent.isValid())
        return false;

    if (!m_messages.empty())
    {
        if (auto max = std::ranges::max(m_messageIds); max.has_value())
        {
            return !m_loading /*&& m_selectedChat->lastMessage()->id() != *max*/;
        }
    }

    return false;
}

void MessageModel::fetchMore(const QModelIndex &parent)
{
    if (parent.isValid())
        return;

    if (!m_loading)
    {
        if (auto max = std::ranges::max(m_messageIds); max.has_value())
        {
            getChatHistory(*max, -MessageSliceLimit, MessageSliceLimit);
        }

        m_loading = true;
        emit loadingChanged();
    }
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto &message = m_messages.at(index.row());
    switch (role)
    {
        case IdRole:
            return message->id_;
        case SenderRole: {
            if (message->is_out_going_)
                return QString();

            return Utils::getTitle(*message, m_storageManager, m_locale);
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
            auto date = QDateTime::fromMSecsSinceEpoch(message->date_ * 1000);

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
            return Utils::isServiceMessage(message.get());
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
            return Utils::getServiceMessageContent(message.get(), m_storageManager, m_locale, true);
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
    return m_messages.size();
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
    static const std::unordered_map<QString, std::function<QString(const QVariantMap &, int, StorageManager *, Locale *)>> chatTypeHandlers = {
        {"chatTypeBasicGroup",
         [](const QVariantMap &type, int onlineCount, StorageManager *store, Locale *locale) {
             return detail::getBasicGroupStatus(store->getBasicGroup(type.value("basic_group_id").toLongLong()), onlineCount, locale);
         }},
        {"chatTypePrivate", [](const QVariantMap &type, int, StorageManager *store,
                               Locale *locale) { return detail::getUserStatus(store->getUser(type.value("user_id").toLongLong()), locale); }},
        {"chatTypeSecret", [](const QVariantMap &type, int, StorageManager *store,
                              Locale *locale) { return detail::getUserStatus(store->getUser(type.value("user_id").toLongLong()), locale); }},
        {"chatTypeSupergroup", [](const QVariantMap &type, int onlineCount, StorageManager *store, Locale *locale) {
             const auto supergroup = store->getSupergroup(type.value("supergroup_id").toLongLong());
             return supergroup->isChannel() ? detail::getChannelStatus(supergroup, onlineCount, store, locale)
                                            : detail::getSupergroupStatus(supergroup, onlineCount, store, locale);
         }}};

    const auto type = m_selectedChat->type();
    const auto chatType = m_selectedChat->type().value("@type").toString();

    if (auto it = chatTypeHandlers.find(chatType); it != chatTypeHandlers.end())
    {
        return it->second(type, m_onlineCount, m_storageManager, m_locale);
    }

    return QString();
}

QString MessageModel::getChatTitle() const noexcept
{
    if (const auto title = m_selectedChat->title(); !Utils::isMeChat(m_selectedChat, m_storageManager))
        return title.isEmpty() ? m_locale->getString("HiddenName") : title;

    return m_locale->getString("SavedMessages");
}

QString MessageModel::getChatPhoto() const noexcept
{
    const auto smallPhoto = m_selectedChat->photo().value("small").toMap();
    const auto localPhoto = smallPhoto.value("local").toMap();

    if (localPhoto.value("is_downloading_completed").toBool())
    {
        return "image://chatPhoto/" + localPhoto.value("path").toString();
    }

    return "image://theme/icon-l-content-avatar-placeholder";
}

void MessageModel::loadHistory() noexcept
{
    if (m_messageIds.empty())
    {
        qDebug() << "No messages available to load history.";
        return;
    }

    if (auto min = std::ranges::min(m_messageIds); min.has_value() && !m_loadingHistory)
    {
        qDebug() << "Loading history starting from message ID:" << *min;

        m_loadingHistory = true;

        getChatHistory(*min, 0, MessageSliceLimit);

        emit loadingChanged();
    }
    else
    {
        qDebug() << "No minimum message ID found or already loading history.";
    }
}

void MessageModel::openChat() noexcept
{
    QVariantMap request;
    request.insert("@type", "openChat");
    request.insert("chat_id", m_selectedChat->id());

    m_client->send(request);

    loadMessages();
}

void MessageModel::closeChat() noexcept
{
    QVariantMap request;
    request.insert("@type", "closeChat");
    request.insert("chat_id", m_selectedChat->id());

    m_client->send(request);
}

void MessageModel::getChatHistory(qint64 fromMessageId, qint32 offset, qint32 limit)
{
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
    QVariantMap request;
    request.insert("@type", "deleteMessages");
    request.insert("chat_id", m_selectedChat->id());
    request.insert("message_ids", QVariantList() << messageId);
    request.insert("revoke", revoke);

    m_client->send(request);
}

void MessageModel::refresh() noexcept
{
    if (m_messages.empty())
        return;

    m_loading = true;
    m_loadingHistory = true;

    beginResetModel();
    m_messages.clear();
    m_messageIds.clear();
    endResetModel();

    emit countChanged();
}

void MessageModel::handleResult(td::td_api::Object *object)
{
    static const std::unordered_map<QString, std::function<void(const QVariantMap &)>> handlers = {
        {"updateNewMessage", [this](const QVariantMap &obj) { handleNewMessage(obj.value("message").toMap()); }},
        {"updateMessageSendSucceeded",
         [this](const QVariantMap &obj) { handleMessageSendSucceeded(obj.value("message").toMap(), obj.value("old_message_id").toLongLong()); }},
        {"updateMessageSendFailed",
         [this](const QVariantMap &obj) {
             handleMessageSendFailed(obj.value("message").toMap(), obj.value("old_message_id").toLongLong(), obj.value("error_code").toInt(),
                                     obj.value("error_message").toString());
         }},
        {"updateMessageContent",
         [this](const QVariantMap &obj) {
             handleMessageContent(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(), obj.value("new_content").toMap());
         }},
        {"updateMessageEdited",
         [this](const QVariantMap &obj) {
             handleMessageEdited(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(), obj.value("edit_date").toInt(),
                                 obj.value("reply_markup").toMap());
         }},
        {"updateMessageIsPinned",
         [this](const QVariantMap &obj) {
             handleMessageIsPinned(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(), obj.value("is_pinned").toBool());
         }},
        {"updateMessageInteractionInfo",
         [this](const QVariantMap &obj) {
             handleMessageInteractionInfo(obj.value("chat_id").toLongLong(), obj.value("message_id").toLongLong(), obj.value("interaction_info").toMap());
         }},
        {"updateChatOnlineMemberCount",
         [this](const QVariantMap &obj) { handleChatOnlineMemberCount(obj.value("chat_id").toLongLong(), obj.value("online_member_count").toInt()); }},
        {"updateChatReadInbox",
         [this](const QVariantMap &obj) {
             handleChatReadInbox(obj.value("chat_id").toLongLong(), obj.value("last_read_inbox_message_id").toLongLong(), obj.value("online_count").toInt());
         }},
        {"updateChatReadOutbox",
         [this](const QVariantMap &obj) { handleChatReadOutbox(obj.value("chat_id").toLongLong(), obj.value("last_read_outbox_message_id").toLongLong()); }},
    };

    if (const auto it = handlers.find(object.value("@type").toString()); it != handlers.end())
    {
        it->second(object);
    }
}

void MessageModel::handleNewMessage(const QVariantMap &message)
{
    if (m_selectedChat->id() != message.value("chat_id").toLongLong())
        return;

    if (auto lastMessage = m_selectedChat->lastMessage(); m_messageIds.contains(lastMessage->id()))
    {
        beginInsertRows(QModelIndex(), rowCount(), rowCount());

        auto newMessage = std::make_unique<Message>();
        newMessage->setFromVariantMap(message);

        auto id = newMessage->id();
        m_messageIds.insert(id);

        m_messages.push_back(std::move(newMessage));

        endInsertRows();

        viewMessages(QVariantList() << id);

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

    auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message->id() == messageId; });

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

    if (auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message->id() == messageId; }); it != m_messages.end())
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

    if (auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message->id() == messageId; }); it != m_messages.end())
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

    if (auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message->id() == messageId; }); it != m_messages.end())
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

    for (const auto &value : messageIds)
    {
        auto messageId = value.toLongLong();
        if (auto it = std::ranges::find_if(m_messages, [messageId](const auto &message) { return message->id() == messageId; }); it != m_messages.end())
        {
            auto index = std::distance(m_messages.begin(), it);

            beginRemoveRows(QModelIndex(), index, index);
            m_messages.erase(it);
            endRemoveRows();
        }
    }
}

void MessageModel::handleChatOnlineMemberCount(qint64 chatId, int onlineMemberCount)
{
    if (chatId == m_selectedChat->id())
    {
        m_onlineCount = onlineMemberCount;

        emit selectedChatChanged();
    }
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
    auto list = messages.value("messages").toList();

    std::vector<std::unique_ptr<Message>> result;

    result.reserve(list.size());

    for (const auto &value : list)
    {
        auto message = std::make_unique<Message>();
        message->setFromVariantMap(value.toMap());
        result.push_back(std::move(message));
    }

    if (result.empty())
    {
        finalizeLoading();
    }
    else
    {
        std::ranges::sort(result, std::ranges::less{}, &Message::id);
        insertMessages(std::move(result));
    }

    emit countChanged();
}

void MessageModel::insertMessages(std::vector<std::unique_ptr<Message>> messages) noexcept
{
    processMessages(messages);

    if (m_loadingHistory)
    {
        m_loadingHistory = false;
        auto count = static_cast<int>(messages.size());

        beginInsertRows(QModelIndex(), 0, count - 1);

        // Insert messages
        m_messages.insert(m_messages.begin(), std::make_move_iterator(messages.begin()), std::make_move_iterator(messages.end()));

        endInsertRows();
        emit moreHistoriesLoaded(count);
    }
    else if (m_loading)
    {
        handleNewMessages(std::move(messages));
    }

    emit loadingChanged();
}

void MessageModel::finalizeLoading() noexcept
{
    if (m_loadingHistory)
        m_loadingHistory = false;
    else
        m_loading = false;

    emit loadingChanged();
}

void MessageModel::processMessages(const std::vector<std::unique_ptr<Message>> &messages) noexcept
{
    for (const auto &message : messages)
    {
        m_messageIds.emplace(message->id());
    }
}

void MessageModel::handleNewMessages(std::vector<std::unique_ptr<Message>> messages) noexcept
{
    m_loading = false;

    auto rowCountBefore = rowCount();
    auto count = static_cast<int>(messages.size());

    beginInsertRows(QModelIndex(), rowCountBefore, rowCountBefore + count - 1);
    m_messages.insert(m_messages.end(), std::make_move_iterator(messages.begin()), std::make_move_iterator(messages.end()));
    endInsertRows();

    if (m_selectedChat && m_selectedChat->unreadCount() > 0)
    {
        QVariantList messageIds;
        for (const auto &message : messages)
        {
            auto id = message->id();
            if (!message->isOutgoing() && id > m_selectedChat->lastReadInboxMessageId())
            {
                messageIds.append(id);
            }
        }

        if (!messageIds.isEmpty())
            viewMessages(messageIds);
    }
}

void MessageModel::loadMessages() noexcept
{
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
