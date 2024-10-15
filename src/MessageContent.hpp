#pragma once

#include <td/telegram/td_api.h>

#include <QDebug>

class MessageContent
{
public:
    virtual ~MessageContent()
    {
        qDebug() << "Destroying MessageContent at" << this;
    }

    virtual void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) = 0;
};

class MessageText : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QString webPage READ webPage NOTIFY textChanged)
    Q_PROPERTY(QString linkPreviewOptions READ linkPreviewOptions NOTIFY textChanged)

public:
    explicit MessageText(QObject *parent = nullptr);

    QString text() const;
    QString webPage() const;
    QString linkPreviewOptions() const;

    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void textChanged();

private:
    QString m_text;
    QString m_webPage;
    QString m_linkPreviewOptions;
};

class MessageAnimation : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption NOTIFY animationChanged)
    Q_PROPERTY(bool showCaptionAboveMedia READ showCaptionAboveMedia NOTIFY animationChanged)
    Q_PROPERTY(bool hasSpoiler READ hasSpoiler NOTIFY animationChanged)
    Q_PROPERTY(bool isSecret READ isSecret NOTIFY animationChanged)

public:
    explicit MessageAnimation(QObject *parent = nullptr);

    QString caption() const;
    bool showCaptionAboveMedia() const;
    bool hasSpoiler() const;
    bool isSecret() const;

    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void animationChanged();

private:
    QString m_caption;
    bool m_showCaptionAboveMedia;
    bool m_hasSpoiler;
    bool m_isSecret;
};

class MessageAudio : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption NOTIFY audioChanged)
    Q_PROPERTY(int duration READ duration NOTIFY audioChanged)
    Q_PROPERTY(QString title READ title NOTIFY audioChanged)
    Q_PROPERTY(QString performer READ performer NOTIFY audioChanged)
    Q_PROPERTY(QString fileName READ fileName NOTIFY audioChanged)

public:
    explicit MessageAudio(QObject *parent = nullptr);

    QString caption() const;
    int duration() const;
    QString title() const;
    QString performer() const;
    QString fileName() const;

    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void audioChanged();

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
    Q_PROPERTY(QString caption READ caption NOTIFY documentChanged)

public:
    explicit MessageDocument(QObject *parent = nullptr);

    QString caption() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void documentChanged();

private:
    QString m_caption;
};

class MessagePhoto : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption NOTIFY photoChanged)
    Q_PROPERTY(bool showCaptionAboveMedia READ showCaptionAboveMedia NOTIFY photoChanged)
    Q_PROPERTY(bool hasSpoiler READ hasSpoiler NOTIFY photoChanged)
    Q_PROPERTY(bool isSecret READ isSecret NOTIFY photoChanged)

public:
    explicit MessagePhoto(QObject *parent = nullptr);

    QString caption() const;
    bool showCaptionAboveMedia() const;
    bool hasSpoiler() const;
    bool isSecret() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void photoChanged();

private:
    QString m_caption;
    bool m_showCaptionAboveMedia;
    bool m_hasSpoiler;
    bool m_isSecret;
};

class MessageSticker : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(bool isPremium READ isPremium NOTIFY stickerChanged)

public:
    explicit MessageSticker(QObject *parent = nullptr);

    bool isPremium() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void stickerChanged();

private:
    bool m_isPremium;
};

class MessageVideo : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption NOTIFY videoChanged)
    Q_PROPERTY(bool showCaptionAboveMedia READ showCaptionAboveMedia NOTIFY videoChanged)
    Q_PROPERTY(bool hasSpoiler READ hasSpoiler NOTIFY videoChanged)
    Q_PROPERTY(bool isSecret READ isSecret NOTIFY videoChanged)

public:
    explicit MessageVideo(QObject *parent = nullptr);

    QString caption() const;
    bool showCaptionAboveMedia() const;
    bool hasSpoiler() const;
    bool isSecret() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void videoChanged();

private:
    QString m_caption;
    bool m_showCaptionAboveMedia;
    bool m_hasSpoiler;
    bool m_isSecret;
};

class MessageVideoNote : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(bool isViewed READ isViewed NOTIFY videoNoteChanged)
    Q_PROPERTY(bool isSecret READ isSecret NOTIFY videoNoteChanged)

public:
    explicit MessageVideoNote(QObject *parent = nullptr);

    bool isViewed() const;
    bool isSecret() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void videoNoteChanged();

