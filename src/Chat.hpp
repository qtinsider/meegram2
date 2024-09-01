#pragma once

#include "TdApi.hpp"

#include <td/telegram/td_api.h>

#include <QObject>
#include <QString>
#include <QVariant>

class Message;
class StorageManager;

class Chat : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qint64 id READ id NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap type READ type NOTIFY dataChanged)
    Q_PROPERTY(QString title READ title NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap photo READ photo NOTIFY dataChanged)
    Q_PROPERTY(qint32 accentColorId READ accentColorId NOTIFY dataChanged)
    Q_PROPERTY(qint64 backgroundCustomEmojiId READ backgroundCustomEmojiId NOTIFY dataChanged)
    Q_PROPERTY(qint32 profileAccentColorId READ profileAccentColorId NOTIFY dataChanged)
    Q_PROPERTY(qint64 profileBackgroundCustomEmojiId READ profileBackgroundCustomEmojiId NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap permissions READ permissions NOTIFY dataChanged)
    Q_PROPERTY(Message *lastMessage READ lastMessage NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap messageSenderId READ messageSenderId NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap blockList READ blockList NOTIFY dataChanged)
    Q_PROPERTY(bool hasProtectedContent READ hasProtectedContent NOTIFY dataChanged)
    Q_PROPERTY(bool isTranslatable READ isTranslatable NOTIFY dataChanged)
    Q_PROPERTY(bool isMarkedAsUnread READ isMarkedAsUnread NOTIFY dataChanged)
    Q_PROPERTY(bool viewAsTopics READ viewAsTopics NOTIFY dataChanged)
    Q_PROPERTY(bool hasScheduledMessages READ hasScheduledMessages NOTIFY dataChanged)
    Q_PROPERTY(bool canBeDeletedOnlyForSelf READ canBeDeletedOnlyForSelf NOTIFY dataChanged)
    Q_PROPERTY(bool canBeDeletedForAllUsers READ canBeDeletedForAllUsers NOTIFY dataChanged)
    Q_PROPERTY(bool canBeReported READ canBeReported NOTIFY dataChanged)
    Q_PROPERTY(bool defaultDisableNotification READ defaultDisableNotification NOTIFY dataChanged)
    Q_PROPERTY(qint32 unreadCount READ unreadCount NOTIFY dataChanged)
    Q_PROPERTY(qint64 lastReadInboxMessageId READ lastReadInboxMessageId NOTIFY dataChanged)
    Q_PROPERTY(qint64 lastReadOutboxMessageId READ lastReadOutboxMessageId NOTIFY dataChanged)
    Q_PROPERTY(qint32 unreadMentionCount READ unreadMentionCount NOTIFY dataChanged)
    Q_PROPERTY(qint32 unreadReactionCount READ unreadReactionCount NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap notificationSettings READ notificationSettings NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap availableReactions READ availableReactions NOTIFY dataChanged)
    Q_PROPERTY(qint32 messageAutoDeleteTime READ messageAutoDeleteTime NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap emojiStatus READ emojiStatus NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap background READ background NOTIFY dataChanged)
    Q_PROPERTY(QString themeName READ themeName NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap actionBar READ actionBar NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap businessBotManageBar READ businessBotManageBar NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap videoChat READ videoChat NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap pendingJoinRequests READ pendingJoinRequests NOTIFY dataChanged)
    Q_PROPERTY(qint64 replyMarkupMessageId READ replyMarkupMessageId NOTIFY dataChanged)
    Q_PROPERTY(Message *draftMessage READ draftMessage NOTIFY dataChanged)
    Q_PROPERTY(QString clientData READ clientData NOTIFY dataChanged)

public:
    explicit Chat(QObject *parent = nullptr);
    explicit Chat(qint64 chatId, ChatList chatList, QObject *parent = nullptr);

    qint64 id() const;
    QVariantMap type() const;
    QString title() const;
    QVariantMap photo() const;
    int accentColorId() const;
    qint64 backgroundCustomEmojiId() const;
    int profileAccentColorId() const;
    qint64 profileBackgroundCustomEmojiId() const;
    QVariantMap permissions() const;
    Message *lastMessage() const;
    QVariantMap messageSenderId() const;
    QVariantMap blockList() const;
    bool hasProtectedContent() const;
    bool isTranslatable() const;
    bool isMarkedAsUnread() const;
    bool viewAsTopics() const;
    bool hasScheduledMessages() const;
    bool canBeDeletedOnlyForSelf() const;
    bool canBeDeletedForAllUsers() const;
    bool canBeReported() const;
    bool defaultDisableNotification() const;
    int unreadCount() const;
    qint64 lastReadInboxMessageId() const;
    qint64 lastReadOutboxMessageId() const;
    int unreadMentionCount() const;
    int unreadReactionCount() const;
    QVariantMap notificationSettings() const;
    QVariantMap availableReactions() const;
    int messageAutoDeleteTime() const;
    QVariantMap emojiStatus() const;
    QVariantMap background() const;
    QString themeName() const;
    QVariantMap actionBar() const;
    QVariantMap businessBotManageBar() const;
    QVariantMap videoChat() const;
    QVariantMap pendingJoinRequests() const;
    qint64 replyMarkupMessageId() const;
    Message *draftMessage() const;
    QString clientData() const;

    bool isUnread() const noexcept;

    QString getTitle() const noexcept;

    qint64 getOrder() const noexcept;

    bool isPinned() const noexcept;
    bool isMuted() const noexcept;

    int getMuteDuration() const noexcept;

    bool isCurrentUser() const noexcept;

signals:
    void dataChanged();

    void chatItemUpdated(qint64 chatId);
    void chatPositionUpdated(qint64 chatId);

private slots:
    void onDataChanged(td::td_api::Object *object);

private:
    td::td_api::object_ptr<td::td_api::chatPosition> calculateChatPosition() noexcept;

    qint64 m_chatId;

    ChatList m_chatList;

    td::td_api::chat *m_chat;
    td::td_api::object_ptr<td::td_api::chatPosition> m_chatPosition;

    bool m_showSavedMessages;

    StorageManager *m_storageManager;
};
