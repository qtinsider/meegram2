#pragma once

#include "Stores.hpp"

#include <functional>
#include <thread>
#include <unordered_map>

class TdApi : public QObject
{
    Q_OBJECT
    Q_ENUMS(ChatList)
    Q_PROPERTY(bool isAuthorized READ isAuthorized NOTIFY isAuthorizedChanged)

    std::vector<std::unique_ptr<Store>> m_stores;

public:
    ~TdApi() = default;

    TdApi(const TdApi &) = delete;
    TdApi &operator=(const TdApi &) = delete;

    static TdApi &getInstance();

    enum ChatList {
        ChatListMain,
        ChatListArchive,
        ChatListFilter,
    };

    Q_INVOKABLE void sendRequest(const QVariantMap &object, const QString &extra = QString());
    Q_INVOKABLE void log(const QVariantMap &object) noexcept;

    bool isAuthorized() const noexcept;

    Q_INVOKABLE void checkCode(const QString &code) noexcept;
    Q_INVOKABLE void checkPassword(const QString &password) noexcept;
    Q_INVOKABLE void logOut() noexcept;
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName) noexcept;
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber) noexcept;
    Q_INVOKABLE void resendCode() noexcept;
    Q_INVOKABLE void deleteAccount(const QString &reason) noexcept;

    Q_INVOKABLE void close() noexcept;

    Q_INVOKABLE void closeSecretChat(qint32 secretChatId);
    Q_INVOKABLE void createBasicGroupChat(qint32 basicGroupId, bool force);
    Q_INVOKABLE void createNewBasicGroupChat(const QVariantList &userIds, const QString &title);
    Q_INVOKABLE void createNewSecretChat(qint32 userId);
    Q_INVOKABLE void createNewSupergroupChat(const QString &title, bool isChannel, const QString &description, const QVariantMap &location);
    Q_INVOKABLE void createPrivateChat(qint32 userId, bool force);
    Q_INVOKABLE void createSecretChat(qint32 secretChatId);
    Q_INVOKABLE void createSupergroupChat(qint32 supergroupId, bool force);
    Q_INVOKABLE void deleteChatHistory(qint64 chatId, bool removeFromChatList, bool revoke);
    Q_INVOKABLE void deleteMessages(qint64 chatId, const QVariantList &messageIds, bool revoke);
    Q_INVOKABLE void downloadFile(qint32 fileId, qint32 priority, qint32 offset, qint32 limit, bool synchronous);
    Q_INVOKABLE void editMessageText(qint64 chatId, qint64 messageId, const QVariantMap &replyMarkup,
                                     const QVariantMap &inputMessageContent);
    Q_INVOKABLE void getChatFilter(qint32 chatFilterId);
    Q_INVOKABLE void getMe();
    Q_INVOKABLE void getMessage(qint64 chatId, qint64 messageId);
    Q_INVOKABLE void getMessages(qint64 chatId, const QVariantList &messageIds);
    Q_INVOKABLE void joinChat(qint64 chatId);
    Q_INVOKABLE void leaveChat(qint64 chatId);

    Q_INVOKABLE void sendChatAction(qint64 chatId, qint64 messageThreadId, const QVariantMap &action);
    Q_INVOKABLE void setLogVerbosityLevel(qint32 newVerbosityLevel);
    Q_INVOKABLE void setOption(const QString &name, const QVariant &value);
    Q_INVOKABLE void toggleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread);

    BasicGroupStore *const basicGroupStore{};
    ChatStore *const chatStore{};
    FileStore *const fileStore{};
    OptionStore *const optionStore{};
    SupergroupStore *const supergroupStore{};
    UserStore *const userStore{};

    Q_INVOKABLE QVariantMap getBasicGroup(qint64 id) const;
    Q_INVOKABLE QVariantMap getBasicGroupFullInfo(qint64 id) const;
    Q_INVOKABLE QVariantMap getChat(qint64 id) const;
    Q_INVOKABLE QVariantMap getFile(qint32 id) const;
    Q_INVOKABLE QVariant getOption(const QString &name) const;
    Q_INVOKABLE QVariantMap getSupergroup(qint64 id) const;
    Q_INVOKABLE QVariantMap getSupergroupFullInfo(qint64 id) const;
    Q_INVOKABLE QVariantMap getUser(qint64 id) const;
    Q_INVOKABLE QVariantMap getUserFullInfo(qint64 id) const;

public slots:
    void listen();

