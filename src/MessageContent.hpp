#pragma once

#include <td/telegram/td_api.h>

#include <QDebug>
#include <QMetaType>

class MessageContent
{
public:
    virtual ~MessageContent()
    {
        // qDebug() << "Destroying MessageContent at" << this;
    }
};

class MessageText : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text CONSTANT)
    Q_PROPERTY(QString formattedText READ formattedText CONSTANT)

public:
    explicit MessageText(td::td_api::object_ptr<td::td_api::messageText> content, QObject *parent = nullptr);

    QString text() const;
    QString formattedText() const;

private:
    QString m_text;
    QString m_formattedText;
};

class MessageAnimation : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption CONSTANT)

public:
    explicit MessageAnimation(td::td_api::object_ptr<td::td_api::messageAnimation> content, QObject *parent = nullptr);

    QString caption() const;

private:
    QString m_caption;
};

class MessageAudio : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption CONSTANT)
    Q_PROPERTY(int duration READ duration CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(QString performer READ performer CONSTANT)
    Q_PROPERTY(QString fileName READ fileName CONSTANT)

public:
    explicit MessageAudio(td::td_api::object_ptr<td::td_api::messageAudio> content, QObject *parent = nullptr);

    QString caption() const;
    int duration() const;
    QString title() const;
    QString performer() const;
    QString fileName() const;

private:
    QString m_caption;
    int m_duration;
    QString m_title;
    QString m_performer;
    QString m_fileName;
};

class MessageDocument : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption CONSTANT)
    Q_PROPERTY(QString fileName READ fileName CONSTANT)

public:
    explicit MessageDocument(td::td_api::object_ptr<td::td_api::messageDocument> content, QObject *parent = nullptr);

    QString caption() const;
    QString fileName() const;

private:
    QString m_caption;
    QString m_fileName;
};

class MessagePhoto : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption CONSTANT)

public:
    explicit MessagePhoto(td::td_api::object_ptr<td::td_api::messagePhoto> content, QObject *parent = nullptr);

    QString caption() const;

private:
    QString m_caption;
};

class MessageSticker : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString emoji READ emoji CONSTANT)

public:
    explicit MessageSticker(td::td_api::object_ptr<td::td_api::messageSticker> content, QObject *parent = nullptr);

    QString emoji() const;

private:
    QString m_emoji;
};

class MessageVideo : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption CONSTANT)

public:
    explicit MessageVideo(td::td_api::object_ptr<td::td_api::messageVideo> content, QObject *parent = nullptr);

    QString caption() const;

private:
    QString m_caption;
};

class MessageVideoNote : public QObject, public MessageContent
{
    Q_OBJECT

public:
    explicit MessageVideoNote(td::td_api::object_ptr<td::td_api::messageVideoNote> content, QObject *parent = nullptr);
};

class MessageVoiceNote : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption CONSTANT)

public:
    explicit MessageVoiceNote(td::td_api::object_ptr<td::td_api::messageVoiceNote> content, QObject *parent = nullptr);

    QString caption() const;

private:
    QString m_caption;
};

class MessageLocation : public QObject, public MessageContent
{
    Q_OBJECT

public:
    explicit MessageLocation(td::td_api::object_ptr<td::td_api::messageLocation> content, QObject *parent = nullptr);
};

class MessageVenue : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString venue READ venue CONSTANT)

public:
    explicit MessageVenue(td::td_api::object_ptr<td::td_api::messageVenue> content, QObject *parent = nullptr);

    QString venue() const;

private:
    QString m_venue;
};

class MessageContact : public QObject, public MessageContent
{
    Q_OBJECT

public:
    explicit MessageContact(td::td_api::object_ptr<td::td_api::messageContact> content, QObject *parent = nullptr);
};

class MessageAnimatedEmoji : public QObject, public MessageContent
{
    Q_OBJECT

public:
    explicit MessageAnimatedEmoji(td::td_api::object_ptr<td::td_api::messageAnimatedEmoji> content, QObject *parent = nullptr);
};

class MessagePoll : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString question READ question CONSTANT)

public:
    explicit MessagePoll(td::td_api::object_ptr<td::td_api::messagePoll> content, QObject *parent = nullptr);

    QString question() const;

private:
    QString m_question;
};

class MessageInvoice : public QObject, public MessageContent
{
    Q_OBJECT

public:
    explicit MessageInvoice(td::td_api::object_ptr<td::td_api::messageInvoice> content, QObject *parent = nullptr);
};

class MessageCall : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(bool isVideo READ isVideo CONSTANT)
    Q_PROPERTY(DiscardReason discardReason READ discardReason CONSTANT)
    Q_PROPERTY(int duration READ duration CONSTANT)

public:
    explicit MessageCall(td::td_api::object_ptr<td::td_api::messageCall> content, QObject *parent = nullptr);

    enum class DiscardReason { Empty, Missed, Declined, Disconnected, HungUp };

    bool isVideo() const;
    DiscardReason discardReason() const;
    int duration() const;

private:
    bool m_isVideo;
    DiscardReason m_discardReason;
    int m_duration;
};

// Declare metatypes for each message content class
Q_DECLARE_METATYPE(MessageText *)
Q_DECLARE_METATYPE(MessageAnimation *)
Q_DECLARE_METATYPE(MessageAudio *)
Q_DECLARE_METATYPE(MessageDocument *)
Q_DECLARE_METATYPE(MessagePhoto *)
Q_DECLARE_METATYPE(MessageSticker *)
Q_DECLARE_METATYPE(MessageVideo *)
Q_DECLARE_METATYPE(MessageVideoNote *)
Q_DECLARE_METATYPE(MessageVoiceNote *)
Q_DECLARE_METATYPE(MessageLocation *)
Q_DECLARE_METATYPE(MessageVenue *)
Q_DECLARE_METATYPE(MessageContact *)
Q_DECLARE_METATYPE(MessageAnimatedEmoji *)
Q_DECLARE_METATYPE(MessagePoll *)
Q_DECLARE_METATYPE(MessageInvoice *)
Q_DECLARE_METATYPE(MessageCall *)
