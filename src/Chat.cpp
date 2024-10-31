#include "Chat.hpp"
#include "Localization.hpp"
#include "Message.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <algorithm>
#include <memory>
#include <ranges>

Chat::Chat(td::td_api::object_ptr<td::td_api::chat> chat, QObject *parent)
    : QObject(parent)
    , m_chat(std::move(chat))
    , m_id(m_chat->id_)
    , m_title(QString::fromStdString(m_chat->title_))
    , m_isMarkedAsUnread(m_chat->is_marked_as_unread_)
    , m_unreadCount(m_chat->unread_count_)
    , m_lastReadInboxMessageId(m_chat->last_read_inbox_message_id_)
    , m_lastReadOutboxMessageId(m_chat->last_read_outbox_message_id_)
    , m_unreadMentionCount(m_chat->unread_mention_count_)
    , m_muteFor(m_chat->notification_settings_ ? m_chat->notification_settings_->mute_for_ : 0)
{
    if (auto &photo = m_chat->photo_; photo)
    {
        setPhoto(std::move(photo));
    }

    if (m_chat->last_message_)
    {
        setLastMessage(std::move(m_chat->last_message_));
    }

    setType(std::move(m_chat->type_));
}

qlonglong Chat::id() const noexcept
{
    return m_id;
}

Chat::Type Chat::type() const noexcept
{
    return m_type;
}

QString Chat::title() const noexcept
{
    return m_title;
}

File *Chat::photo() const noexcept
{
    return m_file.get();
}

Message *Chat::lastMessage() const noexcept
{
    return m_lastMessage.get();
}

bool Chat::isMarkedAsUnread() const noexcept
{
    return m_isMarkedAsUnread;
}

int Chat::unreadCount() const noexcept
{
    return m_unreadCount;
}

qlonglong Chat::lastReadInboxMessageId() const noexcept
{
    return m_lastReadInboxMessageId;
}

qlonglong Chat::lastReadOutboxMessageId() const noexcept
{
    return m_lastReadOutboxMessageId;
}

int Chat::unreadMentionCount() const noexcept
{
    return m_unreadMentionCount;
}

int Chat::muteFor() const noexcept
{
    return m_muteFor;
}

bool Chat::isMuted() const noexcept
{
    return m_muteFor > 0;
}

qlonglong Chat::typeId() const noexcept
{
    return m_typeId;
}

std::vector<std::unique_ptr<ChatPosition>> &Chat::positions() noexcept
{
    return m_positions;
}

void Chat::setTitle(std::string_view title) noexcept
{
    auto newTitle = QString::fromStdString(std::string(title));
    if (m_title != newTitle)
    {
        m_title = std::move(newTitle);
        emit chatChanged();
    }
}

void Chat::setPhoto(td::td_api::object_ptr<td::td_api::chatPhotoInfo> photo) noexcept
{
    if (photo && photo->small_)
    {
        m_file = std::make_unique<File>(std::move(photo->small_));

        attemptDownload();

        emit chatChanged();
    }
}

void Chat::setLastMessage(td::td_api::object_ptr<td::td_api::message> lastMessage) noexcept
{
    if (lastMessage)
    {
        m_lastMessage = std::make_unique<Message>(std::move(lastMessage));
        emit chatChanged();
    }
}

void Chat::setPositions(std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> positions) noexcept
{
    if (positions.empty())
    {
        m_positions.clear();
        emit chatChanged();
        return;
    }

    m_positions.reserve(m_positions.size() + positions.size());

    for (auto &&position : positions)
    {
        auto newPosition = std::make_unique<ChatPosition>(std::move(position));

        std::erase_if(m_positions, [&](const auto &value) { return *value->list() == *newPosition->list(); });

        m_positions.emplace_back(std::move(newPosition));
    }

    emit chatChanged();
}

void Chat::setIsMarkedAsUnread(bool isMarkedAsUnread) noexcept
{
    if (m_isMarkedAsUnread != isMarkedAsUnread)
    {
        m_isMarkedAsUnread = isMarkedAsUnread;
        emit chatChanged();
    }
}

void Chat::setUnreadCount(int unreadCount) noexcept
{
    if (m_unreadCount != unreadCount)
    {
        m_unreadCount = unreadCount;
        emit chatChanged();
    }
}

void Chat::setLastReadInboxMessageId(qlonglong lastReadInboxMessageId) noexcept
{
    if (m_lastReadInboxMessageId != lastReadInboxMessageId)
    {
        m_lastReadInboxMessageId = lastReadInboxMessageId;
        emit chatChanged();
    }
}

void Chat::setLastReadOutboxMessageId(qlonglong lastReadOutboxMessageId) noexcept
{
    if (m_lastReadOutboxMessageId != lastReadOutboxMessageId)
    {
        m_lastReadOutboxMessageId = lastReadOutboxMessageId;
        emit chatChanged();
    }
}

void Chat::setUnreadMentionCount(int unreadMentionCount) noexcept
{
    if (m_unreadMentionCount != unreadMentionCount)
    {
        m_unreadMentionCount = unreadMentionCount;
        emit chatChanged();
    }
}

void Chat::setNotificationSettings(td::td_api::object_ptr<td::td_api::chatNotificationSettings> notificationSettings) noexcept
{
    if (notificationSettings)
    {
        m_muteFor = notificationSettings->mute_for_;
        emit chatChanged();
    }
}

void Chat::onFileChanged()
{
    if (m_file && m_file->isDownloadingCompleted())
    {
        emit chatChanged();
    }
}

void Chat::attemptDownload()
{
    if (m_file && m_file->canBeDownloaded() && !m_file->isDownloadingActive() && !m_file->isDownloadingCompleted())
    {
        m_file->downloadFile();
    }
}

void Chat::setType(td::td_api::object_ptr<td::td_api::ChatType> type) noexcept
{
    switch (type->get_id())
    {
        case td::td_api::chatTypePrivate::ID: {
            m_type = Type::Private;
            m_typeId = static_cast<const td::td_api::chatTypePrivate *>(type.get())->user_id_;
            break;
        }
        case td::td_api::chatTypeSecret::ID: {
            m_type = Type::Secret;
            m_typeId = static_cast<const td::td_api::chatTypeSecret *>(type.get())->secret_chat_id_;
            break;
        }
        case td::td_api::chatTypeBasicGroup::ID: {
            m_type = Type::BasicGroup;
            m_typeId = static_cast<const td::td_api::chatTypeBasicGroup *>(type.get())->basic_group_id_;
            break;
        }
        case td::td_api::chatTypeSupergroup::ID: {
            const auto *chatType = static_cast<const td::td_api::chatTypeSupergroup *>(type.get());
            m_type = chatType && chatType->is_channel_ ? Type::Channel : Type::Supergroup;
            m_typeId = static_cast<const td::td_api::chatTypeSupergroup *>(type.get())->supergroup_id_;
            break;
        }
        default: {
            m_type = Type::None;
            m_typeId = 0;
            break;
        }
    }
}
