#include "Chat.hpp"

#include "Localization.hpp"
#include "Message.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

Chat::Chat(QObject *parent)
    : QObject(parent)
    , m_storageManager(&StorageManager::instance())
{
    connect(m_storageManager, SIGNAL(dataChanged(td::td_api::Object *)), this, SLOT(onDataChanged(td::td_api::Object *)));
}

Chat::Chat(qint64 chatId, ChatList chatList, QObject *parent)
    : QObject(parent)
    , m_chatId(chatId)
    , m_chatList(std::move(chatList))
    , m_storageManager(&StorageManager::instance())
{
    connect(m_storageManager, SIGNAL(dataChanged(td::td_api::Object *)), this, SLOT(onDataChanged(td::td_api::Object *)));

    m_chat = m_storageManager->getChat(m_chatId);

    if (auto &photo = m_chat->photo_; photo && photo->small_)
    {
        m_file = std::make_shared<File>(photo->small_.get());

        connect(m_file.get(), SIGNAL(fileChanged()), this, SLOT(onFileChanged()));

        if (m_file->canBeDownloaded() && !m_file->isDownloadingCompleted())
        {
            m_file->downloadFile();
        }
    }

    if (m_chat->last_message_)
    {
        m_lastMessage = std::make_shared<Message>(m_chat->last_message_.get());
    }

    m_chatPosition = calculateChatPosition();
}

qint64 Chat::id() const
{
    return m_chat->id_;
}

QString Chat::type() const
{
    switch (m_chat->type_->get_id())
    {
        case td::td_api::chatTypePrivate::ID:
            return "Private";

        case td::td_api::chatTypeSecret::ID:
            return "Secret";

        case td::td_api::chatTypeBasicGroup::ID:
            return "Group";

        case td::td_api::chatTypeSupergroup::ID: {
            auto chatType = static_cast<const td::td_api::chatTypeSupergroup *>(m_chat->type_.get());
            return chatType->is_channel_ ? "Channel" : "Supergroup";
        }

        default:
            return {};
    }
}

QString Chat::title() const
{
    return QString::fromStdString(m_chat->title_);
}

QString Chat::photo() const
{
    return m_file ? m_file->localPath() : QString();
}

Message *Chat::lastMessage() const
{
    return m_lastMessage.get();
}

bool Chat::isMarkedAsUnread() const
{
    return m_chat->is_marked_as_unread_;
}

bool Chat::hasScheduledMessages() const
{
    return m_chat->has_scheduled_messages_;
}

bool Chat::canBeDeletedOnlyForSelf() const
{
    return m_chat->can_be_deleted_only_for_self_;
}

bool Chat::canBeDeletedForAllUsers() const
{
    return m_chat->can_be_deleted_for_all_users_;
}

int Chat::unreadCount() const
{
    return m_chat->unread_count_;
}

qint64 Chat::lastReadInboxMessageId() const
{
    return m_chat->last_read_inbox_message_id_;
}

qint64 Chat::lastReadOutboxMessageId() const
{
    return m_chat->last_read_outbox_message_id_;
}

int Chat::unreadMentionCount() const
{
    return m_chat->unread_mention_count_;
}

int Chat::unreadReactionCount() const
{
    return m_chat->unread_reaction_count_;
}

int Chat::messageAutoDeleteTime() const
{
    return m_chat->message_auto_delete_time_;
}

qint64 Chat::replyMarkupMessageId() const
{
    return m_chat->reply_markup_message_id_;
}

Message *Chat::draftMessage() const
{
    return {};
}

qint64 Chat::getOrder() const noexcept
{
    return m_chatPosition ? m_chatPosition->order_ : 0;
}

bool Chat::isPinned() const noexcept
{
    return m_chatPosition ? m_chatPosition->is_pinned_ : false;
}

bool Chat::isMe() const noexcept
{
    if (!m_chat || !m_chat->type_)
    {
        return false;
    }

    const auto myId = m_storageManager->myId();

    switch (m_chat->type_->get_id())
    {
        case td::td_api::chatTypeSecret::ID: {
            const auto userId = static_cast<const td::td_api::chatTypeSecret *>(m_chat->type_.get())->user_id_;
            return myId == userId;
        }
        case td::td_api::chatTypePrivate::ID: {
            const auto userId = static_cast<const td::td_api::chatTypePrivate *>(m_chat->type_.get())->user_id_;
            return myId == userId;
        }
        default:
            return false;
    }
}

QString Chat::getTitle() const noexcept
{
    if (isMe() && m_showSavedMessages)
    {
        return Locale::instance().getString("SavedMessages");
    }

    const auto title = QString::fromStdString(m_chat->title_).trimmed();
    return !title.isEmpty() ? title : Locale::instance().getString("HiddenName");
}

bool Chat::isMuted() const noexcept
{
    return muteFor() > 0;
}

int Chat::muteFor() const noexcept
{
    return m_chat && m_chat->notification_settings_ ? m_chat->notification_settings_->mute_for_ : 0;
}

