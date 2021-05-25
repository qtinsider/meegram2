#pragma once

#include <QAbstractListModel>

#include <unordered_set>

class MessageModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool refreshing READ refreshing NOTIFY refreshingChanged)
public:
    explicit MessageModel(QObject *parent = nullptr);

    enum Role {
        IdRole = Qt::UserRole + 1,
        SenderRole,
        ChatIdRole,
        SendingStateRole,
        SchedulingStateRole,
        IsOutgoingRole,
        IsPinnedRole,
        CanBeEditedRole,
        CanBeForwardedRole,
        CanBeDeletedOnlyForSelfRole,
        CanBeDeletedForAllUsersRole,
        CanGetStatisticsRole,
        CanGetMessageThreadRole,
        IsChannelPostRole,
        ContainsUnreadMentionRole,
        DateRole,
        EditDateRole,
        ForwardInfoRole,
        InteractionInfoRole,
        ReplyInChatIdRole,
        ReplyToMessageIdRole,
        MessageThreadIdRole,
        TtlRole,
        TtlExpiresInRole,
        ViaBotUserIdRole,
        AuthorSignatureRole,
        MediaAlbumIdRole,
        RestrictionReasonRole,
        ContentRole,
        ReplyMarkupRole,
        // Custom role
        BubbleColorRole,
        IsServiceMessageRole,
        SectionRole,
        ServiceMessageRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const override;
    void fetchMore(const QModelIndex &parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const noexcept;

    int count() const noexcept;

    bool refreshing() const noexcept;

    Q_INVOKABLE void loadHistory() noexcept;

    Q_INVOKABLE void openChat(qint64 chatId) noexcept;
    Q_INVOKABLE void closeChat(qint64 chatId) noexcept;

    Q_INVOKABLE void deleteMessage(qint64 messageId) noexcept;
    Q_INVOKABLE void viewMessage(qint64 messageId) noexcept;

    Q_INVOKABLE QVariantMap get(qint64 messageId) const noexcept;
    Q_INVOKABLE int indexOf(qint64 messageId) const noexcept;
    Q_INVOKABLE void scrollToMessage(qint64 messageId) noexcept;

signals:
    void countChanged();
    void refreshingChanged();

public slots:
    void clearAll() noexcept;

private slots:
    void handleNewMessage(const QVariantMap &message);
    void handleMessageSendSucceeded(const QVariantMap &message, qint64 oldMessageId);
    void handleMessageSendFailed(const QVariantMap &message, qint64 oldMessageId, int errorCode, const QString &errorMessage);
    void handleMessageContent(qint64 chatId, qint64 messageId, const QVariantMap &newContent);
    void handleMessageEdited(qint64 chatId, qint64 messageId, int editDate, const QVariantMap &replyMarkup);
    void handleMessageIsPinned(qint64 chatId, qint64 messageId, bool isPinned);
    void handleMessageInteractionInfo(qint64 chatId, qint64 messageId, const QVariantMap &interactionInfo);
    void handleDeleteMessages(qint64 chatId, const QVariantList &messageIds, bool isPermanent, bool fromCache);

    void handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId);

    void handleMessage(const QVariantMap &message);
    void handleMessages(const QVariantMap &messages);

private:
    void insertMessages(const QVariantList &messages);
    void itemChanged(int64_t index);

    qint64 m_chatId{};

    QVariantMap m_chat;
    QList<QVariantMap> m_messages;

    bool m_refreshing{true};

    std::unordered_set<int64_t> m_uniqueIds;
};
