#pragma once

#include <QMutex>
#include <QObject>
#include <QVariant>
#include <QVector>

#include <unordered_map>

class StorageManager : public QObject
{
    Q_OBJECT
public:
    StorageManager(const StorageManager &) = delete;
    StorageManager &operator=(const StorageManager &) = delete;

    static StorageManager &getInstance();

    QVector<qint64> getChatIds() const noexcept;

    Q_INVOKABLE QVariantMap getBasicGroup(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getBasicGroupFullInfo(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getChat(qint64 chatId) const;
    Q_INVOKABLE QVariantMap getFile(qint32 fileId) const;
    Q_INVOKABLE QVariant getOption(const QString &name) const;
    Q_INVOKABLE QVariantMap getSupergroup(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getSupergroupFullInfo(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getUser(qint64 userId) const;
    Q_INVOKABLE QVariantMap getUserFullInfo(qint64 userId) const;

    Q_INVOKABLE qint64 getMyId() const;

    void setChat(const QVariantMap &chat) noexcept;

signals:
    void updateChatItem(qint64 chatId);
    void updateChatPosition(qint64 chatId);

private slots:
    void handleBasicGroup(const QVariantMap &basicGroup);
    void handleBasicGroupFullInfo(qint64 basicGroupId, const QVariantMap &basicGroupFullInfo);

    void handleNewChat(const QVariantMap &chat);
    void handleChatTitle(qint64 chatId, const QString &title);
    void handleChatPhoto(qint64 chatId, const QVariantMap &photo);
    void handleChatPermissions(qint64 chatId, const QVariantMap &permissions);
    void handleChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions);
    void handleChatPosition(qint64 chatId, const QVariantMap &position);
    void handleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread);
    void handleChatIsBlocked(qint64 chatId, bool isBlocked);
    void handleChatHasScheduledMessages(qint64 chatId, bool hasScheduledMessages);
    void handleChatDefaultDisableNotification(qint64 chatId, bool defaultDisableNotification);
    void handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId);
    void handleChatUnreadMentionCount(qint64 chatId, int unreadMentionCount);
    void handleChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings);
    void handleChatActionBar(qint64 chatId, const QVariantMap &actionBar);
    void handleChatReplyMarkup(qint64 chatId, qint64 replyMarkupMessageId);
    void handleChatDraftMessage(qint64 chatId, const QVariantMap &draftMessage, const QVariantList &positions);

    void handleFile(const QVariantMap &file);

    void handleOption(const QString &name, const QVariantMap &value);

    void handleSupergroup(const QVariantMap &supergroup);
    void handleSupergroupFullInfo(qint64 supergroupId, const QVariantMap &supergroupFullInfo);

    void handleUserStatus(qint64 userId, const QVariantMap &status);
    void handleUser(const QVariantMap &user);
    void handleUserFullInfo(qint64 userId, const QVariantMap &userFullInfo);

private:
    void setChatPositions(qint64 chatId, const QVariantList &positions) noexcept;

private:
    StorageManager();

    QMutex m_chatMutex;

    QVariantHash m_options;

    std::unordered_map<int, QVariantMap> m_files;
    std::unordered_map<qint64, QVariantMap> m_chats;
    std::unordered_map<qint64, QVariantMap> m_basicGroup;
    std::unordered_map<qint64, QVariantMap> m_basicGroupFullInfo;
    std::unordered_map<qint64, QVariantMap> m_supergroup;
    std::unordered_map<qint64, QVariantMap> m_supergroupFullInfo;
    std::unordered_map<qint64, QVariantMap> m_users;
    std::unordered_map<qint64, QVariantMap> m_userFullInfo;
};
