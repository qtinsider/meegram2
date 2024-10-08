#include "MessageModel.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QStringList>
#include <QTimer>

#include <algorithm>
#include <ranges>
#include <unordered_set>

namespace {

QString getBasicGroupStatus(const td::td_api::basicGroup *basicGroup, int onlineCount) noexcept
{
    const auto count = basicGroup->member_count_;
    const auto statusId = basicGroup->status_->get_id();

    if (statusId == td::td_api::chatMemberStatusBanned::ID)
    {
        return QObject::tr("YouWereKicked");
    }

    const auto memberString = Locale::instance().formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    if (onlineCount > 1)
    {
        return memberString + ", " + Locale::instance().formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getChannelStatus(const td::td_api::supergroup *supergroup, int onlineCount, StorageManager *store) noexcept
{
    if (!supergroup->is_channel_)
    {
        return {};
    }

    auto count = supergroup->member_count_;
    if (count == 0)
    {
        count = store->getSupergroupFullInfo(supergroup->id_)->member_count_;
    }

    if (count <= 0)
    {
        return supergroup->usernames_ ? QObject::tr("ChannelPublic") : QObject::tr("ChannelPrivate");
    }

    const auto subscriberString = Locale::instance().formatPluralString("Subscribers", count);
    if (count <= 1)
    {
        return subscriberString;
    }

    if (onlineCount > 1)
    {
        return subscriberString + ", " + Locale::instance().formatPluralString("OnlineCount", onlineCount);
    }

    return subscriberString;
}

QString getSupergroupStatus(const td::td_api::supergroup *supergroup, int onlineCount, StorageManager *store) noexcept
{
    const auto hasLocation = supergroup->has_location_;
    auto count = supergroup->member_count_;
    const auto statusId = supergroup->status_->get_id();

    if (statusId == td::td_api::chatMemberStatusBanned::ID)
    {
        return QObject::tr("YouWereKicked");
    }

    if (count == 0)
    {
        count = store->getSupergroupFullInfo(supergroup->id_)->member_count_;
    }

    if (count <= 0)
    {
        if (hasLocation)
        {
            return QObject::tr("MegaLocation");
        }
        return supergroup->usernames_ ? QObject::tr("MegaPublic") : QObject::tr("MegaPrivate");
    }

    const auto memberString = Locale::instance().formatPluralString("Members", count);
    if (count <= 1)
    {
        return memberString;
    }

    if (onlineCount > 1)
    {
        return memberString + ", " + Locale::instance().formatPluralString("OnlineCount", onlineCount);
    }

    return memberString;
}

QString getUserStatus(const td::td_api::user *user) noexcept
{
    if (std::ranges::contains(ServiceNotificationsUserIds, user->id_))
    {
        return QObject::tr("ServiceNotifications");
    }

    if (user->is_support_)
    {
        return QObject::tr("SupportStatus");
    }

    if (user->type_->get_id() == td::td_api::userTypeBot::ID)
    {
        return QObject::tr("Bot");
    }

    switch (user->status_->get_id())
    {
        case td::td_api::userStatusEmpty::ID:
            return QObject::tr("ALongTimeAgo");
        case td::td_api::userStatusLastMonth::ID:
            return QObject::tr("WithinAMonth");
        case td::td_api::userStatusLastWeek::ID:
            return QObject::tr("WithinAWeek");
        case td::td_api::userStatusOffline::ID: {
            const auto was_online = static_cast<const td::td_api::userStatusOffline &>(*user->status_).was_online_;
            if (was_online == 0)
            {
                return QObject::tr("Invisible");
            }

            const auto wasOnline = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(was_online) * 1000);
            const auto currentDate = QDate::currentDate();

            if (currentDate == wasOnline.date())
            {
                return QObject::tr("LastSeenFormatted").arg(QObject::tr("TodayAtFormatted")).arg(wasOnline.toString(QObject::tr("formatterDay12H")));
            }
            else if (wasOnline.date().daysTo(currentDate) < 2)
            {
                return QObject::tr("LastSeenFormatted").arg(QObject::tr("YesterdayAtFormatted")).arg(wasOnline.toString(QObject::tr("formatterDay12H")));
            }

            return QObject::tr("LastSeenDateFormatted")
                .arg(QObject::tr("formatDateAtTime")
                         .arg(wasOnline.toString(QObject::tr("formatterYear")))
                         .arg(wasOnline.toString(QObject::tr("formatterDay12H"))));
        }
        case td::td_api::userStatusOnline::ID:
            return QObject::tr("Online");
        case td::td_api::userStatusRecently::ID:
            return QObject::tr("Lately");
        default:
            return {};
    }
}

}  // namespace

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_client(StorageManager::instance().client())
    , m_storageManager(&StorageManager::instance())
{
    qDebug() << "MessageModel initialized.";

    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    setRoleNames(roleNames());
}

int MessageModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_messages.size();
}

QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= static_cast<int>(m_messages.size()))
        return QVariant();

    const auto it = std::ranges::next(m_messages.begin(), index.row());
    const auto &message = it->second;  // Access the message directly

    switch (role)
    {
        case IdRole:
            return QVariant::fromValue(message->id_);
        case SenderRole: {
            if (message->is_outgoing_)
                return QString();

            return Utils::getTitle(*message, m_storageManager);
        }
        // case ChatIdRole:
        //     return message->chatId();
        // case SendingStateRole:
        //     return message->sendingState();
        // case SchedulingStateRole:
        //     return message->schedulingState();
        case IsOutgoingRole:
            return message->is_outgoing_;
        // case IsPinnedRole:
        //     return message->isPinned();
        // case CanBeEditedRole:
        //     return message->canBeEdited();
        // case CanBeForwardedRole:
        //     return message->canBeForwarded();
        // case CanBeDeletedOnlyForSelfRole:
        //     return message->canBeDeletedOnlyForSelf();
        // case CanBeDeletedForAllUsersRole:
        //     return message->canBeDeletedForAllUsers();
        // case CanGetStatisticsRole:
        //     return message->canGetStatistics();
        // case CanGetMessageThreadRole:
        //     return message->canGetMessageThread();
        // case IsChannelPostRole:
        //     return message->isChannelPost();
        // case ContainsUnreadMentionRole:
        //     return message->containsUnreadMention();
        case DateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(message->date_) * 1000);

            return date.toString(QObject::tr("formatterDay12H"));
        }
        case EditDateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(message->edit_date_) * 1000);

            return date.toString(QObject::tr("formatterDay12H"));
        }
            // case ForwardInfoRole:
            //     return message->forwardInfo();
            // case InteractionInfoRole:
            //     return message->interactionInfo();
            // case MessageThreadIdRole:
            //     return message->messageThreadId();
            // case ViaBotUserIdRole:
            //     return message->viaBotUserId();
            // case AuthorSignatureRole:
            //     return message->authorSignature();
            // case MediaAlbumIdRole:
            //     return message->mediaAlbumId();
            // case RestrictionReasonRole:
            //     return message->restrictionReason();
        case ContentRole: {
            const auto *content = message->content_.get();

            if (content->get_id() == td::td_api::messageText::ID)
            {
                // Cast to const td::td_api::messageText*
                const auto *formattedText = static_cast<const td::td_api::messageText *>(content);
                // Access text_ safely
                const auto *text = formattedText->text_.get();
                // Cast const pointer to void* safely
                return QVariant::fromValue(const_cast<void *>(static_cast<const void *>(text)));
            }

            return QVariant();
        }

            // case ReplyMarkupRole:
            //     return message->replyMarkup();

        case BubbleColorRole:
            return QVariant();
        case ContentTypeRole: {
            if (const auto *content = message->content_.get(); content->get_id() == td::td_api::messageText::ID)
            {
                return "text";
            }

            return QVariant();
        }
        case IsServiceMessageRole: {
            return Utils::isServiceMessage(*message);
        }
        case SectionRole: {
            const auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(message->date_) * 1000);

            const auto days = date.daysTo(QDateTime::currentDateTime());

            if (days == 0)
                return QObject::tr("Today");
            else if (days < 2)
                return QObject::tr("Yesterday");

            return date.toString(QObject::tr("chatFullDate"));
        }
        case ServiceMessageRole: {
            return Utils::getServiceMessageContent(*message, m_storageManager, true);
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
    roles[ContentTypeRole] = "contentType";
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

bool MessageModel::isEndReached() const noexcept
{
    return m_isEndReached;
}

QString MessageModel::getChatSubtitle() const noexcept
{
    if (!m_chat)
        return {};

    const auto chatTypeId = m_chat->getTypeId();
    const auto &chatTypeString = m_chat->type();

    if (chatTypeString == "private" || chatTypeString == "secret")
    {
        return getUserStatus(m_storageManager->getUser(chatTypeId));
    }

    if (chatTypeString == "basicGroup")
    {
        return getBasicGroupStatus(m_storageManager->getBasicGroup(chatTypeId), m_onlineCount);
    }

    if (chatTypeString == "supergroup" || chatTypeString == "channel")
    {
        const auto &supergroup = m_storageManager->getSupergroup(chatTypeId);

        return (chatTypeString == "channel") ? getChannelStatus(supergroup, m_onlineCount, m_storageManager)
                                             : getSupergroupStatus(supergroup, m_onlineCount, m_storageManager);
    }

    return {};
}

Chat *MessageModel::chat() const noexcept
{
    return m_chat;
}

void MessageModel::setChat(Chat *value) noexcept
{
    if (m_chat == value)
        return;

    qDebug() << "Setting new chat. Previous chat ID:" << (m_chat ? m_chat->id() : -1) << ", new chat ID:" << (value ? value->id() : -1);
    if (m_chat)
    {
        disconnect(m_chat, SIGNAL(chatItemUpdated(qlonglong)), this, SIGNAL(chatChanged()));
        disconnect(m_chat, SIGNAL(chatItemUpdated(qlonglong)), this, SLOT(handleChatItem(qlonglong)));
    }

    m_chat = value;

    if (m_chat)
    {
        connect(m_chat, SIGNAL(chatItemUpdated(qlonglong)), this, SIGNAL(chatChanged()));
        connect(m_chat, SIGNAL(chatItemUpdated(qlonglong)), this, SLOT(handleChatItem(qlonglong)));
    }

    emit chatChanged();
}

void MessageModel::loadNextSlice() noexcept
{
    if (!m_loading)
    {
        qDebug() << "Loading next message slice. Max message ID:" << std::ranges::max(m_messages | std::views::keys);

        getChatHistory(std::ranges::max(m_messages | std::views::keys), -MessageSliceLimit, MessageSliceLimit);

        m_loading = true;

        emit loadingChanged();
    }
}

void MessageModel::loadPreviousSlice() noexcept
{
    if (!m_loading)
    {
        qDebug() << "Loading previous message slice. Min message ID:" << std::ranges::min(m_messages | std::views::keys);

        m_loading = true;

        getChatHistory(std::ranges::min(m_messages | std::views::keys), 0, MessageSliceLimit);

        emit loadingChanged();
    }
}

void MessageModel::openChat() noexcept
{
    if (!m_chat)
        return;

    m_client->send(td::td_api::make_object<td::td_api::openChat>(m_chat->id()), {});
}

void MessageModel::closeChat() noexcept
{
    if (!m_chat)
        return;

    m_client->send(td::td_api::make_object<td::td_api::closeChat>(m_chat->id()), {});
}

void MessageModel::getChatHistory(qlonglong fromMessageId, int offset, int limit, bool previous)
{
    if (!m_chat)
    {
        return;
    }

    auto request = td::td_api::make_object<td::td_api::getChatHistory>();
    request->chat_id_ = m_chat->id();
    request->from_message_id_ = fromMessageId;
    request->offset_ = offset;
    request->limit_ = limit;
    request->only_local_ = false;

    m_client->send(std::move(request), [this, previous](auto &&response) {
        if (response->get_id() == td::td_api::messages::ID)
        {
            handleMessages(td::move_tl_object_as<td::td_api::messages>(response), previous);
        }
    });
}

void MessageModel::sendMessage(const QString &message, qlonglong replyToMessageId)
{
    if (!m_chat)
        return;

    auto request = td::td_api::make_object<td::td_api::sendMessage>();

    auto inputMessageContent = td::td_api::make_object<td::td_api::inputMessageText>();

    inputMessageContent->text_ = td::td_api::make_object<td::td_api::formattedText>();
    inputMessageContent->text_->text_ = message.toStdString();

    request->chat_id_ = m_chat->id();

    if (replyToMessageId != 0)
    {
        request->reply_to_ = td::td_api::make_object<td::td_api::inputMessageReplyToMessage>(m_chat->id(), replyToMessageId, nullptr);
    }

    request->input_message_content_ = std::move(inputMessageContent);

    m_client->send(std::move(request), {});
}

void MessageModel::viewMessages(const QStringList &messageIds)
{
    if (!m_chat)
        return;

    std::vector<int64_t> result;
    result.reserve(messageIds.size());

    for (const auto &id : messageIds)
    {
        result.emplace_back(id.toLongLong());
    }

    auto request = td::td_api::make_object<td::td_api::viewMessages>();

    request->chat_id_ = m_chat->id();
    request->message_ids_ = std::move(result);
    request->force_read_ = true;

    m_client->send(std::move(request), {});
}

void MessageModel::deleteMessage(qlonglong messageId, bool revoke) noexcept
{
    if (!m_chat)
        return;

    auto request = td::td_api::make_object<td::td_api::deleteMessages>();

    request->chat_id_ = m_chat->id();
    request->message_ids_ = std::move(std::vector<int64_t>(messageId));
    request->revoke_ = revoke;

    m_client->send(std::move(request), {});
}

void MessageModel::refresh() noexcept
{
    if (m_messages.empty())
        return;

    m_loading = true;
    m_isEndReached = false;

    beginResetModel();
    m_messages.clear();
    endResetModel();

    emit countChanged();
}

void MessageModel::classBegin()
{
}

void MessageModel::componentComplete()
{
    openChat();
    loadMessages();
}

void MessageModel::handleChatItem(qlonglong chatId)
{
    qDebug() << "Is m_chat ID equal to chatId?:" << (m_chat->id() == chatId) << "(m_chat ID:" << m_chat->id() << ", chatId:" << chatId << ")";
    qDebug() << "Total number of items in the model:" << count();
    qDebug() << "Last read inbox message ID:" << m_chat->lastReadInboxMessageId();
    qDebug() << "Last read outbox message ID:" << m_chat->lastReadOutboxMessageId();
    qDebug() << "Total unread message count:" << m_chat->unreadCount();
}

void MessageModel::handleResult(td::td_api::Object *object)
{
    td::td_api::downcast_call(
        *object,
        detail::Overloaded{
            [this](td::td_api::updateNewMessage &value) { handleNewMessage(std::move(value.message_)); },
            [this](td::td_api::updateMessageSendSucceeded &value) { handleMessageSendSucceeded(std::move(value.message_), value.old_message_id_); },
            [this](td::td_api::updateMessageSendFailed &value) {
                handleMessageSendFailed(std::move(value.message_), value.old_message_id_, std::move(value.error_));
            },
            [this](td::td_api::updateMessageContent &value) { handleMessageContent(value.chat_id_, value.message_id_, std::move(value.new_content_)); },
            [this](td::td_api::updateMessageEdited &value) {
                handleMessageEdited(value.chat_id_, value.message_id_, value.edit_date_, std::move(value.reply_markup_));
            },
            [this](td::td_api::updateMessageIsPinned &value) { handleMessageIsPinned(value.chat_id_, value.message_id_, value.is_pinned_); },
            [this](td::td_api::updateMessageInteractionInfo &value) {
                handleMessageInteractionInfo(value.chat_id_, value.message_id_, std::move(value.interaction_info_));
            },
            [this](td::td_api::updateDeleteMessages &value) {
                handleDeleteMessages(value.chat_id_, std::move(value.message_ids_), value.is_permanent_, value.from_cache_);
            },
            [this](td::td_api::updateChatOnlineMemberCount &value) { handleChatOnlineMemberCount(value.chat_id_, value.online_member_count_); },
            [](auto &) {}});
}

void MessageModel::handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message)
{
    if (!m_chat || m_chat->id() != message->chat_id_)
        return;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_messages.emplace(message->id_, std::move(message));
    endInsertRows();

    emit countChanged();
}

