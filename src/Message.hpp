#pragma once

#include <QObject>
#include <QVariant>

class Message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qint64 id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QVariantMap senderId READ senderId WRITE setSenderId NOTIFY senderIdChanged)
    Q_PROPERTY(qint64 chatId READ chatId WRITE setChatId NOTIFY chatIdChanged)
    Q_PROPERTY(QVariantMap sendingState READ sendingState WRITE setSendingState NOTIFY sendingStateChanged)
    Q_PROPERTY(QVariantMap schedulingState READ schedulingState WRITE setSchedulingState NOTIFY schedulingStateChanged)
    Q_PROPERTY(bool isOutgoing READ isOutgoing WRITE setIsOutgoing NOTIFY isOutgoingChanged)
    Q_PROPERTY(bool isPinned READ isPinned WRITE setIsPinned NOTIFY isPinnedChanged)
    Q_PROPERTY(bool isFromOffline READ isFromOffline WRITE setIsFromOffline NOTIFY isFromOfflineChanged)
    Q_PROPERTY(bool canBeEdited READ canBeEdited WRITE setCanBeEdited NOTIFY canBeEditedChanged)
    Q_PROPERTY(bool canBeForwarded READ canBeForwarded WRITE setCanBeForwarded NOTIFY canBeForwardedChanged)
    Q_PROPERTY(bool canBeRepliedInAnotherChat READ canBeRepliedInAnotherChat WRITE setCanBeRepliedInAnotherChat NOTIFY
                   canBeRepliedInAnotherChatChanged)
    Q_PROPERTY(bool canBeSaved READ canBeSaved WRITE setCanBeSaved NOTIFY canBeSavedChanged)
    Q_PROPERTY(
        bool canBeDeletedOnlyForSelf READ canBeDeletedOnlyForSelf WRITE setCanBeDeletedOnlyForSelf NOTIFY canBeDeletedOnlyForSelfChanged)
    Q_PROPERTY(
        bool canBeDeletedForAllUsers READ canBeDeletedForAllUsers WRITE setCanBeDeletedForAllUsers NOTIFY canBeDeletedForAllUsersChanged)
    Q_PROPERTY(bool canGetAddedReactions READ canGetAddedReactions WRITE setCanGetAddedReactions NOTIFY canGetAddedReactionsChanged)
    Q_PROPERTY(bool canGetStatistics READ canGetStatistics WRITE setCanGetStatistics NOTIFY canGetStatisticsChanged)
    Q_PROPERTY(bool canGetMessageThread READ canGetMessageThread WRITE setCanGetMessageThread NOTIFY canGetMessageThreadChanged)
    Q_PROPERTY(bool canGetReadDate READ canGetReadDate WRITE setCanGetReadDate NOTIFY canGetReadDateChanged)
    Q_PROPERTY(bool canGetViewers READ canGetViewers WRITE setCanGetViewers NOTIFY canGetViewersChanged)
    Q_PROPERTY(bool canGetMediaTimestampLinks READ canGetMediaTimestampLinks WRITE setCanGetMediaTimestampLinks NOTIFY
                   canGetMediaTimestampLinksChanged)
    Q_PROPERTY(bool canReportReactions READ canReportReactions WRITE setCanReportReactions NOTIFY canReportReactionsChanged)
    Q_PROPERTY(bool hasTimestampedMedia READ hasTimestampedMedia WRITE setHasTimestampedMedia NOTIFY hasTimestampedMediaChanged)
    Q_PROPERTY(bool isChannelPost READ isChannelPost WRITE setIsChannelPost NOTIFY isChannelPostChanged)
    Q_PROPERTY(bool isTopicMessage READ isTopicMessage WRITE setIsTopicMessage NOTIFY isTopicMessageChanged)
    Q_PROPERTY(bool containsUnreadMention READ containsUnreadMention WRITE setContainsUnreadMention NOTIFY containsUnreadMentionChanged)
    Q_PROPERTY(qint64 date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(qint64 editDate READ editDate WRITE setEditDate NOTIFY editDateChanged)
    Q_PROPERTY(QVariantMap forwardInfo READ forwardInfo WRITE setForwardInfo NOTIFY forwardInfoChanged)
    Q_PROPERTY(QVariantMap importInfo READ importInfo WRITE setImportInfo NOTIFY importInfoChanged)
    Q_PROPERTY(QVariantMap interactionInfo READ interactionInfo WRITE setInteractionInfo NOTIFY interactionInfoChanged)
    Q_PROPERTY(QVariantList unreadReactions READ unreadReactions WRITE setUnreadReactions NOTIFY unreadReactionsChanged)
    Q_PROPERTY(QVariantMap factCheck READ factCheck WRITE setFactCheck NOTIFY factCheckChanged)
    Q_PROPERTY(QVariantMap replyTo READ replyTo WRITE setReplyTo NOTIFY replyToChanged)
    Q_PROPERTY(qint64 messageThreadId READ messageThreadId WRITE setMessageThreadId NOTIFY messageThreadIdChanged)
    Q_PROPERTY(qint64 savedMessagesTopicId READ savedMessagesTopicId WRITE setSavedMessagesTopicId NOTIFY savedMessagesTopicIdChanged)
    Q_PROPERTY(QVariantMap selfDestructType READ selfDestructType WRITE setSelfDestructType NOTIFY selfDestructTypeChanged)
    Q_PROPERTY(double selfDestructIn READ selfDestructIn WRITE setSelfDestructIn NOTIFY selfDestructInChanged)
    Q_PROPERTY(double autoDeleteIn READ autoDeleteIn WRITE setAutoDeleteIn NOTIFY autoDeleteInChanged)
    Q_PROPERTY(qint64 viaBotUserId READ viaBotUserId WRITE setViaBotUserId NOTIFY viaBotUserIdChanged)
    Q_PROPERTY(
        qint64 senderBusinessBotUserId READ senderBusinessBotUserId WRITE setSenderBusinessBotUserId NOTIFY senderBusinessBotUserIdChanged)
    Q_PROPERTY(int senderBoostCount READ senderBoostCount WRITE setSenderBoostCount NOTIFY senderBoostCountChanged)
    Q_PROPERTY(QString authorSignature READ authorSignature WRITE setAuthorSignature NOTIFY authorSignatureChanged)
    Q_PROPERTY(qint64 mediaAlbumId READ mediaAlbumId WRITE setMediaAlbumId NOTIFY mediaAlbumIdChanged)
    Q_PROPERTY(qint64 effectId READ effectId WRITE setEffectId NOTIFY effectIdChanged)
    Q_PROPERTY(QString restrictionReason READ restrictionReason WRITE setRestrictionReason NOTIFY restrictionReasonChanged)
    Q_PROPERTY(QVariantMap content READ content WRITE setContent NOTIFY contentChanged)
    Q_PROPERTY(QVariantMap replyMarkup READ replyMarkup WRITE setReplyMarkup NOTIFY replyMarkupChanged)

public:
    explicit Message(QObject *parent = nullptr);

    qint64 id() const;
    void setId(qint64 id);

    QVariantMap senderId() const;
    void setSenderId(const QVariantMap &senderId);

    qint64 chatId() const;
    void setChatId(qint64 chatId);

    QVariantMap sendingState() const;
    void setSendingState(const QVariantMap &sendingState);

    QVariantMap schedulingState() const;
    void setSchedulingState(const QVariantMap &schedulingState);

    bool isOutgoing() const;
    void setIsOutgoing(bool isOutgoing);

    bool isPinned() const;
    void setIsPinned(bool isPinned);

    bool isFromOffline() const;
    void setIsFromOffline(bool isFromOffline);

    bool canBeEdited() const;
    void setCanBeEdited(bool canBeEdited);

    bool canBeForwarded() const;
    void setCanBeForwarded(bool canBeForwarded);

    bool canBeRepliedInAnotherChat() const;
    void setCanBeRepliedInAnotherChat(bool canBeRepliedInAnotherChat);

    bool canBeSaved() const;
    void setCanBeSaved(bool canBeSaved);

    bool canBeDeletedOnlyForSelf() const;
    void setCanBeDeletedOnlyForSelf(bool canBeDeletedOnlyForSelf);

    bool canBeDeletedForAllUsers() const;
    void setCanBeDeletedForAllUsers(bool canBeDeletedForAllUsers);

    bool canGetAddedReactions() const;
    void setCanGetAddedReactions(bool canGetAddedReactions);

    bool canGetStatistics() const;
    void setCanGetStatistics(bool canGetStatistics);

    bool canGetMessageThread() const;
    void setCanGetMessageThread(bool canGetMessageThread);

    bool canGetReadDate() const;
    void setCanGetReadDate(bool canGetReadDate);

    bool canGetViewers() const;
    void setCanGetViewers(bool canGetViewers);

    bool canGetMediaTimestampLinks() const;
    void setCanGetMediaTimestampLinks(bool canGetMediaTimestampLinks);

    bool canReportReactions() const;
    void setCanReportReactions(bool canReportReactions);

    bool hasTimestampedMedia() const;
    void setHasTimestampedMedia(bool hasTimestampedMedia);

    bool isChannelPost() const;
    void setIsChannelPost(bool isChannelPost);

    bool isTopicMessage() const;
    void setIsTopicMessage(bool isTopicMessage);

    bool containsUnreadMention() const;
    void setContainsUnreadMention(bool containsUnreadMention);

    qint64 date() const;
    void setDate(qint64 date);

    int editDate() const;
    void setEditDate(int editDate);

    QVariantMap forwardInfo() const;
    void setForwardInfo(const QVariantMap &forwardInfo);

    QVariantMap importInfo() const;
    void setImportInfo(const QVariantMap &importInfo);

    QVariantMap interactionInfo() const;
    void setInteractionInfo(const QVariantMap &interactionInfo);

    QVariantList unreadReactions() const;
    void setUnreadReactions(const QVariantList &unreadReactions);

    QVariantMap factCheck() const;
    void setFactCheck(const QVariantMap &factCheck);

    QVariantMap replyTo() const;
    void setReplyTo(const QVariantMap &replyTo);

    qint64 messageThreadId() const;
    void setMessageThreadId(qint64 messageThreadId);

    qint64 savedMessagesTopicId() const;
    void setSavedMessagesTopicId(qint64 savedMessagesTopicId);

    QVariantMap selfDestructType() const;
    void setSelfDestructType(const QVariantMap &selfDestructType);

    double selfDestructIn() const;
    void setSelfDestructIn(double selfDestructIn);

    double autoDeleteIn() const;
    void setAutoDeleteIn(double autoDeleteIn);

    qint64 viaBotUserId() const;
    void setViaBotUserId(qint64 viaBotUserId);

    qint64 senderBusinessBotUserId() const;
    void setSenderBusinessBotUserId(qint64 senderBusinessBotUserId);

    int senderBoostCount() const;
    void setSenderBoostCount(int senderBoostCount);

    QString authorSignature() const;
    void setAuthorSignature(const QString &authorSignature);

    qint64 mediaAlbumId() const;
    void setMediaAlbumId(qint64 mediaAlbumId);

    qint64 effectId() const;
    void setEffectId(qint64 effectId);

    QString restrictionReason() const;
    void setRestrictionReason(const QString &restrictionReason);

    QVariantMap content() const;
    void setContent(const QVariantMap &content);

    QVariantMap replyMarkup() const;
    void setReplyMarkup(const QVariantMap &replyMarkup);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void idChanged();
    void senderIdChanged();
    void chatIdChanged();
    void sendingStateChanged();
    void schedulingStateChanged();
    void isOutgoingChanged();
    void isPinnedChanged();
    void isFromOfflineChanged();
    void canBeEditedChanged();
    void canBeForwardedChanged();
    void canBeRepliedInAnotherChatChanged();
    void canBeSavedChanged();
    void canBeDeletedOnlyForSelfChanged();
    void canBeDeletedForAllUsersChanged();
    void canGetAddedReactionsChanged();
    void canGetStatisticsChanged();
    void canGetMessageThreadChanged();
    void canGetReadDateChanged();
    void canGetViewersChanged();
    void canGetMediaTimestampLinksChanged();
    void canReportReactionsChanged();
    void hasTimestampedMediaChanged();
    void isChannelPostChanged();
    void isTopicMessageChanged();
    void containsUnreadMentionChanged();
    void dateChanged();
    void editDateChanged();
    void forwardInfoChanged();
    void importInfoChanged();
    void interactionInfoChanged();
    void unreadReactionsChanged();
    void factCheckChanged();
    void replyToChanged();
    void messageThreadIdChanged();
    void savedMessagesTopicIdChanged();
    void selfDestructTypeChanged();
    void selfDestructInChanged();
    void autoDeleteInChanged();
    void viaBotUserIdChanged();
    void senderBusinessBotUserIdChanged();
    void senderBoostCountChanged();
    void authorSignatureChanged();
    void mediaAlbumIdChanged();
    void effectIdChanged();
    void restrictionReasonChanged();
    void contentChanged();
    void replyMarkupChanged();

private:
    qint64 m_id;
    QVariantMap m_senderId;
    qint64 m_chatId;
    QVariantMap m_sendingState;
    QVariantMap m_schedulingState;
    bool m_isOutgoing;
    bool m_isPinned;
    bool m_isFromOffline;
    bool m_canBeEdited;
    bool m_canBeForwarded;
    bool m_canBeRepliedInAnotherChat;
    bool m_canBeSaved;
    bool m_canBeDeletedOnlyForSelf;
    bool m_canBeDeletedForAllUsers;
    bool m_canGetAddedReactions;
    bool m_canGetStatistics;
    bool m_canGetMessageThread;
    bool m_canGetReadDate;
    bool m_canGetViewers;
    bool m_canGetMediaTimestampLinks;
    bool m_canReportReactions;
    bool m_hasTimestampedMedia;
    bool m_isChannelPost;
    bool m_isTopicMessage;
    bool m_containsUnreadMention;
    qint64 m_date;
    qint64 m_editDate;
    QVariantMap m_forwardInfo;
    QVariantMap m_importInfo;
    QVariantMap m_interactionInfo;
    QVariantList m_unreadReactions;
    QVariantMap m_factCheck;
    QVariantMap m_replyTo;
    qint64 m_messageThreadId;
    qint64 m_savedMessagesTopicId;
    QVariantMap m_selfDestructType;
    double m_selfDestructIn;
    double m_autoDeleteIn;
    qint64 m_viaBotUserId;
    qint64 m_senderBusinessBotUserId;
    int m_senderBoostCount;
    QString m_authorSignature;
    qint64 m_mediaAlbumId;
    qint64 m_effectId;
    QString m_restrictionReason;
    QVariantMap m_content;
    QVariantMap m_replyMarkup;
};
