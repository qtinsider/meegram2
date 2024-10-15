#include "Chat.hpp"

#include "Localization.hpp"
#include "Message.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

Chat::Chat(QObject *parent)
    : QObject(parent)
    , m_storageManager(&StorageManager::instance())
{
    connect(m_storageManager, SIGNAL(chatsUpdated(td::td_api::Object *)), this, SLOT(onItemChanged(td::td_api::Object *)));
}

Chat::Chat(qlonglong chatId, ChatList chatList, QObject *parent)
    : QObject(parent)
    , m_chatId(chatId)
    , m_chatList(std::move(chatList))
    , m_storageManager(&StorageManager::instance())
{
    connect(m_storageManager, SIGNAL(chatsUpdated(td::td_api::Object *)), this, SLOT(onItemChanged(td::td_api::Object *)));

    m_chat = m_storageManager->getChat(m_chatId);

    if (auto &photo = m_chat->photo_; photo && photo->small_)
    {
        m_file = std::make_unique<File>(photo->small_.get());

        connect(m_file.get(), SIGNAL(fileChanged()), this, SLOT(onFileChanged()));

        if (m_file->canBeDownloaded() && !m_file->isDownloadingActive() && !m_file->isDownloadingCompleted())
        {
            m_file->downloadFile();
        }
    }

    if (m_chat->last_message_)
    {
        m_lastMessage = std::make_unique<Message>(m_chat->last_message_.get());
    }

    m_chatPosition = calculateChatPosition();
}

qlonglong Chat::id() const
{
    return m_chat->id_;
}

QString Chat::type() const
{
    if (!m_chat || !m_chat->type_)
    {
        return {};
    }

    switch (m_chat->type_->get_id())
    {
        case td::td_api::chatTypePrivate::ID:
            return "private";

        case td::td_api::chatTypeSecret::ID:
            return "secret";

        case td::td_api::chatTypeBasicGroup::ID:
            return "group";

        case td::td_api::chatTypeSupergroup::ID: {
            if (const auto *chatType = static_cast<const td::td_api::chatTypeSupergroup *>(m_chat->type_.get()))
            {
                return chatType->is_channel_ ? "channel" : "supergroup";
            }
            return {};
        }

        default:
            return {};
    }
}

QString Chat::title() const
{
    return QString::fromStdString(m_chat->title_);
}

