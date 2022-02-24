#pragma once

#include <QVariant>

#include <functional>
#include <thread>
#include <unordered_map>

class TdApi : public QObject
{
    Q_OBJECT
    Q_ENUMS(ChatList)
    Q_PROPERTY(bool isAuthorized READ isAuthorized NOTIFY isAuthorizedChanged)

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

    void sendRequest(const QVariantMap &object, std::function<void(const QVariantMap &)> callback = {});

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
    Q_INVOKABLE void getMessage(qint64 chatId, qint64 messageId);
    Q_INVOKABLE void getMessages(qint64 chatId, const QVariantList &messageIds);
    Q_INVOKABLE void joinChat(qint64 chatId);
    Q_INVOKABLE void leaveChat(qint64 chatId);

    Q_INVOKABLE void sendChatAction(qint64 chatId, qint64 messageThreadId, const QVariantMap &action);
    Q_INVOKABLE void setLogVerbosityLevel(qint32 newVerbosityLevel);
    Q_INVOKABLE void setOption(const QString &name, const QVariant &value);
    Q_INVOKABLE void toggleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread);

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
    void accountTtl(const QVariantMap &data);
    void addedReactions(const QVariantMap &data);
    void animations(const QVariantMap &data);
    void animatedEmoji(const QVariantMap &data);
    void authorizationState(const QVariantMap &data);
    void autoDownloadSettingsPresets(const QVariantMap &data);
    void authenticationCodeInfo(const QVariantMap &data);
    void availableReactions(const QVariantMap &data);
    void background(const QVariantMap &data);
    void backgrounds(const QVariantMap &data);
    void bankCardInfo(const QVariantMap &data);
    void basicGroup(const QVariantMap &data);
    void basicGroupFullInfo(const QVariantMap &data);
    void botCommands(const QVariantMap &data);
    void callbackQueryAnswer(const QVariantMap &data);
    void callId(const QVariantMap &data);
    void canTransferOwnershipResult(const QVariantMap &data);
    void chat(const QVariantMap &data);
    void chatAdministrators(const QVariantMap &data);
    void chatFilter(const QVariantMap &data);
    void chatFilterInfo(const QVariantMap &data);
    void chatEvents(const QVariantMap &data);
    void chatInviteLinkInfo(const QVariantMap &data);
    void chatInviteLink(const QVariantMap &data);
    void chatPhotos(const QVariantMap &data);
    void chatMembers(const QVariantMap &data);
    void chatsNearby(const QVariantMap &data);
    void chatInviteLinkCounts(const QVariantMap &data);
    void chatInviteLinkMembers(const QVariantMap &data);
    void chatJoinRequests(const QVariantMap &data);
    void chatInviteLinks(const QVariantMap &data);
    void chatLists(const QVariantMap &data);
    void chatMember(const QVariantMap &data);
    void chats(const QVariantMap &data);
    void chatStatistics(const QVariantMap &data);
    void checkChatUsernameResult(const QVariantMap &data);
    void checkStickerSetNameResult(const QVariantMap &data);
    void count(const QVariantMap &data);
    void connectedWebsites(const QVariantMap &data);
    void countries(const QVariantMap &data);
    void customRequestResult(const QVariantMap &data);
    void databaseStatistics(const QVariantMap &data);
    void deepLinkInfo(const QVariantMap &data);
    void error(const QVariantMap &data);
    void emailAddressAuthenticationCodeInfo(const QVariantMap &data);
    void emojis(const QVariantMap &data);
    void filePart(const QVariantMap &data);
    void file(const QVariantMap &data);
    void formattedText(const QVariantMap &data);
    void foundMessages(const QVariantMap &data);
    void gameHighScores(const QVariantMap &data);
    void groupCall(const QVariantMap &data);
    void groupCallId(const QVariantMap &data);
    void hashtags(const QVariantMap &data);
    void httpUrl(const QVariantMap &data);
    void importedContacts(const QVariantMap &data);
    void inlineQueryResults(const QVariantMap &data);
    void internalLinkType(const QVariantMap &data);
    void jsonValue(const QVariantMap &data);
    void languagePackInfo(const QVariantMap &data);
    void languagePackStringValue(const QVariantMap &data);
    void languagePackStrings(const QVariantMap &data);
    void localizationTargetInfo(const QVariantMap &data);
    void logStream(const QVariantMap &data);
    void logVerbosityLevel(const QVariantMap &data);
    void logTags(const QVariantMap &data);
    void loginUrlInfo(const QVariantMap &data);
    void message(const QVariantMap &data);
    void messageFileType(const QVariantMap &data);
    void messageCalendar(const QVariantMap &data);
    void messagePositions(const QVariantMap &data);
    void messageLink(const QVariantMap &data);
    void messageLinkInfo(const QVariantMap &data);
    void messageStatistics(const QVariantMap &data);
    void messageThreadInfo(const QVariantMap &data);
    void messages(const QVariantMap &data);
    void messageSenders(const QVariantMap &data);
    void networkStatistics(const QVariantMap &data);
    void ok(const QVariantMap &data);
    void optionValue(const QVariantMap &data);
    void orderInfo(const QVariantMap &data);
    void passportElements(const QVariantMap &data);
    void paymentForm(const QVariantMap &data);
    void paymentReceipt(const QVariantMap &data);
    void phoneNumberInfo(const QVariantMap &data);
    void proxies(const QVariantMap &data);
    void pushReceiverId(const QVariantMap &data);
    void paymentResult(const QVariantMap &data);
    void proxy(const QVariantMap &data);
    void passportAuthorizationForm(const QVariantMap &data);
    void passportElementsWithErrors(const QVariantMap &data);
    void passportElement(const QVariantMap &data);
    void passwordState(const QVariantMap &data);
    void recommendedChatFilters(const QVariantMap &data);
    void recoveryEmailAddress(const QVariantMap &data);
    void resetPasswordResult(const QVariantMap &data);
    void seconds(const QVariantMap &data);
    void scopeNotificationSettings(const QVariantMap &data);
    void secretChat(const QVariantMap &data);
    void session(const QVariantMap &data);
    void sessions(const QVariantMap &data);
    void sticker(const QVariantMap &data);
    void stickers(const QVariantMap &data);
    void stickerSet(const QVariantMap &data);
    void stickerSets(const QVariantMap &data);
    void storageStatistics(const QVariantMap &data);
    void storageStatisticsFast(const QVariantMap &data);
    void sponsoredMessage(const QVariantMap &data);
    void statisticalGraph(const QVariantMap &data);
    void supergroup(const QVariantMap &data);
    void supergroupFullInfo(const QVariantMap &data);
    void temporaryPasswordState(const QVariantMap &data);
    void text(const QVariantMap &data);
    void textEntities(const QVariantMap &data);
    void testBytes(const QVariantMap &data);
    void testString(const QVariantMap &data);
    void testVectorInt(const QVariantMap &data);
    void testVectorString(const QVariantMap &data);
    void testVectorStringObject(const QVariantMap &data);
    void testInt(const QVariantMap &data);
    void tMeUrls(const QVariantMap &data);
    void update(const QVariantMap &data);
    void updates(const QVariantMap &data);
    void user(const QVariantMap &data);
    void users(const QVariantMap &data);
    void userFullInfo(const QVariantMap &data);
    void userPrivacySettingRules(const QVariantMap &data);
    void validatedOrderInfo(const QVariantMap &data);
    void webPageInstantView(const QVariantMap &data);
    void webPage(const QVariantMap &data);

private slots:
    void handleAuthorizationState(const QVariantMap &data);

private:
    TdApi();

    void initEvents();

    int clientId{};

    std::jthread m_worker;

    bool m_isAuthorized{false};

    std::unordered_map<std::string, std::function<void(const QVariantMap &)>> m_events;
    std::unordered_map<std::uint64_t, std::function<void(const QVariantMap &)>> m_handlers;
};
