#pragma once

#include "Chat.hpp"
#include "TdApi.hpp"

#include <QAbstractListModel>
#include <QTimer>

#include <memory>
#include <vector>

class Client;
class Locale;
class StorageManager;

class ChatModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

    Q_PROPERTY(TdApi::ChatList chatList READ chatList WRITE setChatList NOTIFY chatListChanged)
    Q_PROPERTY(int chatFolderId READ chatFolderId WRITE setChatFolderId NOTIFY chatListChanged)
public:
    explicit ChatModel(QObject *parent = nullptr);

    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TitleRole,
        PhotoRole,
        LastMessageSenderRole,
        LastMessageContentRole,
        LastMessageDateRole,
        IsPinnedRole,
        UnreadMentionCountRole,
        UnreadCountRole,
        IsMutedRole,
    };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const override;
    void fetchMore(const QModelIndex &parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const;

    int count() const;
    bool loading() const;

    TdApi::ChatList chatList() const;
    void setChatList(TdApi::ChatList value);

    int chatFolderId() const;
    void setChatFolderId(int value);

    Q_INVOKABLE void toggleChatIsPinned(qlonglong chatId, bool isPinned);
    Q_INVOKABLE void toggleChatNotificationSettings(qlonglong chatId, bool isMuted);

signals:
    void countChanged();
    void loadingChanged();

    void chatListChanged();

public slots:
    void populate();
    void refresh();

private slots:
    void loadChats();
    void sortChats();

    void handleChatItem(qlonglong chatId);
    void handleChatPosition(qlonglong chatId);

private:
    void clear();

    bool m_loading{true};

    int m_count{};

    ChatList m_chatList;

    QTimer m_sortTimer;
    QTimer m_loadingTimer;

    Client *m_client{};
    Locale *m_locale{};
    StorageManager *m_storageManager{};

    std::vector<std::shared_ptr<Chat>> m_chats;
};
