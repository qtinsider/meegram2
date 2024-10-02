#pragma once

#include "File.hpp"
#include "Message.hpp"
#include "TdApi.hpp"

#include <td/telegram/td_api.h>

#include <QObject>

#include <memory>

class StorageManager;

class Chat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id NOTIFY chatItemUpdated)
    Q_PROPERTY(QString type READ type NOTIFY chatItemUpdated)
    Q_PROPERTY(QString title READ title NOTIFY chatItemUpdated)
    Q_PROPERTY(File *photo READ photo NOTIFY chatItemUpdated)
    Q_PROPERTY(Message *lastMessage READ lastMessage NOTIFY chatItemUpdated)
    Q_PROPERTY(bool isMarkedAsUnread READ isMarkedAsUnread NOTIFY chatItemUpdated)
    Q_PROPERTY(bool hasScheduledMessages READ hasScheduledMessages NOTIFY chatItemUpdated)
    Q_PROPERTY(bool canBeDeletedOnlyForSelf READ canBeDeletedOnlyForSelf NOTIFY chatItemUpdated)
    Q_PROPERTY(bool canBeDeletedForAllUsers READ canBeDeletedForAllUsers NOTIFY chatItemUpdated)
    Q_PROPERTY(int unreadCount READ unreadCount NOTIFY chatItemUpdated)
    Q_PROPERTY(qlonglong lastReadInboxMessageId READ lastReadInboxMessageId NOTIFY chatItemUpdated)
    Q_PROPERTY(qlonglong lastReadOutboxMessageId READ lastReadOutboxMessageId NOTIFY chatItemUpdated)
    Q_PROPERTY(int unreadMentionCount READ unreadMentionCount NOTIFY chatItemUpdated)
    Q_PROPERTY(int unreadReactionCount READ unreadReactionCount NOTIFY chatItemUpdated)
    Q_PROPERTY(int messageAutoDeleteTime READ messageAutoDeleteTime NOTIFY chatItemUpdated)
    Q_PROPERTY(qlonglong replyMarkupMessageId READ replyMarkupMessageId NOTIFY chatItemUpdated)
    Q_PROPERTY(Message *draftMessage READ draftMessage NOTIFY chatItemUpdated)

    Q_PROPERTY(bool isMe READ isMe NOTIFY chatItemUpdated)
    Q_PROPERTY(bool isPinned READ isPinned NOTIFY chatItemUpdated)
    Q_PROPERTY(int muteFor READ muteFor NOTIFY chatItemUpdated)
    Q_PROPERTY(bool isMuted READ isMuted NOTIFY chatItemUpdated)

public:
    explicit Chat(QObject *parent = nullptr);
    explicit Chat(qlonglong chatId, ChatList chatList, QObject *parent = nullptr);

    qlonglong id() const;
    QString type() const;
    QString title() const;
    File *photo() const;
    Message *lastMessage() const;
    bool isMarkedAsUnread() const;
    bool hasScheduledMessages() const;
    bool canBeDeletedOnlyForSelf() const;
    bool canBeDeletedForAllUsers() const;
    int unreadCount() const;
    qlonglong lastReadInboxMessageId() const;
    qlonglong lastReadOutboxMessageId() const;
    int unreadMentionCount() const;
    int unreadReactionCount() const;
    int messageAutoDeleteTime() const;
    qlonglong replyMarkupMessageId() const;
    Message *draftMessage() const;

    qlonglong getOrder() const noexcept;
    qlonglong getChatTypeId() const noexcept;

    Q_INVOKABLE bool isMe() const noexcept;
    Q_INVOKABLE bool isPinned() const noexcept;
    Q_INVOKABLE int muteFor() const noexcept;
    Q_INVOKABLE bool isMuted() const noexcept;

signals:
    void chatItemUpdated(qlonglong chatId);
    void chatPositionUpdated(qlonglong chatId);

private slots:
    void onDataChanged(td::td_api::Object *object);
    void onFileChanged();

private:
    td::td_api::object_ptr<td::td_api::chatPosition> calculateChatPosition() noexcept;

    qlonglong m_chatId;

    ChatList m_chatList;

    td::td_api::chat *m_chat;
    td::td_api::object_ptr<td::td_api::chatPosition> m_chatPosition;

    bool m_showSavedMessages;

    std::unique_ptr<File> m_file;
    std::unique_ptr<Message> m_lastMessage;

    StorageManager *m_storageManager;
};

Q_DECLARE_METATYPE(Chat *);
