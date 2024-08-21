#pragma once

#include "TdApi.hpp"

#include <QAbstractListModel>
#include <QTimer>

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
    ~ChatModel() override;

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

    Q_INVOKABLE bool isPinned(int index) const noexcept;
    Q_INVOKABLE bool isMuted(int index) const noexcept;

    Q_INVOKABLE void toggleChatIsPinned(int index);
    Q_INVOKABLE void toggleChatNotificationSettings(int index);

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

    void handleChatItem(qint64 chatId);
    void handleChatPosition(qint64 chatId);

private:
    void clear();

    Client *m_client{};
    Locale *m_locale{};
    StorageManager *m_storageManager{};

    bool m_loading = true;

    int m_count{};

    ChatList m_chatList;

    QTimer *m_sortTimer;
    QTimer *m_loadingTimer;

    std::vector<int64_t> m_chatIds;
};
