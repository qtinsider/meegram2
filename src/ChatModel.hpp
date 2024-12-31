#pragma once

#include "Chat.hpp"
#include "ChatPosition.hpp"

#include <QAbstractListModel>
#include <QTimer>

#include <memory>
#include <vector>

class Locale;
class StorageManager;

class ChatModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

public:
    explicit ChatModel(std::unique_ptr<ChatList> list, std::shared_ptr<Locale> locale, std::shared_ptr<StorageManager> storage);

    enum Roles {
        IdRole = Qt::UserRole + 1,
        TypeRole,
        TitleRole,
        PhotoRole,
        DateRole,
        LastMessageRole,
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

    Q_INVOKABLE void toggleChatIsPinned(qlonglong chatId, bool isPinned);

    Q_INVOKABLE ChatPosition *getChatPosition(Chat *chat) const;

signals:
    void countChanged();
    void loadingChanged();

public slots:
    void clear();
    void populate();
    void refresh();

private slots:
    void loadChats();
    void sortChats();

    void handleChatItem(qlonglong chatId);
    void handleChatPosition(qlonglong chatId);

private:
    bool m_loading{true};

    int m_count{};

    QTimer m_sortTimer;
    QTimer m_loadingTimer;

    std::unique_ptr<ChatList> m_list;

    std::shared_ptr<Locale> m_locale;
    std::shared_ptr<StorageManager> m_storageManager;

    std::vector<std::weak_ptr<Chat>> m_chats;
};
