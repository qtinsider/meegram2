#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>

#include <map>
#include <memory>
#include <vector>

class Chat;
class ChatInfo;
class Client;
class Locale;
class Message;
class StorageManager;

class MessageModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(Chat *chat READ chat WRITE setChat NOTIFY chatChanged)
    Q_PROPERTY(ChatInfo *chatInfo READ chatInfo NOTIFY chatInfoChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool backFetching READ backFetching NOTIFY backFetchingChanged)

public:
    explicit MessageModel(QObject *parent = nullptr);

    enum Role {
        IdRole = Qt::UserRole + 1,
        SenderRole,
        ChatIdRole,
        IsOutgoingRole,
        DateRole,
        EditDateRole,
        ContentRole,
        // Custom role
        ContentTypeRole,
        IsServiceRole,
        SectionRole
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const override;
    void fetchMore(const QModelIndex &parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const noexcept;

    int count() const noexcept;
    bool loading() const noexcept;
    bool backFetching() const noexcept;

    ChatInfo *chatInfo() const noexcept;

    Chat *chat() const noexcept;
    void setChat(Chat *value) noexcept;

    Q_INVOKABLE Message *getMessage(qlonglong messageId) const noexcept;

    Q_INVOKABLE void openChat() noexcept;
    Q_INVOKABLE void closeChat() noexcept;
    Q_INVOKABLE void getChatHistory(qlonglong fromMessageId, int offset, int limit, bool previous = false) noexcept;
    Q_INVOKABLE void viewMessages(const QList<qlonglong> &messageIds) noexcept;
    Q_INVOKABLE void deleteMessage(qlonglong messageId, bool revoke = false) noexcept;

    Q_INVOKABLE void sendMessage(const QString &message, qlonglong replyToMessageId = 0) noexcept;

    Q_INVOKABLE bool canFetchMoreBack() const noexcept;
    Q_INVOKABLE void fetchMoreBack() noexcept;

signals:
    void chatChanged();
    void chatInfoChanged();
    void countChanged();
    void loadingChanged();

    void backFetchable();
    void backFetched(int numItems);
    void backFetchingChanged();

public slots:
    void refresh() noexcept;

protected:
    void classBegin() override;
    void componentComplete() override;

private slots:
    void handleChatItem() noexcept;
    void handleResult(td::td_api::Object *object) noexcept;

    void handleBasicGroupUpdate(qlonglong groupId) noexcept;
    void handleSupergroupUpdate(qlonglong groupId) noexcept;
    void handleUserUpdate(qlonglong userId) noexcept;

private:
    void handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message) noexcept;
    void handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent) noexcept;
    void handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup) noexcept;
    void handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool isPermanent, bool fromCache) noexcept;

    void handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount) noexcept;

    void handleMessages(std::vector<std::unique_ptr<Message>> &&messages, bool previous) noexcept;

    void loadMessages() noexcept;

    void itemChanged(size_t index) noexcept;

    Chat *m_chat{};

    std::shared_ptr<Client> m_client;

    StorageManager *m_storageManager{};

    int m_onlineCount{0};

    bool m_loading{true};

    bool m_backFetching{false};

    std::unique_ptr<ChatInfo> m_chatInfo;

    std::map<qlonglong, std::unique_ptr<Message>> m_messages;
};