File *Chat::photo() const
{
    return m_file.get();
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

qlonglong Chat::lastReadInboxMessageId() const
{
    return m_chat->last_read_inbox_message_id_;
}

qlonglong Chat::lastReadOutboxMessageId() const
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

qlonglong Chat::replyMarkupMessageId() const
{
    return m_chat->reply_markup_message_id_;
}

Message *Chat::draftMessage() const
{
    return {};
}

qlonglong Chat::getOrder() const noexcept
{
    return m_chatPosition ? m_chatPosition->order_ : 0;
}

qlonglong Chat::getTypeId() const noexcept
{
    if (!m_chat || !m_chat->type_)
    {
        return {};  // Return an empty value if m_chat or type is null
    }

    const auto chatTypeId = m_chat->type_->get_id();

    switch (chatTypeId)
    {
        case td::td_api::chatTypePrivate::ID: {
            const auto *privateChat = static_cast<const td::td_api::chatTypePrivate *>(m_chat->type_.get());
            return privateChat ? privateChat->user_id_ : 0;
        }

        case td::td_api::chatTypeSecret::ID: {
            const auto *secretChat = static_cast<const td::td_api::chatTypeSecret *>(m_chat->type_.get());
            return secretChat ? secretChat->secret_chat_id_ : 0;
        }

        case td::td_api::chatTypeBasicGroup::ID: {
            const auto *basicGroupChat = static_cast<const td::td_api::chatTypeBasicGroup *>(m_chat->type_.get());
            return basicGroupChat ? basicGroupChat->basic_group_id_ : 0;
        }

        case td::td_api::chatTypeSupergroup::ID: {
            const auto *supergroupChat = static_cast<const td::td_api::chatTypeSupergroup *>(m_chat->type_.get());
            return supergroupChat ? supergroupChat->supergroup_id_ : 0;
        }

        default:
            return 0;
    }
}

int Chat::muteFor() const noexcept
{
    return m_chat && m_chat->notification_settings_ ? m_chat->notification_settings_->mute_for_ : 0;
}

bool Chat::isMuted() const noexcept
{
    return muteFor() > 0;
}

bool Chat::isPinned() const noexcept
{
    return m_chatPosition ? m_chatPosition->is_pinned_ : false;
}

void Chat::onItemChanged(td::td_api::Object *object)
{
    if (object == nullptr)
    {
        return;  // Early return if object is null
    }

    auto handleChatUpdate = [&](auto *value, bool emitPosition = false) {
        if (value == nullptr || value->chat_id_ != m_chat->id_)
        {
            return;
        }

        auto newChat = m_storageManager->getChat(value->chat_id_);
        if (!newChat)
        {
            return;
        }

        m_chat = std::move(newChat);

        if (emitPosition)
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

    using UpdateHandler = std::function<void(td::td_api::Object *)>;
    static const std::unordered_map<int, UpdateHandler> handlers = {
        {td::td_api::updateChatTitle::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatTitle *>(obj)); }},
        {td::td_api::updateChatPhoto::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatPhoto *>(obj)); }},
        {td::td_api::updateChatPermissions::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatPermissions *>(obj)); }},
        {td::td_api::updateChatLastMessage::ID,
         [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatLastMessage *>(obj), true); }},
        {td::td_api::updateChatPosition::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatPosition *>(obj), true); }},
        {td::td_api::updateChatReadInbox::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatReadInbox *>(obj)); }},
        {td::td_api::updateChatReadOutbox::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatReadOutbox *>(obj)); }},
        {td::td_api::updateChatActionBar::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatActionBar *>(obj)); }},
        {td::td_api::updateChatDraftMessage::ID,
         [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatDraftMessage *>(obj), true); }},
        {td::td_api::updateChatNotificationSettings::ID,
         [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatNotificationSettings *>(obj)); }},
        {td::td_api::updateChatReplyMarkup::ID, [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatReplyMarkup *>(obj)); }},
        {td::td_api::updateChatUnreadMentionCount::ID,
         [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatUnreadMentionCount *>(obj)); }},
        {td::td_api::updateChatIsMarkedAsUnread::ID,
         [&](td::td_api::Object *obj) { handleChatUpdate(static_cast<td::td_api::updateChatIsMarkedAsUnread *>(obj)); }},
    };

    if (auto it = handlers.find(object->get_id()); it != handlers.end())
    {
        it->second(object);  // Call the appropriate handler if found
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

    const auto findPosition = [&](int id, auto &&folder) -> td::td_api::object_ptr<td::td_api::chatPosition> {
        for (const auto &position : positions)
        {
            if (const auto &list = position->list_; list->get_id() == id && folder(*list))  // Always call folder (empty lambda or actual filter)
            {
                return td::td_api::make_object<td::td_api::chatPosition>(Utils::toChatList(m_chatList), position->order_, position->is_pinned_, nullptr);
            }
        }
        return nullptr;
    };

    switch (m_chatList.type)
    {
        case TdApi::ChatListMain:
            return findPosition(td::td_api::chatListMain::ID, [](const auto &) { return true; });

        case TdApi::ChatListArchive:
            return findPosition(td::td_api::chatListArchive::ID, [](const auto &) { return true; });

        case TdApi::ChatListFolder: {
            auto folder = [this](const auto &list) { return static_cast<const td::td_api::chatListFolder &>(list).chat_folder_id_ == m_chatList.folderId; };
            return findPosition(td::td_api::chatListFolder::ID, folder);
        }

        default:
            return nullptr;
    }
}
