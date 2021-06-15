#pragma once

#include "TdApi.hpp"

#include <QAbstractListModel>
#include <QPair>
#include <QTimer>

class ChatModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int chatFilterId READ chatFilterId WRITE setChatFilterId NOTIFY statusChanged)
    Q_PROPERTY(TdApi::ChatList chatList READ chatList WRITE setChatList NOTIFY statusChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    explicit ChatModel(QObject *parent = nullptr);
    ~ChatModel() override;

    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TitleRole,
        PhotoRole,
        PermissionsRole,
        LastMessageRole,
        PositionsRole,
        IsMarkedAsUnreadRole,
        IsBlockedRole,
        HasScheduledMessagesRole,
        CanBeDeletedOnlyForSelfRole,
        CanBeDeletedForAllUsersRole,
        CanBeReportedRole,
        DefaultDisableNotificationRole,
        UnreadCountRole,
        LastReadInboxMessageIdRole,
        LastReadOutboxMessageIdRole,
        UnreadMentionCountRole,
        NotificationSettingsRole,
        MessageTtlSettingRole,
        ActionBarRole,
        VoiceChatRole,
        ReplyMarkupMessageIdRole,
        DraftMessageRole,
        ClientDataRole,

        ChatListRole,
        IsMuteRole,
        IsPinnedRole,
        LastMessageAuthorRole,
        LastMessageContentRole,
        LastMessageDateRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const override;
    void fetchMore(const QModelIndex &parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const noexcept;

    int count() const noexcept;

    Q_INVOKABLE QVariantMap get(qint64 chatId) const noexcept;

    Q_INVOKABLE void loadChats() noexcept;

    TdApi::ChatList chatList() const noexcept;
    void setChatList(TdApi::ChatList chatList) noexcept;

    int chatFilterId() const noexcept;
    void setChatFilterId(int chatFilterId) noexcept;
    QPair<qint64, qint64> getChatOrder(qint64 chatId, const QVariantMap &chatList) const noexcept;
    QVariantMap getChatList(TdApi::ChatList chatList) const noexcept;

signals:
    void countChanged();
    void statusChanged();

public slots:
    void clear() noexcept;
    void refresh() noexcept;
    void sort(int column = 0, Qt::SortOrder order = Qt::DescendingOrder) override;

private slots:
    void handleNewChat(const QVariantMap &chat);
    void handleChatTitle(qint64 chatId, const QString &title);
    void handleChatPhoto(qint64 chatId, const QVariantMap &photo);
    void handleChatPermissions(qint64 chatId, const QVariantMap &permissions);
    void handleChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions);
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

    void handleChat(const QVariantMap &chat);
    void handleChats(const QVariantMap &chats);

    void handleChatPhoto(const QVariantMap &file);

private:
    void itemChanged(int64_t index);

    bool m_canFetchMore{};
    int m_chatFilterId{};
    qint64 m_lastChatId{};

    QTimer *m_sortTimer;

    QList<QVariantMap> m_chats;

    TdApi::ChatList m_chatList{TdApi::ChatListMain};
};
