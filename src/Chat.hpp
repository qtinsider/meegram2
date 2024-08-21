#pragma once

#include <QObject>
#include <QString>
#include <QVariant>

class Message;

class Chat : public QObject {
    Q_OBJECT

    Q_PROPERTY(qint64 id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QVariantMap type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(QVariantMap photo READ photo WRITE setPhoto NOTIFY photoChanged)
    Q_PROPERTY(qint32 accentColorId READ accentColorId WRITE setAccentColorId
                   NOTIFY accentColorIdChanged)
    Q_PROPERTY(
        qint64 backgroundCustomEmojiId READ backgroundCustomEmojiId WRITE
            setBackgroundCustomEmojiId NOTIFY backgroundCustomEmojiIdChanged)
    Q_PROPERTY(qint32 profileAccentColorId READ profileAccentColorId WRITE
                   setProfileAccentColorId NOTIFY profileAccentColorIdChanged)
    Q_PROPERTY(
        qint64 profileBackgroundCustomEmojiId READ profileBackgroundCustomEmojiId
            WRITE setProfileBackgroundCustomEmojiId NOTIFY
                profileBackgroundCustomEmojiIdChanged)
    Q_PROPERTY(QVariantMap permissions READ permissions WRITE setPermissions
                   NOTIFY permissionsChanged)
    Q_PROPERTY(Message *lastMessage READ lastMessage WRITE setLastMessage NOTIFY
                   lastMessageChanged)
    Q_PROPERTY(QVariantList positions READ positions WRITE setPositions NOTIFY
                   positionsChanged)
    Q_PROPERTY(QVariantList chatLists READ chatLists WRITE setChatLists NOTIFY
                   chatListsChanged)
    Q_PROPERTY(QVariantMap messageSenderId READ messageSenderId WRITE
                   setMessageSenderId NOTIFY messageSenderIdChanged)
    Q_PROPERTY(QVariantMap blockList READ blockList WRITE setBlockList NOTIFY
                   blockListChanged)
    Q_PROPERTY(bool hasProtectedContent READ hasProtectedContent WRITE
                   setHasProtectedContent NOTIFY hasProtectedContentChanged)
    Q_PROPERTY(bool isTranslatable READ isTranslatable WRITE setIsTranslatable
                   NOTIFY isTranslatableChanged)
    Q_PROPERTY(bool isMarkedAsUnread READ isMarkedAsUnread WRITE
                   setIsMarkedAsUnread NOTIFY isMarkedAsUnreadChanged)
    Q_PROPERTY(bool viewAsTopics READ viewAsTopics WRITE setViewAsTopics NOTIFY
                   viewAsTopicsChanged)
    Q_PROPERTY(bool hasScheduledMessages READ hasScheduledMessages WRITE
                   setHasScheduledMessages NOTIFY hasScheduledMessagesChanged)
    Q_PROPERTY(
        bool canBeDeletedOnlyForSelf READ canBeDeletedOnlyForSelf WRITE
            setCanBeDeletedOnlyForSelf NOTIFY canBeDeletedOnlyForSelfChanged)
    Q_PROPERTY(
        bool canBeDeletedForAllUsers READ canBeDeletedForAllUsers WRITE
            setCanBeDeletedForAllUsers NOTIFY canBeDeletedForAllUsersChanged)
    Q_PROPERTY(bool canBeReported READ canBeReported WRITE setCanBeReported NOTIFY
                   canBeReportedChanged)
    Q_PROPERTY(bool defaultDisableNotification READ defaultDisableNotification
                   WRITE setDefaultDisableNotification NOTIFY
                       defaultDisableNotificationChanged)
    Q_PROPERTY(qint32 unreadCount READ unreadCount WRITE setUnreadCount NOTIFY
                   unreadCountChanged)
    Q_PROPERTY(qint64 lastReadInboxMessageId READ lastReadInboxMessageId WRITE
                   setLastReadInboxMessageId NOTIFY lastReadInboxMessageIdChanged)
    Q_PROPERTY(
        qint64 lastReadOutboxMessageId READ lastReadOutboxMessageId WRITE
            setLastReadOutboxMessageId NOTIFY lastReadOutboxMessageIdChanged)
    Q_PROPERTY(qint32 unreadMentionCount READ unreadMentionCount WRITE
                   setUnreadMentionCount NOTIFY unreadMentionCountChanged)
    Q_PROPERTY(qint32 unreadReactionCount READ unreadReactionCount WRITE
                   setUnreadReactionCount NOTIFY unreadReactionCountChanged)
    Q_PROPERTY(QVariantMap notificationSettings READ notificationSettings WRITE
                   setNotificationSettings NOTIFY notificationSettingsChanged)
    Q_PROPERTY(QVariantMap availableReactions READ availableReactions WRITE
                   setAvailableReactions NOTIFY availableReactionsChanged)
    Q_PROPERTY(qint32 messageAutoDeleteTime READ messageAutoDeleteTime WRITE
                   setMessageAutoDeleteTime NOTIFY messageAutoDeleteTimeChanged)
    Q_PROPERTY(QVariantMap emojiStatus READ emojiStatus WRITE setEmojiStatus
                   NOTIFY emojiStatusChanged)
    Q_PROPERTY(QVariantMap background READ background WRITE setBackground NOTIFY
                   backgroundChanged)
    Q_PROPERTY(QString themeName READ themeName WRITE setThemeName NOTIFY
                   themeNameChanged)
    Q_PROPERTY(QVariantMap actionBar READ actionBar WRITE setActionBar NOTIFY
                   actionBarChanged)
    Q_PROPERTY(QVariantMap businessBotManageBar READ businessBotManageBar WRITE
                   setBusinessBotManageBar NOTIFY businessBotManageBarChanged)
    Q_PROPERTY(QVariantMap videoChat READ videoChat WRITE setVideoChat NOTIFY
                   videoChatChanged)
    Q_PROPERTY(QVariantMap pendingJoinRequests READ pendingJoinRequests WRITE
                   setPendingJoinRequests NOTIFY pendingJoinRequestsChanged)
    Q_PROPERTY(qint64 replyMarkupMessageId READ replyMarkupMessageId WRITE
                   setReplyMarkupMessageId NOTIFY replyMarkupMessageIdChanged)
    Q_PROPERTY(Message *draftMessage READ draftMessage WRITE setDraftMessage
                   NOTIFY draftMessageChanged)
    Q_PROPERTY(QString clientData READ clientData WRITE setClientData NOTIFY
                   clientDataChanged)

public:
    explicit Chat(QObject *parent = nullptr);
    ~Chat();

    qint64 id() const;
    QVariantMap type() const;
    QString title() const;
    QVariantMap photo() const;
    qint32 accentColorId() const;
    qint64 backgroundCustomEmojiId() const;
    qint32 profileAccentColorId() const;
    qint64 profileBackgroundCustomEmojiId() const;
    QVariantMap permissions() const;
    Message *lastMessage() const;
    QVariantList positions() const;
    QVariantList chatLists() const;
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
    qint32 unreadCount() const;
    qint64 lastReadInboxMessageId() const;
    qint64 lastReadOutboxMessageId() const;
    qint32 unreadMentionCount() const;
    qint32 unreadReactionCount() const;
    QVariantMap notificationSettings() const;
    QVariantMap availableReactions() const;
    qint32 messageAutoDeleteTime() const;
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

    void setId(qint64 id);
    void setType(const QVariantMap &type);
    void setTitle(const QString &title);
    void setPhoto(const QVariantMap &photo);
    void setAccentColorId(qint32 accentColorId);
    void setBackgroundCustomEmojiId(qint64 backgroundCustomEmojiId);
    void setProfileAccentColorId(qint32 profileAccentColorId);
    void setProfileBackgroundCustomEmojiId(qint64 profileBackgroundCustomEmojiId);
    void setPermissions(const QVariantMap &permissions);
    void setLastMessage(Message *lastMessage);
    void setPositions(const QVariantList &positions);
    void setChatLists(const QVariantList &chatLists);
    void setMessageSenderId(const QVariantMap &messageSenderId);
    void setBlockList(const QVariantMap &blockList);
    void setHasProtectedContent(bool hasProtectedContent);
    void setIsTranslatable(bool isTranslatable);
    void setIsMarkedAsUnread(bool isMarkedAsUnread);
    void setViewAsTopics(bool viewAsTopics);
    void setHasScheduledMessages(bool hasScheduledMessages);
    void setCanBeDeletedOnlyForSelf(bool canBeDeletedOnlyForSelf);
    void setCanBeDeletedForAllUsers(bool canBeDeletedForAllUsers);
    void setCanBeReported(bool canBeReported);
    void setDefaultDisableNotification(bool defaultDisableNotification);
    void setUnreadCount(qint32 unreadCount);
    void setLastReadInboxMessageId(qint64 lastReadInboxMessageId);
    void setLastReadOutboxMessageId(qint64 lastReadOutboxMessageId);
    void setUnreadMentionCount(qint32 unreadMentionCount);
    void setUnreadReactionCount(qint32 unreadReactionCount);
    void setNotificationSettings(const QVariantMap &notificationSettings);
    void setAvailableReactions(const QVariantMap &availableReactions);
    void setMessageAutoDeleteTime(qint32 messageAutoDeleteTime);
    void setEmojiStatus(const QVariantMap &emojiStatus);
    void setBackground(const QVariantMap &background);
    void setThemeName(const QString &themeName);
    void setActionBar(const QVariantMap &actionBar);
    void setBusinessBotManageBar(const QVariantMap &businessBotManageBar);
    void setVideoChat(const QVariantMap &videoChat);
    void setPendingJoinRequests(const QVariantMap &pendingJoinRequests);
    void setReplyMarkupMessageId(qint64 replyMarkupMessageId);
    void setDraftMessage(Message *draftMessage);
    void setClientData(const QString &clientData);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void idChanged();
    void typeChanged();
    void titleChanged();
    void photoChanged();
    void accentColorIdChanged();
    void backgroundCustomEmojiIdChanged();
    void profileAccentColorIdChanged();
    void profileBackgroundCustomEmojiIdChanged();
    void permissionsChanged();
    void lastMessageChanged();
    void positionsChanged();
    void chatListsChanged();
    void messageSenderIdChanged();
    void blockListChanged();
    void hasProtectedContentChanged();
    void isTranslatableChanged();
    void isMarkedAsUnreadChanged();
    void viewAsTopicsChanged();
    void hasScheduledMessagesChanged();
    void canBeDeletedOnlyForSelfChanged();
    void canBeDeletedForAllUsersChanged();
    void canBeReportedChanged();
    void defaultDisableNotificationChanged();
    void unreadCountChanged();
    void lastReadInboxMessageIdChanged();
    void lastReadOutboxMessageIdChanged();
    void unreadMentionCountChanged();
    void unreadReactionCountChanged();
    void notificationSettingsChanged();
    void availableReactionsChanged();
    void messageAutoDeleteTimeChanged();
    void emojiStatusChanged();
    void backgroundChanged();
    void themeNameChanged();
    void actionBarChanged();
    void businessBotManageBarChanged();
    void videoChatChanged();
    void pendingJoinRequestsChanged();
    void replyMarkupMessageIdChanged();
    void draftMessageChanged();
    void clientDataChanged();

private:
    qint64 m_id;
    QVariantMap m_type;
    QString m_title;
    QVariantMap m_photo;
    qint32 m_accentColorId;
    qint64 m_backgroundCustomEmojiId;
    qint32 m_profileAccentColorId;
    qint64 m_profileBackgroundCustomEmojiId;
    QVariantMap m_permissions;
    Message *m_lastMessage;
    QVariantList m_positions;
    QVariantList m_chatLists;
    QVariantMap m_messageSenderId;
    QVariantMap m_blockList;
    bool m_hasProtectedContent;
    bool m_isTranslatable;
    bool m_isMarkedAsUnread;
    bool m_viewAsTopics;
    bool m_hasScheduledMessages;
    bool m_canBeDeletedOnlyForSelf;
    bool m_canBeDeletedForAllUsers;
    bool m_canBeReported;
    bool m_defaultDisableNotification;
    qint32 m_unreadCount;
    qint64 m_lastReadInboxMessageId;
    qint64 m_lastReadOutboxMessageId;
    qint32 m_unreadMentionCount;
    qint32 m_unreadReactionCount;
    QVariantMap m_notificationSettings;
    QVariantMap m_availableReactions;
    qint32 m_messageAutoDeleteTime;
    QVariantMap m_emojiStatus;
    QVariantMap m_background;
    QString m_themeName;
    QVariantMap m_actionBar;
    QVariantMap m_businessBotManageBar;
    QVariantMap m_videoChat;
    QVariantMap m_pendingJoinRequests;
    qint64 m_replyMarkupMessageId;
    Message *m_draftMessage;
    QString m_clientData;
};
