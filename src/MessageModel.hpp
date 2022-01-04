#pragma once

#include <QAbstractListModel>

#include <unordered_set>

class MessageModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QVariant chat READ getChat NOTIFY chatChanged)
    Q_PROPERTY(QString chatSubtitle READ getChatSubtitle NOTIFY statusChanged)
    Q_PROPERTY(QString chatTitle READ getChatTitle NOTIFY statusChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

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

    QVariant getChat() const noexcept;
    QString getChatSubtitle() const noexcept;
    QString getChatTitle() const noexcept;

    Q_INVOKABLE void loadHistory() noexcept;

    Q_INVOKABLE void openChat(qint64 chatId) noexcept;
    Q_INVOKABLE void closeChat() noexcept;

    Q_INVOKABLE void deleteMessage(qint64 messageId) noexcept;
    Q_INVOKABLE void viewMessage(qint64 messageId) noexcept;

    Q_INVOKABLE QVariantMap get(qint64 messageId) const noexcept;
    Q_INVOKABLE int getIndex(qint64 messageId) const noexcept;

signals:
    void chatChanged();
    void countChanged();
    void moreHistoriesLoaded(int modelIndex);
    void statusChanged();

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

    void handleChatOnlineMemberCount(qint64 chatId, int onlineMemberCount);

    void handleChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId);

    void handleMessage(const QVariantMap &message);
    void handleMessages(const QVariantMap &messages);

private:
    void insertMessages(const QVariantList &messages);

    QString getBasicGroupStatus(const QVariantMap &basicGroup, const QVariantMap &chat) const noexcept;
    QString getChannelStatus(const QVariantMap &supergroup, const QVariantMap &chat) const noexcept;
    QString getSupergroupStatus(const QVariantMap &supergroup, const QVariantMap &chat) const noexcept;

    static QString getUserStatus(const QVariantMap &user) noexcept;

    static QString getMessageDate(int64_t time) noexcept;

    void itemChanged(int64_t index);

    QVariantMap m_chat;
    QList<QVariantMap> m_messages;

    qint64 m_chatId{};

    bool m_loading{};
    bool m_loadingHistory{};

    std::unordered_set<int64_t> m_messageIds;
};
