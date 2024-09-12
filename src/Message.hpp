#pragma once

#include <td/telegram/td_api.h>

#include <QVariant>

class StorageManager;

class Message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap senderId READ senderId NOTIFY dataChanged)
    Q_PROPERTY(qlonglong chatId READ chatId NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap sendingState READ sendingState NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap schedulingState READ schedulingState NOTIFY dataChanged)
    Q_PROPERTY(bool isOutgoing READ isOutgoing NOTIFY dataChanged)
    Q_PROPERTY(bool isPinned READ isPinned NOTIFY dataChanged)
    Q_PROPERTY(bool isFromOffline READ isFromOffline NOTIFY dataChanged)
    Q_PROPERTY(bool canBeEdited READ canBeEdited NOTIFY dataChanged)
    Q_PROPERTY(bool canBeForwarded READ canBeForwarded NOTIFY dataChanged)
    Q_PROPERTY(bool canBeRepliedInAnotherChat READ canBeRepliedInAnotherChat NOTIFY dataChanged)
    Q_PROPERTY(bool canBeSaved READ canBeSaved NOTIFY dataChanged)
    Q_PROPERTY(bool canBeDeletedOnlyForSelf READ canBeDeletedOnlyForSelf NOTIFY dataChanged)
    Q_PROPERTY(bool canBeDeletedForAllUsers READ canBeDeletedForAllUsers NOTIFY dataChanged)
    Q_PROPERTY(bool canGetAddedReactions READ canGetAddedReactions NOTIFY dataChanged)
    Q_PROPERTY(bool canGetStatistics READ canGetStatistics NOTIFY dataChanged)
    Q_PROPERTY(bool canGetMessageThread READ canGetMessageThread NOTIFY dataChanged)
    Q_PROPERTY(bool canGetReadDate READ canGetReadDate NOTIFY dataChanged)
    Q_PROPERTY(bool canGetViewers READ canGetViewers NOTIFY dataChanged)
    Q_PROPERTY(bool canGetMediaTimestampLinks READ canGetMediaTimestampLinks NOTIFY dataChanged)
    Q_PROPERTY(bool canReportReactions READ canReportReactions NOTIFY dataChanged)
    Q_PROPERTY(bool hasTimestampedMedia READ hasTimestampedMedia NOTIFY dataChanged)
    Q_PROPERTY(bool isChannelPost READ isChannelPost NOTIFY dataChanged)
    Q_PROPERTY(bool isTopicMessage READ isTopicMessage NOTIFY dataChanged)
    Q_PROPERTY(bool containsUnreadMention READ containsUnreadMention NOTIFY dataChanged)
    Q_PROPERTY(qlonglong date READ date NOTIFY dataChanged)
    Q_PROPERTY(qlonglong editDate READ editDate NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap forwardInfo READ forwardInfo NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap importInfo READ importInfo NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap interactionInfo READ interactionInfo NOTIFY dataChanged)
    Q_PROPERTY(QVariantList unreadReactions READ unreadReactions NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap factCheck READ factCheck NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap replyTo READ replyTo NOTIFY dataChanged)
    Q_PROPERTY(qlonglong messageThreadId READ messageThreadId NOTIFY dataChanged)
    Q_PROPERTY(qlonglong savedMessagesTopicId READ savedMessagesTopicId NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap selfDestructType READ selfDestructType NOTIFY dataChanged)
    Q_PROPERTY(double selfDestructIn READ selfDestructIn NOTIFY dataChanged)
    Q_PROPERTY(double autoDeleteIn READ autoDeleteIn NOTIFY dataChanged)
    Q_PROPERTY(qlonglong viaBotUserId READ viaBotUserId NOTIFY dataChanged)
    Q_PROPERTY(qlonglong senderBusinessBotUserId READ senderBusinessBotUserId NOTIFY dataChanged)
    Q_PROPERTY(int senderBoostCount READ senderBoostCount NOTIFY dataChanged)
    Q_PROPERTY(QString authorSignature READ authorSignature NOTIFY dataChanged)
    Q_PROPERTY(qlonglong mediaAlbumId READ mediaAlbumId NOTIFY dataChanged)
    Q_PROPERTY(qlonglong effectId READ effectId NOTIFY dataChanged)
    Q_PROPERTY(QString restrictionReason READ restrictionReason NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap content READ content NOTIFY dataChanged)
    Q_PROPERTY(QVariantMap replyMarkup READ replyMarkup NOTIFY dataChanged)

public:
    explicit Message(QObject *parent = nullptr);
    explicit Message(td::td_api::message *message, QObject *parent = nullptr);

    qlonglong id() const;
    QVariantMap senderId() const;
    qlonglong chatId() const;
    QVariantMap sendingState() const;
    QVariantMap schedulingState() const;
    bool isOutgoing() const;
    bool isPinned() const;
    bool isFromOffline() const;
    bool canBeEdited() const;
    bool canBeForwarded() const;
    bool canBeRepliedInAnotherChat() const;
    bool canBeSaved() const;
    bool canBeDeletedOnlyForSelf() const;
    bool canBeDeletedForAllUsers() const;
    bool canGetAddedReactions() const;
    bool canGetStatistics() const;
    bool canGetMessageThread() const;
    bool canGetReadDate() const;
    bool canGetViewers() const;
    bool canGetMediaTimestampLinks() const;
    bool canReportReactions() const;
    bool hasTimestampedMedia() const;
    bool isChannelPost() const;
    bool isTopicMessage() const;
    bool containsUnreadMention() const;
    qlonglong date() const;
    qlonglong editDate() const;
    QVariantMap forwardInfo() const;
    QVariantMap importInfo() const;
    QVariantMap interactionInfo() const;
    QVariantList unreadReactions() const;
    QVariantMap factCheck() const;
    QVariantMap replyTo() const;
    qlonglong messageThreadId() const;
    qlonglong savedMessagesTopicId() const;
    QVariantMap selfDestructType() const;
    double selfDestructIn() const;
    double autoDeleteIn() const;
    qlonglong viaBotUserId() const;
    qlonglong senderBusinessBotUserId() const;
    int senderBoostCount() const;
    QString authorSignature() const;
    qlonglong mediaAlbumId() const;
    qlonglong effectId() const;
    QString restrictionReason() const;
    QVariantMap content() const;
    QVariantMap replyMarkup() const;

    QString getContent() noexcept;
    QString getTitle() noexcept;
    QString getDate() noexcept;
    QString getSenderName() noexcept;

    bool isServiceMessage();
    QString getServiceMessageContent();

    void setMessage(td::td_api::message *message);

signals:
    void dataChanged();

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void handleMessageSendSucceeded(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId);
    void handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId, td::td_api::object_ptr<td::td_api::error> &&error);
    void handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent);
    void handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup);
    void handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned);
    void handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo);

    td::td_api::chat *m_chat{};
    td::td_api::message *m_message{};

    bool m_openUser{false};

    StorageManager *m_storageManager{};
};
