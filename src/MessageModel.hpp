#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>

#include <memory>
#include <optional>
#include <unordered_set>
#include <vector>

class Client;
class Locale;
class StorageManager;

class MessageModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QString chatId READ getChatId WRITE setChatId NOTIFY selectedChatChanged)

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
    Q_INVOKABLE void getChatHistory(qlonglong fromMessageId, int offset, int limit);
    void viewMessages(std::vector<int64_t> &&messageIds);
    Q_INVOKABLE void deleteMessage(qlonglong messageId, bool revoke = false) noexcept;

    Q_INVOKABLE void sendMessage(const QString &message, qlonglong replyToMessageId = 0);

signals:
    void countChanged();
    void moreHistoriesLoaded(int modelIndex);
    void loadingChanged();
    void selectedChatChanged();

public slots:
    void refresh() noexcept;

protected:
    void classBegin() override;
    void componentComplete() override;

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message);
    void handleMessageSendSucceeded(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId);
    void handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId, td::td_api::object_ptr<td::td_api::error> &&error);
    void handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent);
    void handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup);
    void handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned);
    void handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo);
    void handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool isPermanent, bool fromCache);

    void handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount);

    void handleChatReadInbox(qlonglong chatId, qlonglong lastReadInboxMessageId, int unreadCount);
    void handleChatReadOutbox(qlonglong chatId, qlonglong lastReadOutboxMessageId);

    void handleMessages(td::td_api::object_ptr<td::td_api::messages> &&messages);
    void insertMessages(std::vector<td::td_api::object_ptr<td::td_api::message>> &&messages) noexcept;

    void finalizeLoading() noexcept;
    void processMessages(const std::vector<td::td_api::object_ptr<td::td_api::message>> &messages) noexcept;
    void handleNewMessages(std::vector<td::td_api::object_ptr<td::td_api::message>> &&messages) noexcept;

    void loadMessages() noexcept;

    void itemChanged(int64_t index);

    Client *m_client{};
    Locale *m_locale{};
    StorageManager *m_storageManager{};

    int m_onlineCount = 0;

    bool m_loading = true;
    bool m_loadingHistory = true;

    const td::td_api::chat *m_selectedChat{};

    std::unordered_set<std::optional<qlonglong>> m_messageIds;
    std::vector<td::td_api::object_ptr<td::td_api::message>> m_messages;
};