void MessageModel::handleMessageSendSucceeded(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId)
{
}

void MessageModel::handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId,
                                           td::td_api::object_ptr<td::td_api::error> &&error)
{
}

void MessageModel::handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent)
{
    if (!m_chat || chatId != m_chat->id())
        return;

    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        it->second->content_ = std::move(newContent);

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup)
{
    if (!m_chat || chatId != m_chat->id())
        return;

    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        it->second->edit_date_ = editDate;
        it->second->reply_markup_ = std::move(replyMarkup);

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned)
{
    if (!m_chat || chatId != m_chat->id())
        return;

    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        it->second->is_pinned_ = isPinned;

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId,
                                                td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo)
{
    if (!m_chat || chatId != m_chat->id())
        return;

    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        it->second->interaction_info_ = std::move(interactionInfo);

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool /*isPermanent*/, bool /*fromCache*/)
{
    if (!m_chat || chatId != m_chat->id())
        return;

    std::unordered_set idsSet(messageIds.begin(), messageIds.end());

    std::vector<int> indicesToRemove;

    for (const auto &[id, message] : m_messages)
    {
        if (idsSet.contains(id))
        {
            auto it = m_messages.find(id);
            auto index = std::ranges::distance(m_messages.begin(), it);
            indicesToRemove.emplace_back(index);
        }
    }

    std::ranges::sort(indicesToRemove, std::less<>());

    if (!indicesToRemove.empty())
    {
        beginRemoveRows(QModelIndex(), indicesToRemove.front(), indicesToRemove.back());

        std::erase_if(m_messages, [&idsSet](const auto &pair) { return idsSet.contains(pair.first); });

        endRemoveRows();
    }
}

