#pragma once

#include "MessageContent.hpp"

#include <QDateTime>

#include <memory>

class Message : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id CONSTANT)
    Q_PROPERTY(qlonglong chatId READ chatId CONSTANT)
    Q_PROPERTY(qlonglong senderId READ senderId CONSTANT)
    Q_PROPERTY(bool isOutgoing READ isOutgoing CONSTANT)
    Q_PROPERTY(QDateTime date READ date CONSTANT)
    Q_PROPERTY(QDateTime editDate READ editDate NOTIFY messageChanged)

    Q_PROPERTY(QString contentType READ contentTypeString NOTIFY messageChanged)
    Q_PROPERTY(bool isService READ isService CONSTANT)

public:
    explicit Message(td::td_api::object_ptr<td::td_api::message> message, QObject *parent = nullptr);

    enum class SenderType { Unknown, Chat, User };

    qlonglong id() const;
    qlonglong chatId() const;
    qlonglong senderId() const;
    bool isOutgoing() const;
    QDateTime date() const;
    QDateTime editDate() const;
    MessageContent *content() const;

    bool isService() const noexcept;

    int contentType() const;
    QString contentTypeString() const;

    SenderType senderType() const;

    void setContent(td::td_api::object_ptr<td::td_api::MessageContent> content);
    void setEditDate(int editDate);

signals:
    void messageChanged();

private:
    qlonglong m_id;
    qlonglong m_chatId;
    qlonglong m_senderId;
    bool m_isOutgoing;
    QDateTime m_date;
    QDateTime m_editDate;

    int m_contentType;
    SenderType m_senderType;

    std::unique_ptr<MessageContent> m_content;

    td::td_api::object_ptr<td::td_api::message> m_message;
};

Q_DECLARE_METATYPE(Message *);
