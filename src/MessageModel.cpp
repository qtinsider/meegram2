#include "MessageModel.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "StorageManager.hpp"
#include "Localization.hpp"
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
    const auto count = basicGroup->member_count_;
    const auto statusId = basicGroup->status_->get_id();

    if (statusId == td::td_api::chatMemberStatusBanned::ID)
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
        return {};
    }

    auto count = supergroup->member_count_;
    if (count == 0)
    {
        count = store->getSupergroupFullInfo(supergroup->id_)->member_count_;
    }

    if (count <= 0)
    {
        return supergroup->usernames_ ? locale->getString("ChannelPublic") : locale->getString("ChannelPrivate");
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
    const auto hasLocation = supergroup->has_location_;
    auto count = supergroup->member_count_;
    const auto statusId = supergroup->status_->get_id();

    if (statusId == td::td_api::chatMemberStatusBanned::ID)
    {
        return locale->getString("YouWereKicked");
    }

    if (count == 0)
    {
        count = store->getSupergroupFullInfo(supergroup->id_)->member_count_;
    }

    if (count <= 0)
    {
        if (hasLocation)
        {
            return locale->getString("MegaLocation");
        }
        return supergroup->usernames_ ? locale->getString("MegaPublic") : locale->getString("MegaPrivate");
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
    if (std::ranges::contains(ServiceNotificationsUserIds, user->id_))
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
        case td::td_api::userStatusEmpty::ID:
            return locale->getString("ALongTimeAgo");
        case td::td_api::userStatusLastMonth::ID:
            return locale->getString("WithinAMonth");
        case td::td_api::userStatusLastWeek::ID:
            return locale->getString("WithinAWeek");
        case td::td_api::userStatusOffline::ID: {
            const auto was_online = static_cast<const td::td_api::userStatusOffline &>(*user->status_).was_online_;
            if (was_online == 0)
            {
                return locale->getString("Invisible");
            }

            const auto wasOnline = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(was_online) * 1000);
            const auto currentDate = QDate::currentDate();

            if (currentDate == wasOnline.date())
            {
                return locale->getString("LastSeenFormatted")
                    .arg(locale->getString("TodayAtFormatted"))
                    .arg(wasOnline.toString(locale->getString("formatterDay12H")));
            }
            else if (wasOnline.date().daysTo(currentDate) < 2)
            {
                return locale->getString("LastSeenFormatted")
                    .arg(locale->getString("YesterdayAtFormatted"))
                    .arg(wasOnline.toString(locale->getString("formatterDay12H")));
            }

            return locale->getString("LastSeenDateFormatted")
                .arg(locale->getString("formatDateAtTime")
                         .arg(wasOnline.toString(locale->getString("formatterYear")))
                         .arg(wasOnline.toString(locale->getString("formatterDay12H"))));
        }
        case td::td_api::userStatusOnline::ID:
            return locale->getString("Online");
        case td::td_api::userStatusRecently::ID:
            return locale->getString("Lately");
        default:
            return {};
    }
}

}  // namespace

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_storageManager(&StorageManager::instance())
    , m_client(StorageManager::instance().client())
    , m_locale(&Locale::instance())

