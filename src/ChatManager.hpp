#pragma once

#include "ChatFolderModel.hpp"
#include "ChatModel.hpp"
#include "MessageModel.hpp"

#include <QObject>

#include <memory>
#include <vector>

class Client;
class StorageManager;
class Chat;
class BasicGroup;
class StorageManager;
class Supergroup;
class User;

class ChatInfoFormatter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit ChatInfoFormatter(std::shared_ptr<Chat> chat, std::shared_ptr<Locale> locale, std::shared_ptr<StorageManager> storage);

    QString title() const noexcept;
    QString status() const noexcept;

signals:
    void statusChanged();

private slots:
    void handleBasicGroupUpdate(qlonglong groupId) noexcept;
    void handleSupergroupUpdate(qlonglong groupId) noexcept;
    void handleUserUpdate(qlonglong userId) noexcept;
    void handleChatOnlineMemberCount(qlonglong chatId, int onlineMemberCount) noexcept;

private:
    void initializeMembers() noexcept;
    void updateStatus() noexcept;

    QString formatStatus(int memberCount, const char *memberKey, const char *onlineKey) const noexcept;
    int getMemberCountWithFallback() const noexcept;
    bool isServiceNotification() const noexcept;
    QString formatUserStatus() const noexcept;
    QString formatOfflineStatus() const noexcept;

    int m_onlineMemberCount{};

    QString m_title, m_status;

    std::shared_ptr<Chat> m_chat;
    std::shared_ptr<Locale> m_locale;
    std::shared_ptr<StorageManager> m_storageManager;

    std::shared_ptr<User> m_user;
    std::shared_ptr<BasicGroup> m_basicGroup;
    std::shared_ptr<Supergroup> m_supergroup;
};

class ChatManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject *folderModel READ folderModel NOTIFY folderModelChanged)

    Q_PROPERTY(QObject *mainModel READ mainModel NOTIFY mainModelChanged)
    Q_PROPERTY(QObject *archivedModel READ archivedModel NOTIFY archivedModelChanged)
    Q_PROPERTY(QList<QObject *> folderModels READ folderModels NOTIFY folderModelsChanged)

    Q_PROPERTY(Chat *selectedChat READ selectedChat NOTIFY selectedChatChanged)

    Q_PROPERTY(QObject *chatInfo READ chatInfoFormatter NOTIFY selectedChatChanged)
    Q_PROPERTY(QObject *messageModel READ messageModel NOTIFY selectedChatChanged)

public:
    explicit ChatManager(std::shared_ptr<StorageManager> storageManager, std::shared_ptr<Locale> locale);

    QObject *folderModel() const noexcept;

    QObject *mainModel() const noexcept;
    QObject *archivedModel() const noexcept;
    QList<QObject *> folderModels() const noexcept;

    Chat *selectedChat() const noexcept;

    QObject *chatInfoFormatter() const noexcept;
    QObject *messageModel() const noexcept;

    Q_INVOKABLE void openChat(qlonglong chatId) noexcept;
    Q_INVOKABLE void closeChat(qlonglong chatId) noexcept;

signals:
    void selectedChatChanged();

    void folderModelChanged();

    void mainModelChanged();
    void archivedModelChanged();
    void folderModelsChanged();

private slots:
    void onChatFoldersUpdated() noexcept;

private:
    void updateFolderModels() noexcept;

    std::shared_ptr<Client> m_client;
    std::shared_ptr<Locale> m_locale;
    std::shared_ptr<StorageManager> m_storage;

    std::unique_ptr<ChatModel> m_mainModel;
    std::unique_ptr<ChatModel> m_archivedModel;
    std::vector<std::unique_ptr<ChatModel>> m_folderModels;

    std::unique_ptr<ChatFolderModel> m_folderModel;

    std::shared_ptr<Chat> m_selectedChat;

    std::unique_ptr<ChatInfoFormatter> m_infoFormatter;
    std::unique_ptr<MessageModel> m_messageModel;
};
