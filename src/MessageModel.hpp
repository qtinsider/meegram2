#pragma once

#include "Message.hpp"

#include <QAbstractListModel>

#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

class Chat;
class Client;
class Locale;
class StorageManager;

class MessageModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QObject *locale READ locale WRITE setLocale)
    Q_PROPERTY(QObject *storageManager READ storageManager WRITE setStorageManager)

    Q_PROPERTY(Chat *selectedChat READ selectedChat WRITE setSelectedChat NOTIFY selectedChatChanged)

    Q_PROPERTY(QString chatSubtitle READ getChatSubtitle NOTIFY selectedChatChanged)
    Q_PROPERTY(QString chatTitle READ getChatTitle NOTIFY selectedChatChanged)
    Q_PROPERTY(QString chatPhoto READ getChatPhoto NOTIFY selectedChatChanged)

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
        MessageThreadIdRole,
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

    QObject *storageManager() const;
    void setStorageManager(QObject *storageManager);

    QObject *locale() const;
    void setLocale(QObject *locale);

    Chat *selectedChat() const;
    void setSelectedChat(Chat *value);

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
    void countChanged();
    void moreHistoriesLoaded(int modelIndex);
    void loadingChanged();
    void selectedChatChanged();

public slots:
    void refresh() noexcept;

private slots:
    void handleResult(const QVariantMap &object);

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
    void insertMessages(std::vector<std::unique_ptr<Message>> messages) noexcept;

    void finalizeLoading() noexcept;
    void processMessages(const std::vector<std::unique_ptr<Message>> &messages) noexcept;
    void handleNewMessages(std::vector<std::unique_ptr<Message>> messages) noexcept;

    void loadMessages() noexcept;

    void itemChanged(int64_t index);

    Chat *m_selectedChat{};

    Client *m_client{};
    Locale *m_locale{};
    StorageManager *m_storageManager{};

    int m_onlineCount = 0;

    bool m_loading = true;
    bool m_loadingHistory = true;

    std::vector<std::unique_ptr<Message>> m_messages;
    std::unordered_set<std::optional<qint64>> m_messageIds;
};