signals:
    void codeRequested(const QVariant &codeInfo);
    void passwordRequested(const QVariant &passwordInfo);
    void registrationRequested(const QVariant &termsOfService);

    void isAuthorizedChanged();

    void updateAuthorizationState(const QVariantMap &authorizationState);
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
    void updateMessageUnreadReactions(qint64 chatId, qint64 messageId, const QVariantList &unreadReactions, int unreadReactionCount);
    void updateMessageLiveLocationViewed(qint64 chatId, qint64 messageId);
    void updateNewChat(const QVariantMap &chat);
    void updateChatTitle(qint64 chatId, const QString &title);
    void updateChatPhoto(qint64 chatId, const QVariantMap &photo);
    void updateChatPermissions(qint64 chatId, const QVariantMap &permissions);
    void updateChatLastMessage(qint64 chatId, const QVariantMap &lastMessage, const QVariantList &positions);
    void updateChatPosition(qint64 chatId, const QVariantMap &position);
    void updateChatReadInbox(qint64 chatId, qint64 lastReadInboxMessageId, int unreadCount);
    void updateChatReadOutbox(qint64 chatId, qint64 lastReadOutboxMessageId);
    void updateChatActionBar(qint64 chatId, const QVariantMap &actionBar);
    void updateChatAvailableReactions(qint64 chatId, const QStringList &availableReactions);
    void updateChatDraftMessage(qint64 chatId, const QVariantMap &draftMessage, const QVariantList &positions);
    void updateChatMessageSender(qint64 chatId, const QVariantMap &messageSenderId);
    void updateChatMessageTtl(qint64 chatId, int messageTtl);
    void updateChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings);
    void updateChatPendingJoinRequests(qint64 chatId, const QVariantMap &pendingJoinRequests);
    void updateChatReplyMarkup(qint64 chatId, qint64 replyMarkupMessageId);
    void updateChatTheme(qint64 chatId, const QString &themeName);
    void updateChatUnreadMentionCount(qint64 chatId, int unreadMentionCount);
    void updateChatUnreadReactionCount(qint64 chatId, int unreadReactionCount);
    void updateChatVideoChat(qint64 chatId, const QVariantMap &videoChat);
    void updateChatDefaultDisableNotification(qint64 chatId, bool defaultDisableNotification);
    void updateChatHasProtectedContent(qint64 chatId, bool hasProtectedContent);
    void updateChatHasScheduledMessages(qint64 chatId, bool hasScheduledMessages);
    void updateChatIsBlocked(qint64 chatId, bool isBlocked);
    void updateChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread);
    void updateChatFilters(const QVariantList &chatFilters);
    void updateChatOnlineMemberCount(qint64 chatId, int onlineMemberCount);
    void updateScopeNotificationSettings(const QVariantMap &scope, const QVariantMap &notificationSettings);
    void updateNotification(int notificationGroupId, const QVariantMap &notification);
    void updateNotificationGroup(int notificationGroupId, const QVariantMap &type, qint64 chatId, qint64 notificationSettingsChatId,
                                 bool isSilent, int totalCount, const QVariantList &addedNotifications,
                                 const QVariantList &removedNotificationIds);
    void updateActiveNotifications(const QVariantList &groups);
    void updateHavePendingNotifications(bool haveDelayedNotifications, bool haveUnreceivedNotifications);
    void updateDeleteMessages(qint64 chatId, const QVariantList &messageIds, bool isPermanent, bool fromCache);
    void updateChatAction(qint64 chatId, qint64 messageThreadId, const QVariantMap &senderId, const QVariantMap &action);
    void updateUserStatus(qint64 userId, const QVariantMap &status);
    void updateUser(const QVariantMap &user);
    void updateBasicGroup(const QVariantMap &basicGroup);
    void updateSupergroup(const QVariantMap &supergroup);
    void updateSecretChat(const QVariantMap &secretChat);
    void updateUserFullInfo(qint64 userId, const QVariantMap &userFullInfo);
    void updateBasicGroupFullInfo(qint64 basicGroupId, const QVariantMap &basicGroupFullInfo);
    void updateSupergroupFullInfo(qint64 supergroupId, const QVariantMap &supergroupFullInfo);
    void updateServiceNotification(const QString &type, const QVariantMap &content);
    void updateFile(const QVariantMap &file);
    void updateFileGenerationStart(qint64 generationId, const QString &originalPath, const QString &destinationPath,
                                   const QString &conversion);
    void updateFileGenerationStop(qint64 generationId);
    void updateCall(const QVariantMap &call);
    void updateGroupCall(const QVariantMap &groupCall);
    void updateGroupCallParticipant(int groupCallId, const QVariantMap &participant);
    void updateNewCallSignalingData(int callId, const QByteArray &data);
    void updateUserPrivacySettingRules(const QVariantMap &setting, const QVariantMap &rules);
    void updateUnreadMessageCount(const QVariantMap &chatList, int unreadCount, int unreadUnmutedCount);
    void updateUnreadChatCount(const QVariantMap &chatList, int totalCount, int unreadCount, int unreadUnmutedCount,
                               int markedAsUnreadCount, int markedAsUnreadUnmutedCount);
    void updateOption(const QString &name, const QVariantMap &value);
    void updateStickerSet(const QVariantMap &stickerSet);
    void updateInstalledStickerSets(bool isMasks, const QVariantList &stickerSetIds);
    void updateTrendingStickerSets(const QVariantMap &stickerSets);
    void updateRecentStickers(bool isAttached, const QVariantList &stickerIds);
    void updateFavoriteStickers(const QVariantList &stickerIds);
    void updateSavedAnimations(const QVariantList &animationIds);
    void updateSelectedBackground(bool forDarkTheme, const QVariantMap &background);
    void updateChatThemes(const QVariantList &chatThemes);
    void updateLanguagePackStrings(const QString &localizationTarget, const QString &languagePackId, const QVariantList &strings);
    void updateConnectionState(const QVariantMap &state);
    void updateTermsOfService(const QString &termsOfServiceId, const QVariantMap &termsOfService);
    void updateUsersNearby(const QVariantList &usersNearby);
    void updateReactions(const QVariantList &reactions);
    void updateDiceEmojis(const QStringList &emojis);
    void updateAnimatedEmojiMessageClicked(qint64 chatId, qint64 messageId, const QVariantMap &sticker);
    void updateAnimationSearchParameters(const QString &provider, const QStringList &emojis);
    void updateSuggestedActions(const QVariantList &addedActions, const QVariantList &removedActions);
    void updateNewInlineQuery(qint64 id, qint64 senderUserId, const QVariantMap &userLocation, const QVariantMap &chatType,
                              const QString &query, const QString &offset);
    void updateNewChosenInlineResult(qint64 senderUserId, const QVariantMap &userLocation, const QString &query, const QString &resultId,
                                     const QString &inlineMessageId);
    void updateNewCallbackQuery(qint64 id, qint64 senderUserId, qint64 chatId, qint64 messageId, qint64 chatInstance,
                                const QVariantMap &payload);
    void updateNewInlineCallbackQuery(qint64 id, qint64 senderUserId, const QString &inlineMessageId, qint64 chatInstance,
                                      const QVariantMap &payload);
    void updateNewShippingQuery(qint64 id, qint64 senderUserId, const QString &invoice_payload_, const QVariantMap &shipping_address_);
    void updateNewPreCheckoutQuery(qint64 id, qint64 senderUserId, const QString &currency, qint64 totalAmount,
                                   const QByteArray &invoicePayload, const QString &shippingOptionId, const QVariantMap &orderInfo);
    void updateNewCustomEvent(const QString &event);
    void updateNewCustomQuery(qint64 id, const QString &data, int timeout);
    void updatePoll(const QVariantMap &poll);
    void updatePollAnswer(qint64 pollId, qint64 userId, const QVariantList &optionIds);
    void updateChatMember(qint64 chatId, qint64 actorUserId, int date, const QVariantMap &inviteLink, const QVariantMap &oldChatMember,
                          const QVariantMap &newChatMember);
    void updateNewChatJoinRequest(qint64 chatId, const QVariantMap &request, const QVariantMap &inviteLink);

    // Returns ...
    void basicGroupFullInfo(const QVariantMap &data);
    void chat(const QVariantMap &data);
    void chatMembers(const QVariantMap &data);
    void chats(const QVariantMap &data);
    void countries(const QVariantMap &data);
    void error(const QVariantMap &data);
    void file(const QVariantMap &data);
    void message(const QVariantMap &data);
    void messages(const QVariantMap &data);
    void ok(const QVariantMap &data);
    void stickers(const QVariantMap &data);
    void stickerSet(const QVariantMap &data);
    void stickerSets(const QVariantMap &data);
    void supergroupFullInfo(const QVariantMap &data);
    void userFullInfo(const QVariantMap &data);
    void userProfilePhotos(const QVariantMap &data);
    void users(const QVariantMap &data);

private slots:
    void handleAuthorizationState(const QVariantMap &data);

private:
    TdApi();

    void initEvents();

    template <typename T>
    requires std::is_base_of_v<Store, T> T &emplace()
    {
        auto t = new T;
        m_stores.emplace_back(std::unique_ptr<T>(t));
        return *t;
    }

    int clientId{};

    std::jthread m_worker;

    bool m_isAuthorized{false};

    std::unordered_map<std::string, std::function<void(const QVariantMap &)>> m_events;
};
