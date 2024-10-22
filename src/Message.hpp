#pragma once

#include "MessageContent.hpp"

#include <QDateTime>
#include <QVariant>

#include <memory>

class Chat;
class StorageManager;

class Message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id NOTIFY messageChanged)
    Q_PROPERTY(qlonglong chatId READ chatId NOTIFY messageChanged)
    Q_PROPERTY(qlonglong senderId READ senderId NOTIFY messageChanged)
    Q_PROPERTY(bool isOutgoing READ isOutgoing NOTIFY messageChanged)
    Q_PROPERTY(QDateTime date READ date NOTIFY messageChanged)
    Q_PROPERTY(QDateTime editDate READ editDate NOTIFY messageChanged)

    Q_PROPERTY(QString content READ getContent NOTIFY messageChanged)
    Q_PROPERTY(bool isService READ isService NOTIFY messageChanged)
    Q_PROPERTY(QString senderType READ senderType NOTIFY messageChanged)

public:
    explicit Message(td::td_api::object_ptr<td::td_api::message> message, QObject *parent = nullptr);

    qlonglong id() const;
    qlonglong chatId() const;
    qlonglong senderId() const;
    bool isOutgoing() const;
    QDateTime date() const;
    QDateTime editDate() const;
    MessageContent *content() const;

    QString getTitle() const noexcept;
    QString getSenderName() const noexcept;
    QString getServiceMessageContent() const;
    QString getContent() const noexcept;

    bool isService() const noexcept;
    QString senderType() const noexcept;

    int contentType() const;

    void setMessage(td::td_api::object_ptr<td::td_api::message> message);
    void setContent(td::td_api::object_ptr<td::td_api::MessageContent> content);

signals:
    void messageChanged();

private:
    int m_contentType;

    Chat *m_chat{};
    StorageManager *m_storageManager{};

    std::unique_ptr<MessageContent> m_content;

    td::td_api::object_ptr<td::td_api::message> m_message;
};

Q_DECLARE_METATYPE(Message *);
