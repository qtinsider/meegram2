#pragma once

#include "Stores.hpp"

#include <functional>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

class TdApi : public QObject
{
    Q_OBJECT
    Q_ENUMS(AuthorizationState ChatList)
    Q_PROPERTY(TdApi::AuthorizationState authorizationState READ getAuthorizationState NOTIFY authorizationStateChanged)

    std::vector<std::unique_ptr<Store>> stores;

public:
    ~TdApi() = default;

    TdApi(const TdApi &) = delete;
    TdApi &operator=(const TdApi &) = delete;

    static TdApi &getInstance();

    enum AuthorizationState {
        AuthorizationStateWaitTdlibParameters,
        AuthorizationStateWaitEncryptionKey,
        AuthorizationStateWaitPhoneNumber,
        AuthorizationStateWaitCode,
        AuthorizationStateWaitOtherDeviceConfirmation,
        AuthorizationStateWaitRegistration,
        AuthorizationStateWaitPassword,
        AuthorizationStateReady,
        AuthorizationStateLoggingOut,
        AuthorizationStateClosing,
        AuthorizationStateClosed,
    };

    enum ChatList {
        ChatListMain,
        ChatListArchive,
        ChatListFilter,
    };

    Q_INVOKABLE void initialize();

    Q_INVOKABLE void sendRequest(const QVariantMap &js);
    Q_INVOKABLE void log(const QVariantMap &js) noexcept;

    [[nodiscard]] AuthorizationState getAuthorizationState() const noexcept;

    Q_INVOKABLE void checkCode(const QString &code) noexcept;
    Q_INVOKABLE void checkPassword(const QString &password) noexcept;
    Q_INVOKABLE void logOut() noexcept;
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName) noexcept;
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber) noexcept;
    Q_INVOKABLE void resendCode() noexcept;
    Q_INVOKABLE void deleteAccount(const QString &reason) noexcept;

    Q_INVOKABLE void close() noexcept;

    Q_INVOKABLE void closeChat(qint64 chatId);
    Q_INVOKABLE void closeSecretChat(qint32 secretChatId);
    Q_INVOKABLE void createBasicGroupChat(qint32 basicGroupId, bool force);
    Q_INVOKABLE void createNewBasicGroupChat(const QList<qint32> &userIds, const QString &title);
    Q_INVOKABLE void createNewSecretChat(qint32 userId);
    Q_INVOKABLE void createNewSupergroupChat(const QString &title, bool isChannel, const QString &description, const QVariantMap &location);
    Q_INVOKABLE void createPrivateChat(qint32 userId, bool force);
    Q_INVOKABLE void createSecretChat(qint32 secretChatId);
    Q_INVOKABLE void createSupergroupChat(qint32 supergroupId, bool force);
    Q_INVOKABLE void deleteChatHistory(qint64 chatId, bool removeFromChatList, bool revoke);
    Q_INVOKABLE void deleteMessages(qint64 chatId, const QList<qint64> &messageIds, bool revoke);
    Q_INVOKABLE void downloadFile(qint32 fileId, qint32 priority, qint32 offset, qint32 limit, bool synchronous);
    Q_INVOKABLE void editMessageText(qint64 chatId, qint64 messageId, const QVariantMap &replyMarkup,
                                     const QVariantMap &inputMessageContent);
    Q_INVOKABLE void getChat(qint64 chatId);
    Q_INVOKABLE void getChats(const QVariantMap &chatList, qint64 offsetOrder, qint64 offsetChatId, qint32 limit);
    Q_INVOKABLE void getChatFilter(qint32 chatFilterId);
    Q_INVOKABLE void getChatHistory(qint64 chatId, qint64 fromMessageId, qint32 offset, qint32 limit, bool onlyLocal);
    Q_INVOKABLE void getMe();
    Q_INVOKABLE void getMessage(qint64 chatId, qint64 messageId);
    Q_INVOKABLE void getMessages(qint64 chatId, const QList<qint64> &messageIds);
    Q_INVOKABLE void joinChat(qint64 chatId);
    Q_INVOKABLE void leaveChat(qint64 chatId);
    Q_INVOKABLE void openChat(qint64 chatId);
    Q_INVOKABLE void sendChatAction(qint64 chatId, qint64 messageThreadId, const QVariantMap &action);
    Q_INVOKABLE void sendMessage(qint64 chatId, qint64 messageThreadId, qint64 replyToMessageId, const QVariantMap &options,
                                 const QVariantMap &replyMarkup, const QVariantMap &inputMessageContent);
    Q_INVOKABLE void setChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings);
    Q_INVOKABLE void setLogVerbosityLevel(qint32 newVerbosityLevel);
    Q_INVOKABLE void setOption(const QString &name, const QVariant &value);
    Q_INVOKABLE void toggleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread);
    Q_INVOKABLE void toggleChatIsPinned(const QVariantMap &chatList, qint64 chatId, bool isPinned);
    Q_INVOKABLE void viewMessages(qint64 chatId, qint64 messageThreadId, const QList<qint64> &messageIds, bool forceRead);

    BasicGroupStore *const basicGroupStore{};
    ChatStore *const chatStore{};
    FileStore *const fileStore{};
    OptionStore *const optionStore{};
    SupergroupStore *const supergroupStore{};
    UserStore *const userStore{};

public slots:
    void listen();

