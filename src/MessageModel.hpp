#pragma once

#include <QAbstractListModel>

#include <unordered_set>

class MessageModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString chatId READ getChatId WRITE setChatId NOTIFY chatIdChanged)
    Q_PROPERTY(QString chatSubtitle READ getChatSubtitle NOTIFY statusChanged)
    Q_PROPERTY(QString chatTitle READ getChatTitle NOTIFY statusChanged)
    Q_PROPERTY(QString chatPhoto READ getChatPhoto NOTIFY statusChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool loadingHistory READ loadingHistory NOTIFY loadingChanged)

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

    bool loading() const noexcept;
    bool loadingHistory() const noexcept;

    QString getChatId() const noexcept;
    void setChatId(const QString &value) noexcept;

    QString getChatSubtitle() const noexcept;
    QString getChatTitle() const noexcept;
    QString getChatPhoto() const noexcept;

    Q_INVOKABLE void loadHistory() noexcept;

    Q_INVOKABLE void openChat() noexcept;
    Q_INVOKABLE void closeChat() noexcept;
    Q_INVOKABLE void getChatHistory(qint64 fromMessageId, qint32 offset, qint32 limit);
    Q_INVOKABLE void viewMessages(const QVariantList &messageIds);
    Q_INVOKABLE void deleteMessage(qint64 messageId, bool revoke = false) noexcept;

    Q_INVOKABLE void sendMessage(const QString &message, qint64 replyToMessageId = 0);

signals:
    void chatIdChanged();

    void countChanged();
    void moreHistoriesLoaded(int modelIndex);
    void statusChanged();

    void loadingChanged();

public slots:
    void refresh() noexcept;

private slots:
    void handleNewMessage(const QVariantMap &message);
    void handleMessageSendSucceeded(const QVariantMap &message, qint64 oldMessageId);
    void handleMessageSendFailed(const QVariantMap &message, qint64 oldMessageId, int errorCode, const QString &errorMessage);
    void handleMessageContent(qint64 chatId, qint64 messageId, const QVariantMap &newContent);
    void handleMessageEdited(qint64 chatId, qint64 messageId, int editDate, const QVariantMap &replyMarkup);
    void handleMessageIsPinned(qint64 chatId, qint64 messageId, bool isPinned);
    void handleMessageInteractionInfo(qint64 chatId, qint64 messageId, const QVariantMap &interactionInfo);
    void handleDeleteMessages(qint64 chatId, const QVariantList &messageIds, bool isPermanent, bool fromCache);

    void handleChatOnlineMemberCount(qint64 chatId, int onlineMemberCount);

    void handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId);

private:
    void handleMessages(const QVariantMap &messages);
    void insertMessages(const QVariantList &messages) noexcept;

    void loadMessages() noexcept;

    void itemChanged(int64_t index);

    QVariantMap m_chat;
    QList<QVariantMap> m_messages;

    QString m_chatId{};

    bool m_loading{true};
    bool m_loadingHistory{true};

    std::unordered_set<int64_t> m_messageIds;
};
