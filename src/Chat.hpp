#pragma once

#include "ChatPosition.hpp"
#include "File.hpp"
#include "Message.hpp"
#include "Utils.hpp"

#include <td/telegram/td_api.h>

#include <QObject>

#include <memory>

class Chat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id NOTIFY chatChanged)
    Q_PROPERTY(QString type READ type NOTIFY chatChanged)
    Q_PROPERTY(QString title READ title NOTIFY chatChanged)
    Q_PROPERTY(File *photo READ photo NOTIFY chatChanged)
    Q_PROPERTY(Message *lastMessage READ lastMessage NOTIFY chatChanged)
    Q_PROPERTY(bool isMarkedAsUnread READ isMarkedAsUnread NOTIFY chatChanged)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY chatChanged)
    Q_PROPERTY(qlonglong lastReadInboxMessageId READ lastReadInboxMessageId NOTIFY chatChanged)
    Q_PROPERTY(qlonglong lastReadOutboxMessageId READ lastReadOutboxMessageId NOTIFY chatChanged)
    Q_PROPERTY(int unreadMentionCount READ unreadMentionCount NOTIFY chatChanged)

    Q_PROPERTY(int muteFor READ muteFor NOTIFY chatChanged)
    Q_PROPERTY(bool isMuted READ isMuted NOTIFY chatChanged)

public:
    explicit Chat(td::td_api::object_ptr<td::td_api::chat> chat, QObject *parent = nullptr);

    qlonglong id() const noexcept;
    QString type() const noexcept;
    QString title() const noexcept;
    File *photo() const noexcept;
    Message *lastMessage() const noexcept;
    bool isMarkedAsUnread() const noexcept;
    int unreadCount() const noexcept;
    qlonglong lastReadInboxMessageId() const noexcept;
    qlonglong lastReadOutboxMessageId() const noexcept;
    int unreadMentionCount() const noexcept;

    qlonglong getTypeId() const noexcept;

    int muteFor() const noexcept;
    bool isMuted() const noexcept;

    std::vector<std::unique_ptr<ChatPosition>> &positions() noexcept;

    void setTitle(std::string_view title) noexcept;
    void setPhoto(td::td_api::object_ptr<td::td_api::chatPhotoInfo> photo) noexcept;
    void setLastMessage(td::td_api::object_ptr<td::td_api::message> lastMessage) noexcept;
    void setPositions(std::vector<td::td_api::object_ptr<td::td_api::chatPosition>> positions) noexcept;
    void setIsMarkedAsUnread(bool isMarkedAsUnread) noexcept;
    void setUnreadCount(int unreadCount) noexcept;
    void setLastReadInboxMessageId(qlonglong lastReadInboxMessageId) noexcept;
    void setLastReadOutboxMessageId(qlonglong lastReadOutboxMessageId) noexcept;
    void setUnreadMentionCount(int unreadMentionCount) noexcept;
    void setNotificationSettings(td::td_api::object_ptr<td::td_api::chatNotificationSettings> notificationSettings) noexcept;

signals:
    void chatChanged();

private slots:
    void onFileChanged();

private:
    void attemptDownload();

    td::td_api::object_ptr<td::td_api::chat> m_chat;

    qlonglong m_id;
    int m_typeId;
    QString m_title;
    bool m_isMarkedAsUnread;
    int m_unreadCount;
    qlonglong m_lastReadInboxMessageId;
    qlonglong m_lastReadOutboxMessageId;
    int m_unreadMentionCount;

    bool m_muteFor;

    std::unique_ptr<File> m_file;
    std::unique_ptr<Message> m_lastMessage;

    std::vector<std::unique_ptr<ChatPosition>> m_positions;
};
