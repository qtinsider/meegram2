#pragma once

#include <QObject>
#include <QVariant>

class TdApi;

class Store : public QObject
{
public:
    Store() = default;
    virtual ~Store() = default;

    Store(const Store &) = delete;
    Store &operator=(const Store &) = delete;

    virtual void initialize(TdApi *controller) = 0;
};

class BasicGroupStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    Q_INVOKABLE QVariantMap get(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getFullInfo(qint64 groupId) const;

private slots:
    void handleUpdateBasicGroup(const QVariantMap &basicGroup);
    void handleUpdateBasicGroupFullInfo(qint64 basicGroupId, const QVariantMap &basicGroupFullInfo);

private:
    QHash<qint64, QVariantMap> m_basicGroup;
    QHash<qint64, QVariantMap> m_fullInfo;
};

class ChatStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    [[nodiscard]] QList<qint64> getIds() const noexcept;

    Q_INVOKABLE QVariantMap get(qint64 chatId) const;

private slots:
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

private:
    QHash<qint64, QVariantMap> m_chats;
};

class FileStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    Q_INVOKABLE QVariantMap get(int fileId) const;

private slots:
    void handleUpdateFile(const QVariantMap &file);

private:
    QHash<int, QVariantMap> m_files;
};

class OptionStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    Q_INVOKABLE QVariant get(const QString &name) const;

private slots:
    void handleUpdateOption(const QString &name, const QVariantMap &value);

private:
    QVariantHash m_options;
};

class SupergroupStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    Q_INVOKABLE QVariantMap get(qint64 groupId) const;
    Q_INVOKABLE QVariantMap getFullInfo(qint64 groupId) const;

private slots:
    void handleUpdateSupergroup(const QVariantMap &supergroup);
    void handleUpdateSupergroupFullInfo(qint64 supergroupId, const QVariantMap &supergroupFullInfo);

private:
    QHash<qint64, QVariantMap> m_supergroup;
    QHash<qint64, QVariantMap> m_fullInfo;
};

class UserStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    Q_INVOKABLE qint64 getMyId() const;
    Q_INVOKABLE QVariantMap get(qint64 userId) const;
    Q_INVOKABLE QVariantMap getFullInfo(qint64 userId) const;

private slots:
    void handleUpdateUserStatus(qint64 userId, const QVariantMap &status);
    void handleUpdateUser(const QVariantMap &user);
    void handleUpdateUserFullInfo(qint64 userId, const QVariantMap &userFullInfo);

private:
    QHash<qint64, QVariantMap> m_users;
    QHash<qint64, QVariantMap> m_fullInfo;
};