{
    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    setRoleNames(roleNames());
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

    if (!m_selectedChat)
        return {};

    if (!m_messages.empty())
    {
        if (auto max = std::ranges::max(m_messageIds); max.has_value())
        {
            return !m_loading && m_selectedChat->last_message_->id_ != *max;
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
            return QString::number(message->id_);
        case SenderRole: {
            if (message->is_outgoing_)
                return QString();

            return Utils::getTitle(*message, m_storageManager, m_locale);
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

            return date.toString(m_locale->getString("formatterDay12H"));
        }
        case EditDateRole: {
            auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(message->edit_date_) * 1000);

            return date.toString(m_locale->getString("formatterDay12H"));
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
        case IsServiceMessageRole: {
            return Utils::isServiceMessage(*message);
        }
        case SectionRole: {
            const auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(message->date_) * 1000);

            const auto days = date.daysTo(QDateTime::currentDateTime());

            if (days == 0)
                return m_locale->getString("Today");
            else if (days < 2)
                return m_locale->getString("Yesterday");

            return date.toString(m_locale->getString("chatFullDate"));
        }
        case ServiceMessageRole: {
            return Utils::getServiceMessageContent(*message, m_storageManager, m_locale, true);
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

QString MessageModel::getChatId() const noexcept
{
    if (!m_selectedChat)
        return {};

    return QString::number(m_selectedChat->id_);
}

void MessageModel::setChatId(const QString &value) noexcept
{
    auto chat = m_storageManager->getChat(value.toLongLong());

    if (chat && (!m_selectedChat || m_selectedChat->id_ != chat->id_))
    {
        m_selectedChat = chat;

        emit selectedChatChanged();
    }
}

QString MessageModel::getChatSubtitle() const noexcept
{
    if (!m_selectedChat)
        return {};

    const auto chatTypeId = m_selectedChat->type_->get_id();

    switch (chatTypeId)
    {
        case td::td_api::chatTypePrivate::ID: {
            const auto userId = static_cast<const td::td_api::chatTypePrivate *>(m_selectedChat->type_.get())->user_id_;
            return getUserStatus(m_storageManager->getUser(userId), m_locale);
        }

        case td::td_api::chatTypeSecret::ID: {
            const auto secretChatId = static_cast<const td::td_api::chatTypeSecret *>(m_selectedChat->type_.get())->secret_chat_id_;
            return getUserStatus(m_storageManager->getUser(secretChatId), m_locale);
        }

        case td::td_api::chatTypeBasicGroup::ID: {
            const auto basicGroupId = static_cast<const td::td_api::chatTypeBasicGroup *>(m_selectedChat->type_.get())->basic_group_id_;
            return getBasicGroupStatus(m_storageManager->getBasicGroup(basicGroupId), m_onlineCount, m_locale);
        }

        case td::td_api::chatTypeSupergroup::ID: {
            const auto *type = static_cast<const td::td_api::chatTypeSupergroup *>(m_selectedChat->type_.get());
            const auto supergroup = m_storageManager->getSupergroup(type->supergroup_id_);
            return type->is_channel_ ? getChannelStatus(supergroup, m_onlineCount, m_storageManager, m_locale)
                                     : getSupergroupStatus(supergroup, m_onlineCount, m_storageManager, m_locale);
        }

        default:
            return {};
    }
}

QString MessageModel::getChatTitle() const noexcept
{
    if (!m_selectedChat)
        return {};

    const auto title = QString::fromStdString(m_selectedChat->title_);
    if (Utils::isMeChat(m_selectedChat, m_storageManager))
        return m_locale->getString("SavedMessages");

    return title.isEmpty() ? m_locale->getString("HiddenName") : title;
}

QString MessageModel::getChatPhoto() const noexcept
{
    if (!m_selectedChat)
        return "image://theme/icon-l-content-avatar-placeholder";

    const auto &chatPhoto = m_selectedChat->photo_;
    if (chatPhoto && chatPhoto->small_)
    {
        const auto &smallPhoto = chatPhoto->small_;
        if (smallPhoto->local_ && smallPhoto->local_->is_downloading_completed_)
        {
            return QString::fromStdString("image://chatPhoto/" + smallPhoto->local_->path_);
        }
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
    if (!m_selectedChat)
        return;

    m_client->send(td::td_api::make_object<td::td_api::openChat>(m_selectedChat->id_), {});
}

void MessageModel::closeChat() noexcept
{
    if (!m_selectedChat)
        return;

    m_client->send(td::td_api::make_object<td::td_api::closeChat>(m_selectedChat->id_), {});
}

void MessageModel::getChatHistory(qlonglong fromMessageId, int offset, int limit)
{
    if (!m_selectedChat)
        return;

    auto request = td::td_api::make_object<td::td_api::getChatHistory>();

    request->chat_id_ = m_selectedChat->id_;
    request->from_message_id_ = fromMessageId;
    request->offset_ = offset;
    request->limit_ = limit;
    request->only_local_ = false;

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::messages::ID)
            handleMessages(td::move_tl_object_as<td::td_api::messages>(response));
    });
}

void MessageModel::sendMessage(const QString &message, qlonglong replyToMessageId)
{
    if (!m_selectedChat)
        return;

    auto request = td::td_api::make_object<td::td_api::sendMessage>();

    auto inputMessageContent = td::td_api::make_object<td::td_api::inputMessageText>();

    inputMessageContent->text_ = td::td_api::make_object<td::td_api::formattedText>();
    inputMessageContent->text_->text_ = message.toStdString();

    request->chat_id_ = m_selectedChat->id_;

    if (replyToMessageId != 0)
    {
        request->reply_to_ = td::td_api::make_object<td::td_api::inputMessageReplyToMessage>(m_selectedChat->id_, replyToMessageId, nullptr);
    }

    request->input_message_content_ = std::move(inputMessageContent);

    m_client->send(std::move(request), {});
}

void MessageModel::viewMessages(std::vector<int64_t> &&messageIds)
{
    if (!m_selectedChat)
        return;

    auto request = td::td_api::make_object<td::td_api::viewMessages>();

    request->chat_id_ = m_selectedChat->id_;
    request->message_ids_ = std::move(messageIds);
    request->force_read_ = true;

    m_client->send(std::move(request), {});
}

void MessageModel::deleteMessage(qlonglong messageId, bool revoke) noexcept
{
    if (!m_selectedChat)
        return;

    auto request = td::td_api::make_object<td::td_api::deleteMessages>();

    request->chat_id_ = m_selectedChat->id_;
    request->message_ids_ = std::move(std::vector<int64_t>(messageId));
    request->revoke_ = revoke;

    m_client->send(std::move(request), {});
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

void MessageModel::classBegin()
{
}

void MessageModel::componentComplete()
{
    openChat();
    loadMessages();
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
            [this](td::td_api::updateChatReadInbox &value) { handleChatReadInbox(value.chat_id_, value.last_read_inbox_message_id_, value.unread_count_); },
            [this](td::td_api::updateChatReadOutbox &value) { handleChatReadOutbox(value.chat_id_, value.last_read_outbox_message_id_); }, [](auto &) {}});
}

