#include "MessageModel.hpp"

#include "BasicGroup.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"
#include "Supergroup.hpp"
#include "Utils.hpp"

#include <QDateTime>
#include <QDebug>
#include <QLocale>
#include <QTimer>

#include <algorithm>
#include <ranges>
#include <unordered_set>

#include <QDate>
#include <QObject>
#include <QString>
#include <memory>

MessageModel::MessageModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_client(StorageManager::instance().client())
    , m_storageManager(&StorageManager::instance())
{
    qDebug() << "MessageModel initialized.";

    connect(m_client.get(), SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    connect(m_storageManager, SIGNAL(userUpdated(qlonglong)), this, SLOT(handleUserUpdate(qlonglong)));
    connect(m_storageManager, SIGNAL(basicGroupUpdated(qlonglong)), this, SLOT(handleBasicGroupUpdate(qlonglong)));
    connect(m_storageManager, SIGNAL(supergroupUpdated(qlonglong)), this, SLOT(handleSupergroupUpdate(qlonglong)));

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
            return QVariant::fromValue(message->id());
        case SenderRole: {
            return message->getSenderName();
        }
        case ChatIdRole:
            return message->chatId();
        case IsOutgoingRole:
            return message->isOutgoing();
        case DateRole: {
            const auto date = message->date();
            return date.toString(QObject::tr("formatterDay12H"));
        }
        case EditDateRole: {
            const auto date = message->editDate();
            return date.toString(QObject::tr("formatterDay12H"));
        }
        case ContentRole: {
            // const auto *content = message->content_.get();

            // if (content->get_id() == td::td_api::messageText::ID)
            // {
            //     // Cast to const td::td_api::messageText*
            //     const auto *formattedText = static_cast<const td::td_api::messageText *>(content);
            //     // Access text_ safely
            //     const auto *text = formattedText->text_.get();
            //     // Cast const pointer to void* safely
            //     return QVariant::fromValue(const_cast<void *>(static_cast<const void *>(text)));
            // }

            return QVariant();
        }

        case ContentTypeRole: {
            // if (const auto *content = message->content_.get(); content->get_id() == td::td_api::messageText::ID)
            // {
            //     return "text";
            // }

            return QVariant();
        }
        case IsServiceMessageRole: {
            return message->isService();
        }
        case SectionRole: {
            const auto date = message->date();
            const auto days = date.daysTo(QDateTime::currentDateTime());

            if (days == 0)
                return QObject::tr("Today");
            else if (days < 2)
                return QObject::tr("Yesterday");

            return date.toString(QObject::tr("chatFullDate"));
        }
        case ServiceMessageRole: {
            return message->getServiceMessageContent();
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
    roles[IsOutgoingRole] = "isOutgoing";
    roles[IsPinnedRole] = "isPinned";
    roles[DateRole] = "date";
    roles[EditDateRole] = "editDate";
    roles[ContentRole] = "content";
    // Custom
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

ChatInfo *MessageModel::chatInfo() const noexcept
{
    return m_chatInfo.get();
}

Chat *MessageModel::chat() const noexcept
{
    return m_chat;
}

void MessageModel::setChat(Chat *value) noexcept
{
    if (m_chat == value)
        return;

    if (m_chat)
    {
        disconnect(m_chat, SIGNAL(chatChanged()), this, SIGNAL(chatChanged()));
        disconnect(m_chat, SIGNAL(chatChanged()), this, SLOT(handleChatItem()));
    }

    m_chat = value;

    if (m_chat)
    {
        connect(m_chat, SIGNAL(chatChanged()), this, SIGNAL(chatChanged()));
        connect(m_chat, SIGNAL(chatChanged()), this, SLOT(handleChatItem()));
    }

    m_chatInfo = std::make_unique<ChatInfo>(m_chat, this);

    connect(m_chatInfo.get(), SIGNAL(infoChanged()), this, SIGNAL(chatInfoChanged()));

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
        return;

    auto request = td::td_api::make_object<td::td_api::getChatHistory>();
    request->chat_id_ = m_chat->id();
    request->from_message_id_ = fromMessageId;
    request->offset_ = offset;
    request->limit_ = limit;
    request->only_local_ = false;

    m_client->send(std::move(request), [this, previous](auto &&response) {
        if (response->get_id() == td::td_api::messages::ID)
        {
            auto messages = td::td_api::move_object_as<td::td_api::messages>(response);

            if (!messages || messages->messages_.empty())
            {
                m_loading = false;
                emit loadingChanged();

                return;
            }

            std::vector<std::unique_ptr<Message>> newMessages;
            newMessages.reserve(messages->total_count_);  // Reserve space to avoid reallocations

            for (auto &&message : messages->messages_)
            {
                if (message)
                {
                    newMessages.emplace_back(std::make_unique<Message>(std::move(message)));
                }
            }

            handleMessages(std::move(newMessages), previous);
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

void MessageModel::viewMessages(const QList<qlonglong> &messageIds)
{
    if (!m_chat)
        return;

    std::vector<int64_t> result;
    result.reserve(messageIds.size());

    for (const auto &id : messageIds)
    {
        result.emplace_back(id);
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

void MessageModel::handleChatItem()
{
    qDebug() << "(m_chat ID:" << m_chat->id() << ")";
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
            [this](td::td_api::updateDeleteMessages &value) {
                handleDeleteMessages(value.chat_id_, std::move(value.message_ids_), value.is_permanent_, value.from_cache_);
            },
            [this](td::td_api::updateChatOnlineMemberCount &value) { handleChatOnlineMemberCount(value.chat_id_, value.online_member_count_); },
            [](auto &) {}});
}

void MessageModel::handleUserUpdate(qlonglong userId)
{
    if (!m_chat || m_chat->typeId() != userId)
        return;

    if (m_chat->type() == Chat::Private || m_chat->type() == Chat::Secret)
    {
        if (auto user = m_storageManager->getUser(userId))
        {
            m_chatInfo->setUser(user);
        }
    }
}

void MessageModel::handleBasicGroupUpdate(qlonglong groupId)
{
    if (!m_chat || m_chat->typeId() != groupId)
        return;

    if (m_chat->type() == Chat::BasicGroup)
    {
        if (auto group = m_storageManager->getBasicGroup(groupId))
        {
            m_chatInfo->setBasicGroup(group);
        }
    }
}

void MessageModel::handleSupergroupUpdate(qlonglong groupId)
{
    if (!m_chat || m_chat->typeId() != groupId)
        return;

    if (m_chat->type() == Chat::Supergroup)
    {
        if (auto group = m_storageManager->getSupergroup(groupId))
        {
            m_chatInfo->setSupergroup(group);
        }
    }
}

void MessageModel::handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message)
{
    if (!m_chat || m_chat->id() != message->chat_id_)
        return;

    const auto messageId = message->id_;

    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_messages.emplace(messageId, std::make_unique<Message>(std::move(message)));
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
        it->second->setContent(std::move(newContent));

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup)
{
    if (!m_chat || chatId != m_chat->id())
        return;

    if (auto it = m_messages.find(messageId); it != m_messages.end())
    {
        it->second->setEditDate(editDate);

        itemChanged(std::ranges::distance(m_messages.begin(), it));
    }
}

void MessageModel::handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool isPermanent, bool fromCache)
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

void MessageModel::handleMessages(std::vector<std::unique_ptr<Message>> &&messages, bool previous)
{
    const auto messageCount = static_cast<int>(messages.size());

    qDebug() << "handleMessages called. Number of messages received:" << messageCount;

    const int insertPos = previous ? 0 : rowCount();

    beginInsertRows(QModelIndex(), insertPos, insertPos + messageCount - 1);

    for (auto &message : messages)
    {
        m_messages.emplace(message->id(), std::move(message));
    }

    endInsertRows();

    if (previous)
    {
        emit moreHistoriesLoaded(messageCount);
    }

    if (m_loading)
    {
        m_loading = false;
        emit loadingChanged();
    }

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

void MessageModel::itemChanged(size_t index)
{
    QModelIndex modelIndex = createIndex(static_cast<int>(index), 0);

    emit dataChanged(modelIndex, modelIndex);
}

ChatInfo::ChatInfo(Chat *chat, QObject *parent)
    : QObject(parent)
    , m_chat(chat)
    , m_storageManager(&StorageManager::instance())
{
    initializeMembers();
    updateStatus();
}

QString ChatInfo::title() const noexcept
{
    const auto title = m_chat->title();
    return !title.isEmpty() ? title : tr("HiddenName");
}

QString ChatInfo::status() const noexcept
{
    return m_status;
}

void ChatInfo::setOnlineCount(int onlineCount)
{
    if (m_onlineCount != onlineCount)
    {
        m_onlineCount = onlineCount;
        updateStatus();
    }
}

void ChatInfo::setUser(std::shared_ptr<User> user)
{
    m_user = user;
    updateStatus();
}

void ChatInfo::setBasicGroup(std::shared_ptr<BasicGroup> group)
{
    m_basicGroup = group;
    updateStatus();
}

void ChatInfo::setSupergroup(std::shared_ptr<Supergroup> group)
{
    m_supergroup = group;
    updateStatus();
}

void ChatInfo::initializeMembers()
{
    const auto chatType = m_chat->type();
    const auto chatTypeId = m_chat->typeId();

    if (chatType == Chat::Private || chatType == Chat::Secret)
    {
        m_user = m_storageManager->getUser(chatTypeId);
    }

    if (chatType == Chat::BasicGroup)
    {
        m_basicGroup = m_storageManager->getBasicGroup(chatTypeId);
    }

    if (chatType == Chat::Supergroup || chatType == Chat::Channel)
    {
        m_supergroup = m_storageManager->getSupergroup(chatTypeId);
    }
}

void ChatInfo::updateStatus()
{
    QString newStatus;

    if (m_basicGroup)
    {
        if (m_basicGroup->status() == BasicGroup::Status::Banned)
            newStatus = QObject::tr("YouWereKicked");
        else
            newStatus = formatStatus(m_basicGroup->memberCount(), "Members", "OnlineCount");
    }
    else if (m_supergroup)
    {
        if (!m_supergroup->isChannel() && m_supergroup->status() == Supergroup::Status::Banned)
        {
            newStatus = QObject::tr("YouWereKicked");
        }
        else
        {
            int count = getMemberCountWithFallback();
            newStatus = (count <= 0)
                            ? (m_supergroup->hasLocation() ? QObject::tr("MegaLocation")
                                                           : (m_supergroup->usernames().isEmpty() ? QObject::tr("MegaPrivate") : QObject::tr("MegaPublic")))
                            : formatStatus(count, "Members", "OnlineCount");
        }
    }
    else if (m_user)
    {
        if (isServiceNotification())
        {
            newStatus = QObject::tr("ServiceNotifications");
        }
        else if (m_user->isSupport())
        {
            newStatus = QObject::tr("SupportStatus");
        }
        else if (m_user->type() == User::Type::Bot)
        {
            newStatus = QObject::tr("Bot");
        }
        else
        {
            newStatus = formatUserStatus();
        }
    }

    if (m_status != newStatus)
    {
        m_status = newStatus;
        emit infoChanged();
    }
}

int ChatInfo::getMemberCountWithFallback() const
{
    int count = m_supergroup->memberCount();
    if (count == 0)
    {
        if (const auto fullInfo = m_storageManager->getSupergroupFullInfo(m_supergroup->id()))
        {
            count = fullInfo->memberCount();
        }
    }

    return count;
}

QString ChatInfo::formatStatus(int memberCount, const char *memberKey, const char *onlineKey) const
{
    const auto memberString = Locale::instance().formatPluralString(memberKey, memberCount);
    if (memberCount <= 1)
        return memberString;

    if (m_onlineCount > 1)
    {
        return memberString + ", " + Locale::instance().formatPluralString(onlineKey, m_onlineCount);
    }

    return memberString;
}

bool ChatInfo::isServiceNotification() const
{
    return std::ranges::contains(ServiceNotificationsUserIds, m_user->id());
}

QString ChatInfo::formatUserStatus() const
{
    switch (m_user->status())
    {
        case User::Status::Empty:
            return QObject::tr("ALongTimeAgo");
        case User::Status::LastMonth:
            return QObject::tr("WithinAMonth");
        case User::Status::LastWeek:
            return QObject::tr("WithinAWeek");
        case User::Status::Offline:
            return formatOfflineStatus();
        case User::Status::Online:
            return QObject::tr("Online");
        case User::Status::Recently:
            return QObject::tr("Lately");
        default:
            return {};
    }
}

QString ChatInfo::formatOfflineStatus() const
{
    const auto wasOnline = m_user->wasOnline();
    if (wasOnline.isNull())
        return QObject::tr("Invisible");

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
        .arg(QObject::tr("formatDateAtTime").arg(wasOnline.toString(QObject::tr("formatterYear"))).arg(wasOnline.toString(QObject::tr("formatterDay12H"))));
}
