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
class BasicGroup;
class Locale;
class Message;
class StorageManager;
class Supergroup;
class User;

class MessageModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(Chat *chat READ chat WRITE setChat NOTIFY chatChanged)
    Q_PROPERTY(ChatInfo *chatInfo READ chatInfo NOTIFY chatInfoChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool isEndReached READ isEndReached NOTIFY isEndReachedChanged)

public:
    explicit MessageModel(QObject *parent = nullptr);

    enum Role {
        IdRole = Qt::UserRole + 1,
        SenderRole,
        ChatIdRole,
        IsOutgoingRole,
        IsPinnedRole,
        DateRole,
        EditDateRole,
        ContentRole,
        // Custom role
        ContentTypeRole,
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

    ChatInfo *chatInfo() const noexcept;

    Chat *chat() const noexcept;
    void setChat(Chat *value) noexcept;

    Q_INVOKABLE void openChat() noexcept;
    Q_INVOKABLE void closeChat() noexcept;
    Q_INVOKABLE void getChatHistory(qlonglong fromMessageId, int offset, int limit, bool previous = false);
    Q_INVOKABLE void viewMessages(const QList<qlonglong> &messageIds);
    Q_INVOKABLE void deleteMessage(qlonglong messageId, bool revoke = false) noexcept;

    Q_INVOKABLE void sendMessage(const QString &message, qlonglong replyToMessageId = 0);

    Q_INVOKABLE void loadNextSlice() noexcept;      // Scrolling to top
    Q_INVOKABLE void loadPreviousSlice() noexcept;  // Scrolling to bottom

signals:
    void chatChanged();
    void chatInfoChanged();
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
    void handleChatItem();
    void handleResult(td::td_api::Object *object);

    void handleUserUpdate(qlonglong userId);
    void handleBasicGroupUpdate(qlonglong groupId);
    void handleSupergroupUpdate(qlonglong groupId);

private:
    void handleNewMessage(td::td_api::object_ptr<td::td_api::message> &&message);
    void handleMessageSendSucceeded(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId);
    void handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId,
                                 td::td_api::object_ptr<td::td_api::error> &&error);
    void handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent);
    void handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup);
    void handleDeleteMessages(qlonglong chatId, std::vector<int64_t> &&messageIds, bool isPermanent, bool fromCache);

    void handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount);

    void handleMessages(std::vector<std::unique_ptr<Message>> &&messages, bool previous);

    void loadMessages() noexcept;

    void itemChanged(size_t index);

    Chat *m_chat{};

    std::shared_ptr<Client> m_client;

    StorageManager *m_storageManager{};

    int m_onlineCount{0};

    bool m_loading{true};
    bool m_isEndReached{false};

    std::unique_ptr<ChatInfo> m_chatInfo;

    std::map<qlonglong, std::unique_ptr<Message>> m_messages;
};

class ChatInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString status READ status NOTIFY infoChanged)

public:
    explicit ChatInfo(Chat *chat, QObject *parent = nullptr);

    QString title() const noexcept;
    QString status() const noexcept;

    void setOnlineCount(int onlineCount);

    void setUser(std::shared_ptr<User> user);
    void setBasicGroup(std::shared_ptr<BasicGroup> group);
    void setSupergroup(std::shared_ptr<Supergroup> group);

signals:
    void infoChanged();

private:
    void initializeMembers();
    void updateStatus();

    QString formatStatus(int memberCount, const char *memberKey, const char *onlineKey) const;
    int getMemberCountWithFallback() const;
    bool isServiceNotification() const;
    QString formatUserStatus() const;
    QString formatOfflineStatus() const;

    int m_onlineCount{};

    QString m_title, m_status;

    Chat *m_chat{};
    StorageManager *m_storageManager{};

    qlonglong m_chatTypeId{};

    std::shared_ptr<User> m_user;
    std::shared_ptr<BasicGroup> m_basicGroup;
    std::shared_ptr<Supergroup> m_supergroup;
};