void Chat::toggleIsPinned()
{
    // auto request = td::td_api::make_object<td::td_api::toggleChatIsPinned>();
    // request->chat_list_ = Utils::toChatList(m_chatList);
    // request->chat_id_ = data(modelIndex, IdRole).toLongLong();
    // request->is_pinned_ = !data(modelIndex, IsPinnedRole).toBool();

    // m_client->send(std::move(request), [this](auto &&response) {
    //     if (response->get_id() == td::td_api::ok::ID)
    //         QMetaObject::invokeMethod(this, "populate", Qt::QueuedConnection);
    // });
}

void Chat::toggleNotificationSettings()
{
    // const auto chatId = data(modelIndex, IdRole).toLongLong();
    // const auto isMuted = !data(modelIndex, IsMutedRole).toBool();

    // if (const auto isMutedPrev = Utils::isChatMuted(chatId, m_storageManager); isMutedPrev == isMuted)
    //     return;

    // const auto muteFor = isMuted ? MutedValueMax : MutedValueMin;

    // auto newNotificationSettings = td::td_api::make_object<td::td_api::chatNotificationSettings>();
    // newNotificationSettings->use_default_mute_for_ = false;
    // newNotificationSettings->mute_for_ = muteFor;

    // auto request = td::td_api::make_object<td::td_api::setChatNotificationSettings>();
    // request->chat_id_ = chatId;
    // request->notification_settings_ = std::move(newNotificationSettings);

    // m_client->send(std::move(request), [this](auto &&value) {
    //     if (value->get_id() == td::td_api::ok::ID)
    //         QMetaObject::invokeMethod(this, "populate", Qt::QueuedConnection);
    // });
}

void Chat::onDataChanged(td::td_api::Object *object)
{
    auto handleChatUpdate = [&](auto *value, bool emitSignal = false) {
        if (value->chat_id_ != m_chat->id_)
            return;

        m_chat = m_storageManager->getChat(value->chat_id_);

        if (emitSignal)
        {
            if (m_chat->last_message_)
            {
                m_lastMessage->setMessage(m_chat->last_message_.get());
            }

            m_chatPosition = calculateChatPosition();

            emit chatPositionUpdated(value->chat_id_);
        }

        emit chatItemUpdated(value->chat_id_);
    };

    switch (object->get_id())
    {
        case td::td_api::updateChatTitle::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatTitle *>(object));
            break;
        case td::td_api::updateChatPhoto::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatPhoto *>(object));
            break;
        case td::td_api::updateChatPermissions::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatPermissions *>(object));
            break;
        case td::td_api::updateChatLastMessage::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatLastMessage *>(object), true);
            break;
        case td::td_api::updateChatPosition::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatPosition *>(object), true);
            break;
        case td::td_api::updateChatReadInbox::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatReadInbox *>(object));
            break;
        case td::td_api::updateChatReadOutbox::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatReadOutbox *>(object));
            break;
        case td::td_api::updateChatActionBar::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatActionBar *>(object));
            break;
        case td::td_api::updateChatDraftMessage::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatDraftMessage *>(object), true);
            break;
        case td::td_api::updateChatNotificationSettings::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatNotificationSettings *>(object));
            break;
        case td::td_api::updateChatReplyMarkup::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatReplyMarkup *>(object));
            break;
        case td::td_api::updateChatUnreadMentionCount::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatUnreadMentionCount *>(object));
            break;
        case td::td_api::updateChatIsMarkedAsUnread::ID:
            handleChatUpdate(static_cast<td::td_api::updateChatIsMarkedAsUnread *>(object));
            break;
        default:
            break;
    }
}

void Chat::onFileChanged()
{
    if (m_file && m_file->isDownloadingCompleted())
    {
        emit chatItemUpdated(m_chat->id_);
    }
}

td::td_api::object_ptr<td::td_api::chatPosition> Chat::calculateChatPosition() noexcept
{
    if (!m_chat || m_chat->positions_.empty())
    {
        return nullptr;
    }

    const auto &positions = m_chat->positions_;

    const auto findPosition = [&](int id,
                                  std::function<bool(const td::td_api::ChatList &)> &&folder = nullptr) -> td::td_api::object_ptr<td::td_api::chatPosition> {
        for (const auto &position : positions)
        {
            if (const auto &list = position->list_; list->get_id() == id && (!folder || folder(*list)))
            {
                return td::td_api::make_object<td::td_api::chatPosition>(Utils::toChatList(m_chatList), position->order_, position->is_pinned_, nullptr);
            }
        }
        return nullptr;
    };

    switch (m_chatList.type)
    {
        case TdApi::ChatListMain:
            return findPosition(td::td_api::chatListMain::ID);
        case TdApi::ChatListArchive:
            return findPosition(td::td_api::chatListArchive::ID);
        case TdApi::ChatListFolder: {
            auto folder = [this](const td::td_api::ChatList &list) {
                return static_cast<const td::td_api::chatListFolder &>(list).chat_folder_id_ == m_chatList.folderId;
            };
            return findPosition(td::td_api::chatListFolder::ID, folder);
        }
        default:
            return nullptr;
    }
}