signals:
    void authorizationStateReady();
    void authorizationStateChanged(TdApi::AuthorizationState authorizationState);

    void updateAuthorizationState(const QVariantMap &authorizationState);
    void updateConnectionState(const QVariantMap &connectionState);

    void updateNewMessage(const QVariantMap &message);
    void updateMessageSendAcknowledged(qint64 chatId, qint64 messageId);
    void updateMessageSendSucceeded(const QVariantMap &message, qint64 oldMessageId);
    void updateMessageSendFailed(const QVariantMap &message, qint64 oldMessageId, int errorCode, const QString &errorMessage);
    void updateMessageContent(qint64 chatId, qint64 messageId, const QVariantMap &newContent);
    void updateMessageEdited(qint64 chatId, qint64 messageId, int editDate, const QVariantMap &replyMarkup);
    void updateMessageIsPinned(qint64 chatId, qint64 messageId, bool isPinned);
    void updateMessageInteractionInfo(qint64 chatId, qint64 messageId, const QVariantMap &interactionInfo);
    void updateMessageContentOpened(qint64 chatId, qint64 messageId);
    void updateMessageMentionRead(qint64 chatId, qint64 messageId, int unreadMentionCount);
    void updateMessageLiveLocationViewed(qint64 chatId, qint64 messageId);

    void updateDeleteMessages(qint64 chatId, const QVariantList &messageIds, bool isPermanent, bool fromCache);
    void updateUnreadMessageCount(const QVariantMap &chatList, int unreadCount, int unreadUnmutedCount);
    void updateUnreadChatCount(const QVariantMap &chatList, int totalCount, int unreadCount, int unreadUnmutedCount,
                               int markedAsUnreadCount, int markedAsUnreadUnmutedCount);
    void updateActiveNotifications(const QVariantList &groups);
    void updateNotificationGroup(int notificationGroupId, const QVariantMap &type, qint64 chatId, qint64 notificationSettingsChatId,
                                 bool isSilent, int totalCount, const QVariantList &addedNotifications,
                                 const QVariantList &removedNotificationIds);

    void updateNewChat(const QVariantMap &chat);
    void updateChatTitle(qint64 chatId, const QString &title);
    void updateChatPhoto(qint64 chatId, const QVariantMap &chatPhotoInfo);
    void updateChatPermissions(qint64 chatId, const QVariantMap &permissions);
    void updateChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions);
    void updateChatPosition(qint64 chatId, const QVariantMap &position);
    void updateChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread);
    void updateChatIsBlocked(qint64 chatId, bool isBlocked);
    void updateChatHasScheduledMessages(qint64 chatId, bool hasScheduledMessages);
    void updateChatDefaultDisableNotification(qint64 chatId, bool defaultDisableNotification);
    void updateChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount);
    void updateChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId);
    void updateChatUnreadMentionCount(qint64 chatId, int unreadMentionCount);
    void updateChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings);
    void updateScopeNotificationSettings(const QVariantMap &scope, const QVariantMap &notificationSettings);
    void updateChatActionBar(qint64 chatId, const QVariantMap &actionBar);
    void updateChatReplyMarkup(qint64 chatId, qint64 replyMarkupMessageId);
    void updateChatDraftMessage(qint64 chatId, const QVariantMap &draftMessage, const QVariantList &positions);
    void updateChatFilters(const QVariantList &chatFilters);
    void updateChatOnlineMemberCount(qint64 chatId, int onlineMemberCount);

    void updateOption(const QString &name, const QVariantMap &value);

    void updateUserChatAction(qint64 chatId, qint64 messageThreadId, int userId, const QVariantMap &action);
    void updateUserStatus(int userId, const QVariantMap &status);
    void updateUser(const QVariantMap &user);
    void updateBasicGroup(const QVariantMap &basicGroup);
    void updateSupergroup(const QVariantMap &supergroup);
    void updateUserFullInfo(int userId, const QVariantMap &userFullInfo);
    void updateBasicGroupFullInfo(int basicGroupId, const QVariantMap &basicGroupFullInfo);
    void updateSupergroupFullInfo(int supergroupId, const QVariantMap &supergroupFullInfo);

    void updateFile(const QVariantMap &file);

    // Returns ...
    void basicGroupFullInfo(const QVariantMap &data);
    void chat(const QVariantMap &data);
    void chatMembers(const QVariantMap &data);
    void chats(const QVariantMap &data);
    void error(const QVariantMap &data);
    void file(const QVariantMap &data);
    void message(const QVariantMap &data);
    void messages(const QVariantMap &data);
    void stickers(const QVariantMap &data);
    void stickerSet(const QVariantMap &data);
    void stickerSets(const QVariantMap &data);
    void supergroupFullInfo(const QVariantMap &data);
    void userFullInfo(const QVariantMap &data);
    void userProfilePhotos(const QVariantMap &data);
    void users(const QVariantMap &data);

private:
    TdApi();

    void initEvents();
    void handleAuthorizationState(const QVariantMap &data);

    template <typename T>
    requires std::is_base_of_v<Store, T> T &emplace()
    {
        auto t = new T;
        stores.emplace_back(std::unique_ptr<T>(t));
        return *t;
    }

    int clientId{};

    std::jthread m_worker;

    mutable std::shared_mutex mutex;

    AuthorizationState m_state{AuthorizationStateClosed};

    std::unordered_map<std::string, std::function<void(const QVariantMap &)>> m_events;
};