void MessageModel::handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message)
{
    if (!m_selectedChat || m_selectedChat->id_ != message->chat_id_)
        return;

    if (auto &lastMessage = m_selectedChat->last_message_; !m_messageIds.contains(lastMessage->id_))
    {
        auto id = message->id_;

        beginInsertRows(QModelIndex(), rowCount(), rowCount());
        m_messageIds.insert(id);
        m_messages.emplace_back(std::move(message));
        endInsertRows();

        viewMessages({id});
        emit countChanged();
    }
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
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    if (auto it = std::ranges::find(m_messages, messageId, &td::td_api::message::id_); it != m_messages.end())
    {
        (*it)->content_ = std::move(newContent);

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup)
{
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    if (auto it = std::ranges::find(m_messages, messageId, &td::td_api::message::id_); it != m_messages.end())
    {
        (*it)->edit_date_ = editDate;
        (*it)->reply_markup_ = std::move(replyMarkup);

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned)
{
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    if (auto it = std::ranges::find(m_messages, messageId, &td::td_api::message::id_); it != m_messages.end())
    {
        (*it)->is_pinned_ = isPinned;

        itemChanged(std::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo)
{
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    if (auto it = std::ranges::find(m_messages, messageId, &td::td_api::message::id_); it != m_messages.end())
    {
        (*it)->interaction_info_ = std::move(interactionInfo);

        itemChanged(std::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool /*isPermanent*/, bool /*fromCache*/)
{
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    for (const auto id : messageIds)
    {
        if (auto it = std::ranges::find(m_messages, id, &td::td_api::message::id_); it != m_messages.end())
        {
            auto index = std::ranges::distance(m_messages.begin(), it);

            beginRemoveRows(QModelIndex(), index, index);
            m_messages.erase(it);
            endRemoveRows();
        }
    }
}

void MessageModel::handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount)
{
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    m_onlineCount = onlineMemberCount;

    emit selectedChatChanged();
}

void MessageModel::handleChatReadInbox(qlonglong chatId, qlonglong lastReadInboxMessageId, int unreadCount)
{
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    // m_selectedChat->last_read_inbox_message_id_ = lastReadInboxMessageId;
    // m_selectedChat->unread_count_ = unreadCount;

    emit selectedChatChanged();
}

void MessageModel::handleChatReadOutbox(qlonglong chatId, qlonglong lastReadOutboxMessageId)
{
    if (!m_selectedChat || chatId != m_selectedChat->id_)
        return;

    // m_selectedChat->last_read_outbox_message_id_ = lastReadOutboxMessageId;

    emit selectedChatChanged();
}

void MessageModel::handleMessages(td::td_api::object_ptr<td::td_api::messages> &&messages)
{
    if (!messages)
    {
        finalizeLoading();
    }
    else
    {
        std::ranges::sort(messages->messages_, std::ranges::less{}, &td::td_api::message::id_);
        insertMessages(std::move(messages->messages_));
    }

    emit countChanged();
}

void MessageModel::insertMessages(std::vector<td::td_api::object_ptr<td::td_api::message>> &&messages) noexcept
{
    processMessages(messages);

    if (m_loadingHistory)
    {
        m_loadingHistory = false;
        auto count = static_cast<int>(messages.size());

        if (count > 0)
        {
            beginInsertRows(QModelIndex(), 0, count - 1);

            // Insert messages
            m_messages.insert(m_messages.begin(), std::make_move_iterator(messages.begin()), std::make_move_iterator(messages.end()));

            endInsertRows();
            emit moreHistoriesLoaded(count);
        }
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

void MessageModel::processMessages(const std::vector<td::td_api::object_ptr<td::td_api::message>> &messages) noexcept
{
    for (const auto &message : messages)
    {
        m_messageIds.emplace(message->id_);
    }
}

void MessageModel::handleNewMessages(std::vector<td::td_api::object_ptr<td::td_api::message>> &&messages) noexcept
{
    // Check if the selected chat is valid
    if (!m_selectedChat)
        return;

    m_loading = false;

    // Handle unread messages first
    if (m_selectedChat->unread_count_ > 0 && !messages.empty())
    {
        std::vector<int64_t> messageIds;
        messageIds.reserve(messages.size());  // Reserve space to avoid reallocations

        for (const auto &message : messages)
        {
            if (message)  // Ensure the message is valid
            {
                auto id = message->id_;
                if (!message->is_outgoing_ && id > m_selectedChat->last_read_inbox_message_id_)
                {
                    messageIds.emplace_back(id);
                }
            }
        }

        if (!messageIds.empty())
        {
            viewMessages(std::move(messageIds));  // Move the message IDs to viewMessages
        }
    }

    // Insert messages into the model
    auto rowCountBefore = rowCount();
    auto count = static_cast<int>(messages.size());

    if (count > 0)
    {
        beginInsertRows(QModelIndex(), rowCountBefore, rowCountBefore + count - 1);

        // Move messages into m_messages
        m_messages.insert(m_messages.end(), std::make_move_iterator(messages.begin()), std::make_move_iterator(messages.end()));

        endInsertRows();
    }

    emit loadingChanged();  // Emit signal to notify that loading state has changed
}

void MessageModel::loadMessages() noexcept
{
    if (!m_selectedChat)
        return;

    const auto unread = m_selectedChat->unread_count_ > 0;

    const auto fromMessageId = unread ? m_selectedChat->last_read_inbox_message_id_ : m_selectedChat->last_message_->id_;

    const auto offset = unread ? -1 - MessageSliceLimit : 0;
    const auto limit = unread ? 2 * MessageSliceLimit : MessageSliceLimit;

    getChatHistory(fromMessageId, offset, limit);
}

void MessageModel::itemChanged(int64_t index)
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}
