#pragma once

#include <QObject>
#include <QVariant>
#include <QVector>

#include <unordered_map>

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

    QVariantMap get(qint64 groupId) const;
    QVariantMap getFullInfo(qint64 groupId) const;

private slots:
    void handleUpdateBasicGroup(const QVariantMap &basicGroup);
    void handleUpdateBasicGroupFullInfo(qint64 basicGroupId, const QVariantMap &basicGroupFullInfo);

private:
    std::unordered_map<qint64, QVariantMap> m_basicGroup;
    std::unordered_map<qint64, QVariantMap> m_fullInfo;
};

class ChatStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    QVector<qint64> getIds() const noexcept;

    QVariantMap get(qint64 chatId) const;

signals:
    void updateChatItem(qint64 chatId);
    void updateChatPosition(qint64 chatId);

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
    void setChatPositions(qint64 chatId, const QVariantList &positions) noexcept;

    std::unordered_map<qint64, QVariantMap> m_chats;
};

class FileStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    QVariantMap get(int fileId) const;

private slots:
    void handleUpdateFile(const QVariantMap &file);

private:
    std::unordered_map<int, QVariantMap> m_files;
};

class OptionStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    QVariant get(const QString &name) const;

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

    QVariantMap get(qint64 groupId) const;
    QVariantMap getFullInfo(qint64 groupId) const;

private slots:
    void handleUpdateSupergroup(const QVariantMap &supergroup);
    void handleUpdateSupergroupFullInfo(qint64 supergroupId, const QVariantMap &supergroupFullInfo);

private:
    std::unordered_map<qint64, QVariantMap> m_supergroup;
    std::unordered_map<qint64, QVariantMap> m_fullInfo;
};

class UserStore : public Store
{
    Q_OBJECT

public:
    void initialize(TdApi *controller) override;

    qint64 getMyId() const;
    QVariantMap get(qint64 userId) const;
    QVariantMap getFullInfo(qint64 userId) const;

private slots:
    void handleUpdateUserStatus(qint64 userId, const QVariantMap &status);
    void handleUpdateUser(const QVariantMap &user);
    void handleUpdateUserFullInfo(qint64 userId, const QVariantMap &userFullInfo);

private:
    std::unordered_map<qint64, QVariantMap> m_users;
    std::unordered_map<qint64, QVariantMap> m_fullInfo;
};