private:
    bool m_isViewed;
    bool m_isSecret;
};

class MessageVoiceNote : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption NOTIFY voiceNoteChanged)
    Q_PROPERTY(bool isListened READ isListened NOTIFY voiceNoteChanged)

public:
    explicit MessageVoiceNote(QObject *parent = nullptr);

    QString caption() const;
    bool isListened() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void voiceNoteChanged();

private:
    QString m_caption;
    bool m_isListened;
};

class MessageLocation : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString location READ location NOTIFY locationChanged)
    Q_PROPERTY(int livePeriod READ livePeriod NOTIFY locationChanged)
    Q_PROPERTY(int expiresIn READ expiresIn NOTIFY locationChanged)
    Q_PROPERTY(int heading READ heading NOTIFY locationChanged)
    Q_PROPERTY(int proximityAlertRadius READ proximityAlertRadius NOTIFY locationChanged)

public:
    explicit MessageLocation(QObject *parent = nullptr);

    QString location() const;
    int livePeriod() const;
    int expiresIn() const;
    int heading() const;
    int proximityAlertRadius() const;

    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void locationChanged();

private:
    QString m_location;
    int m_livePeriod;
    int m_expiresIn;
    int m_heading;
    int m_proximityAlertRadius;
};

class MessageVenue : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString venue READ venue NOTIFY venueChanged)

public:
    explicit MessageVenue(QObject *parent = nullptr);

    QString venue() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void venueChanged();

private:
    QString m_venue;
};

class MessageContact : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString contact READ contact NOTIFY contactChanged)

public:
    explicit MessageContact(QObject *parent = nullptr);

    QString contact() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void contactChanged();

private:
    QString m_contact;
};

class MessageAnimatedEmoji : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString animatedEmoji READ animatedEmoji NOTIFY animatedEmojiChanged)
    Q_PROPERTY(QString emoji READ emoji NOTIFY animatedEmojiChanged)

public:
    explicit MessageAnimatedEmoji(QObject *parent = nullptr);

    QString animatedEmoji() const;
    QString emoji() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void animatedEmojiChanged();

private:
    QString m_animatedEmoji;
    QString m_emoji;
};

class MessagePoll : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString poll READ poll NOTIFY pollChanged)

public:
    explicit MessagePoll(QObject *parent = nullptr);

    QString poll() const;
    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void pollChanged();

private:
    QString m_poll;
};

class MessageInvoice : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString productInfo READ productInfo NOTIFY invoiceChanged)
    Q_PROPERTY(QString currency READ currency NOTIFY invoiceChanged)
    Q_PROPERTY(qint64 totalAmount READ totalAmount NOTIFY invoiceChanged)
    Q_PROPERTY(QString startParameter READ startParameter NOTIFY invoiceChanged)
    Q_PROPERTY(bool isTest READ isTest NOTIFY invoiceChanged)
    Q_PROPERTY(bool needShippingAddress READ needShippingAddress NOTIFY invoiceChanged)
    Q_PROPERTY(qint64 receiptMessageId READ receiptMessageId NOTIFY invoiceChanged)
    Q_PROPERTY(QString extendedMedia READ extendedMedia NOTIFY invoiceChanged)

public:
    explicit MessageInvoice(QObject *parent = nullptr);

    QString productInfo() const;
    QString currency() const;
    qint64 totalAmount() const;
    QString startParameter() const;
    bool isTest() const;
    bool needShippingAddress() const;
    qint64 receiptMessageId() const;
    QString extendedMedia() const;

    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void invoiceChanged();

private:
    QString m_productInfo;
    QString m_currency;
    qint64 m_totalAmount;
    QString m_startParameter;
    bool m_isTest;
    bool m_needShippingAddress;
    qint64 m_receiptMessageId;
    QString m_extendedMedia;
};

class MessageCall : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(bool isVideo READ isVideo NOTIFY callChanged)
    Q_PROPERTY(QString discardReason READ discardReason NOTIFY callChanged)
    Q_PROPERTY(int duration READ duration NOTIFY callChanged)

public:
    explicit MessageCall(QObject *parent = nullptr);

    bool isVideo() const;
    QString discardReason() const;
    int duration() const;

    void handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content) override;

signals:
    void callChanged();

private:
    bool m_isVideo;
    QString m_discardReason;
    int m_duration;
};
