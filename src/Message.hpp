#pragma once

#include "MessageContent.hpp"

#include <QDateTime>
#include <QVariant>

#include <memory>

class StorageManager;

class Message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id NOTIFY messageChanged)
    Q_PROPERTY(qlonglong chatId READ chatId NOTIFY messageChanged)
    Q_PROPERTY(qlonglong senderId READ senderId NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap sendingState READ sendingState NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap schedulingState READ schedulingState NOTIFY messageChanged)
    Q_PROPERTY(bool isOutgoing READ isOutgoing NOTIFY messageChanged)
    Q_PROPERTY(bool isPinned READ isPinned NOTIFY messageChanged)
    Q_PROPERTY(bool isFromOffline READ isFromOffline NOTIFY messageChanged)
    Q_PROPERTY(bool canBeEdited READ canBeEdited NOTIFY messageChanged)
    Q_PROPERTY(bool canBeForwarded READ canBeForwarded NOTIFY messageChanged)
    Q_PROPERTY(bool canBeRepliedInAnotherChat READ canBeRepliedInAnotherChat NOTIFY messageChanged)
    Q_PROPERTY(bool canBeSaved READ canBeSaved NOTIFY messageChanged)
    Q_PROPERTY(bool canBeDeletedOnlyForSelf READ canBeDeletedOnlyForSelf NOTIFY messageChanged)
    Q_PROPERTY(bool canBeDeletedForAllUsers READ canBeDeletedForAllUsers NOTIFY messageChanged)
    Q_PROPERTY(bool canGetAddedReactions READ canGetAddedReactions NOTIFY messageChanged)
    Q_PROPERTY(bool canGetStatistics READ canGetStatistics NOTIFY messageChanged)
    Q_PROPERTY(bool canGetMessageThread READ canGetMessageThread NOTIFY messageChanged)
    Q_PROPERTY(bool canGetReadDate READ canGetReadDate NOTIFY messageChanged)
    Q_PROPERTY(bool canGetViewers READ canGetViewers NOTIFY messageChanged)
    Q_PROPERTY(bool canGetMediaTimestampLinks READ canGetMediaTimestampLinks NOTIFY messageChanged)
    Q_PROPERTY(bool canReportReactions READ canReportReactions NOTIFY messageChanged)
    Q_PROPERTY(bool hasTimestampedMedia READ hasTimestampedMedia NOTIFY messageChanged)
    Q_PROPERTY(bool isChannelPost READ isChannelPost NOTIFY messageChanged)
    Q_PROPERTY(bool isTopicMessage READ isTopicMessage NOTIFY messageChanged)
    Q_PROPERTY(bool containsUnreadMention READ containsUnreadMention NOTIFY messageChanged)
    Q_PROPERTY(QDateTime date READ date NOTIFY messageChanged)
    Q_PROPERTY(QDateTime editDate READ editDate NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap forwardInfo READ forwardInfo NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap importInfo READ importInfo NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap interactionInfo READ interactionInfo NOTIFY messageChanged)
    Q_PROPERTY(QVariantList unreadReactions READ unreadReactions NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap factCheck READ factCheck NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap replyTo READ replyTo NOTIFY messageChanged)
    Q_PROPERTY(qlonglong messageThreadId READ messageThreadId NOTIFY messageChanged)
    Q_PROPERTY(qlonglong savedMessagesTopicId READ savedMessagesTopicId NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap selfDestructType READ selfDestructType NOTIFY messageChanged)
    Q_PROPERTY(double selfDestructIn READ selfDestructIn NOTIFY messageChanged)
    Q_PROPERTY(double autoDeleteIn READ autoDeleteIn NOTIFY messageChanged)
    Q_PROPERTY(qlonglong viaBotUserId READ viaBotUserId NOTIFY messageChanged)
    Q_PROPERTY(qlonglong senderBusinessBotUserId READ senderBusinessBotUserId NOTIFY messageChanged)
    Q_PROPERTY(int senderBoostCount READ senderBoostCount NOTIFY messageChanged)
    Q_PROPERTY(QString authorSignature READ authorSignature NOTIFY messageChanged)
    Q_PROPERTY(qlonglong mediaAlbumId READ mediaAlbumId NOTIFY messageChanged)
    Q_PROPERTY(qlonglong effectId READ effectId NOTIFY messageChanged)
    Q_PROPERTY(QString restrictionReason READ restrictionReason NOTIFY messageChanged)
    Q_PROPERTY(QVariantMap replyMarkup READ replyMarkup NOTIFY messageChanged)

    Q_PROPERTY(QString content READ getContent NOTIFY messageChanged)
    Q_PROPERTY(bool isService READ isService NOTIFY messageChanged)
    Q_PROPERTY(QString senderType READ senderType NOTIFY messageChanged)

public:
    explicit Message(QObject *parent = nullptr);
    explicit Message(td::td_api::message *message, QObject *parent = nullptr);

    qlonglong id() const;
    qlonglong chatId() const;
    qlonglong senderId() const;
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
    QDateTime date() const;
    QDateTime editDate() const;
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
    MessageContent *content() const;
    QVariantMap replyMarkup() const;

    QString getTitle() const noexcept;
    QString getSenderName() const noexcept;
    QString getServiceMessageContent() const;
    QString getContent() const noexcept;

    bool isService() const noexcept;
    QString senderType() const noexcept;

    void setMessage(td::td_api::message *message);
    void setContent(td::td_api::object_ptr<td::td_api::MessageContent> content);

signals:
    void messageChanged();

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void handleMessageSendSucceeded(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId);
    void handleMessageSendFailed(td::td_api::object_ptr<td::td_api::message> &&message, qlonglong oldMessageId,
                                 td::td_api::object_ptr<td::td_api::error> &&error);
    void handleMessageContent(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::MessageContent> &&newContent);
    void handleMessageEdited(qlonglong chatId, qlonglong messageId, int editDate, td::td_api::object_ptr<td::td_api::ReplyMarkup> &&replyMarkup);
    void handleMessageIsPinned(qlonglong chatId, qlonglong messageId, bool isPinned);
    void handleMessageInteractionInfo(qlonglong chatId, qlonglong messageId, td::td_api::object_ptr<td::td_api::messageInteractionInfo> &&interactionInfo);

    int m_contentType;

    td::td_api::chat *m_chat{};
    td::td_api::message *m_message{};

    StorageManager *m_storageManager{};

    std::unique_ptr<MessageContent> m_content;
};

Q_DECLARE_METATYPE(Message *);