void MessageModel::handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount)
{
    if (!m_chat || chatId != m_chat->id())
        return;

    m_onlineCount = onlineMemberCount;

    emit chatChanged();
}

void MessageModel::handleMessages(td::td_api::object_ptr<td::td_api::messages> &&messagesPtr, bool previous)
{
    if (!messagesPtr || messagesPtr->messages_.empty())
    {
        qDebug() << "handleMessages called. No messages received.";
        m_loading = false;
        emit loadingChanged();
        return;  // Early return if messagesPtr is null or messages are empty
    }

    auto &messages = messagesPtr->messages_;
    const auto messageCount = static_cast<int>(messages.size());

    qDebug() << "handleMessages called. Number of messages received:" << messageCount;

    // Calculate the insertion position
    const int insertPos = previous ? 0 : rowCount();
    beginInsertRows(QModelIndex(), insertPos, insertPos + messageCount - 1);
    qDebug() << "beginInsertRows called. Inserting at position:" << insertPos;

    std::ranges::for_each(messages, [this](auto &&message) { m_messages.emplace(message->id_, std::move(message)); });

    endInsertRows();

    if (previous)
    {
        emit moreHistoriesLoaded(messageCount);
        qDebug() << "moreHistoriesLoaded signal emitted with count:" << messageCount;
    }

    if (m_loading)
    {
        m_loading = false;
        qDebug() << "Loading is true, inserting messages at the end.";
    }

    emit loadingChanged();
    emit countChanged();
}

void MessageModel::loadMessages() noexcept
{
    if (!m_chat)
        return;

    const auto unread = m_chat->unreadCount() > 0;

    const auto fromMessageId = unread ? m_chat->lastReadInboxMessageId() : m_chat->lastMessage()->id();

    const auto offset = unread ? -1 - MessageSliceLimit : 0;
    const auto limit = unread ? 2 * MessageSliceLimit : MessageSliceLimit;

    getChatHistory(fromMessageId, offset, limit);
}

void MessageModel::itemChanged(int64_t index)
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}
