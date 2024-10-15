#pragma once

#include "Chat.hpp"
#include "Message.hpp"

#include <td/telegram/td_api.h>

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>

#include <map>
#include <memory>
#include <vector>

class Client;
class StorageManager;

class MessageModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(Chat *chat READ chat WRITE setChat NOTIFY chatChanged)
    Q_PROPERTY(QString chatSubtitle READ getChatSubtitle NOTIFY chatChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool isEndReached READ isEndReached NOTIFY isEndReachedChanged)

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
        ContentTypeRole,
        BubbleColorRole,
        IsServiceMessageRole,
        SectionRole,
        ServiceMessageRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const noexcept;

    int count() const noexcept;
    bool loading() const noexcept;
    bool isEndReached() const noexcept;

    QString getChatSubtitle() const noexcept;

    Chat *chat() const noexcept;
    void setChat(Chat *value) noexcept;

    Q_INVOKABLE void openChat() noexcept;
    Q_INVOKABLE void closeChat() noexcept;
    Q_INVOKABLE void getChatHistory(qlonglong fromMessageId, int offset, int limit, bool previous = false);
    Q_INVOKABLE void viewMessages(const QStringList &messageIds);
    Q_INVOKABLE void deleteMessage(qlonglong messageId, bool revoke = false) noexcept;

    Q_INVOKABLE void sendMessage(const QString &message, qlonglong replyToMessageId = 0);

    Q_INVOKABLE void loadNextSlice() noexcept;      // Scrolling to top
    Q_INVOKABLE void loadPreviousSlice() noexcept;  // Scrolling to bottom

signals:
    void chatChanged();
    void countChanged();
    void loadingChanged();
    void isEndReachedChanged();

    void moreHistoriesLoaded(int index);

public slots:
    void refresh() noexcept;

protected:
    void classBegin() override;
    void componentComplete() override;

private slots:
    void handleChatItem(qlonglong chatId);
    void handleResult(td::td_api::Object *object);

private:
    void handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message);
    void handleMessageSendSucceeded(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId);
    void handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId,
                                 td::td_api::object_ptr<td::td_api::error> &&error);
    void handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent);
    void handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup);
    void handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned);
    void handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo);
    void handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool isPermanent, bool fromCache);

    void handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount);

    void handleMessages(std::vector<std::unique_ptr<Message> > &&messages, bool previous);

    void loadMessages() noexcept;

    void itemChanged(int64_t index);

    Client *m_client{};
    StorageManager *m_storageManager{};

    Chat *m_chat{};

    int m_onlineCount{0};

    bool m_loading{true};
    bool m_isEndReached{false};

    std::map<qlonglong, std::unique_ptr<Message>> m_messages;
};
