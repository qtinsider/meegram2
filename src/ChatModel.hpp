#pragma once

#include "TdApi.hpp"

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>
#include <QTimer>
#include <QVector>

class StorageManager;

class ChatModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(TdManager *manager READ manager WRITE setManager)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)

    Q_PROPERTY(TdApi::ChatList chatList READ chatList WRITE setChatList NOTIFY chatListChanged)
    Q_PROPERTY(int chatFilterId READ chatFilterId WRITE setChatFilterId NOTIFY chatListChanged)

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

    TdManager *manager() const;
    void setManager(TdManager *manager);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    bool canFetchMore(const QModelIndex &parent = QModelIndex()) const override;
    void fetchMore(const QModelIndex &parent = QModelIndex()) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const;

    int count() const;
    bool loading() const;

    TdApi::ChatList chatList() const;
    void setChatList(TdApi::ChatList value);

    int chatFilterId() const;
    void setChatFilterId(int value);

    Q_INVOKABLE QVariant get(int index) const noexcept;

    Q_INVOKABLE bool isPinned(int index) const noexcept;
    Q_INVOKABLE bool isMuted(int index) const noexcept;

    Q_INVOKABLE void toggleChatIsPinned(int index);
    Q_INVOKABLE void toggleChatNotificationSettings(int index);

protected:
    void classBegin() override;
    void componentComplete() override;

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

    void handleChatPhoto(const QVariantMap &file);

private:
    void clear();

    Client *m_client;
    Locale *m_locale;
    TdManager *m_manager;
    StorageManager *m_storageManager;

    bool m_loading{true};

    int m_count{};

    int m_chatFilterId{};
    TdApi::ChatList m_chatList{TdApi::ChatListMain};

    QTimer *m_sortTimer;
    QTimer *m_loadingTimer;

    QVector<qint64> m_chatIds;

    QVariantMap m_list;
};
