#pragma once

#include <QVariant>

#include <functional>
#include <thread>
#include <unordered_map>

class TdApi : public QObject
{
    Q_OBJECT
    Q_ENUMS(AuthorizationState ChatList)
    Q_PROPERTY(TdApi::AuthorizationState authorizationState READ getAuthorizationState NOTIFY authorizationStateChanged)

public:
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

    void sendRequest(const QVariantMap &request, std::function<void(const QVariantMap &)> callback = {});

    AuthorizationState getAuthorizationState() const noexcept;

    Q_INVOKABLE void checkCode(const QString &code) noexcept;
    Q_INVOKABLE void checkPassword(const QString &password) noexcept;
    Q_INVOKABLE void logOut() noexcept;
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName) noexcept;
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber) noexcept;
    Q_INVOKABLE void resendCode() noexcept;
    Q_INVOKABLE void deleteAccount(const QString &reason) noexcept;

    Q_INVOKABLE void close() noexcept;

    Q_INVOKABLE void downloadFile(qint32 fileId, qint32 priority, qint32 offset, qint32 limit, bool synchronous);

    Q_INVOKABLE void setLogVerbosityLevel(qint32 newVerbosityLevel);
    Q_INVOKABLE void setOption(const QString &name, const QVariant &value);

public slots:
    void listen();
    void initialParameters();

signals:
    void codeRequested(const QString &phoneNumber, const QVariantMap &type, const QVariantMap &nextType, int timeout);
    void passwordRequested(const QString &passwordHint, bool hasRecoveryEmailAddress, const QString &recoveryEmailAddressPattern);
    void registrationRequested(const QString &text, int minUserAge, bool showPopup);
    void ready();
    void loggingOut();

    void error(const QString &errorString);

    void authorizationStateChanged(AuthorizationState state);

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
    void updateNewShippingQuery(qint64 id, qint64 senderUserId, const QString &invoicePayload, const QVariantMap &shippingAddress);
    void updateNewPreCheckoutQuery(qint64 id, qint64 senderUserId, const QString &currency, qint64 totalAmount,
                                   const QByteArray &invoicePayload, const QString &shippingOptionId, const QVariantMap &orderInfo);
    void updateNewCustomEvent(const QString &event);
    void updateNewCustomQuery(qint64 id, const QString &data, int timeout);
    void updatePoll(const QVariantMap &poll);
    void updatePollAnswer(qint64 pollId, qint64 userId, const QVariantList &optionIds);
    void updateChatMember(qint64 chatId, qint64 actorUserId, int date, const QVariantMap &inviteLink, const QVariantMap &oldChatMember,
                          const QVariantMap &newChatMember);
    void updateNewChatJoinRequest(qint64 chatId, const QVariantMap &request, const QVariantMap &inviteLink);

private slots:
    void handleAuthorizationState(const QVariantMap &data);

private:
    TdApi();

    void initEvents();

    int clientId{};

    std::jthread m_worker;

    std::atomic_uint64_t m_requestId;

    AuthorizationState m_state{AuthorizationStateClosed};

    std::unordered_map<std::string, std::function<void(const QVariantMap &)>> m_events;
    std::unordered_map<std::uint64_t, std::function<void(const QVariantMap &)>> m_handlers;
};
