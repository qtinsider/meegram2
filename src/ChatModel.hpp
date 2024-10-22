#pragma once

#include "Chat.hpp"
#include "ChatPosition.hpp"

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

    Q_PROPERTY(ChatList *list READ list WRITE setList NOTIFY listChanged)
public:
    explicit ChatModel(QObject *parent = nullptr);

    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TitleRole,
        PhotoRole,
        LastMessage,
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

    ChatList *list() const;
    void setList(ChatList *value);

    Q_INVOKABLE void toggleChatIsPinned(qlonglong chatId, bool isPinned);
    Q_INVOKABLE void toggleChatNotificationSettings(qlonglong chatId, bool isMuted);

    Q_INVOKABLE ChatPosition *getChatPosition(Chat *chat, ChatList *list) const;

signals:
    void countChanged();
    void loadingChanged();

    void listChanged();

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

    QTimer m_sortTimer;
    QTimer m_loadingTimer;

    std::shared_ptr<Client> m_client;

    Locale *m_locale{};
    StorageManager *m_storageManager{};

    std::unique_ptr<ChatList> m_list;

    std::vector<std::weak_ptr<Chat>> m_chats;
};
