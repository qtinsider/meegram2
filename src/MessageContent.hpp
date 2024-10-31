#pragma once

#include <td/telegram/td_api.h>

#include <QDebug>
#include <QMetaType>

class MessageContent
{
public:
    virtual ~MessageContent()
    {
        qDebug() << "Destroying MessageContent at" << this;
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
    Q_PROPERTY(QString caption READ caption NOTIFY animationChanged)
    Q_PROPERTY(bool showCaptionAboveMedia READ showCaptionAboveMedia NOTIFY animationChanged)
    Q_PROPERTY(bool hasSpoiler READ hasSpoiler NOTIFY animationChanged)
    Q_PROPERTY(bool isSecret READ isSecret NOTIFY animationChanged)

public:
    explicit MessageAnimation(td::td_api::object_ptr<td::td_api::messageAnimation> content, QObject *parent = nullptr);

    QString caption() const;
    bool showCaptionAboveMedia() const;
    bool hasSpoiler() const;
    bool isSecret() const;

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
    explicit MessageAudio(td::td_api::object_ptr<td::td_api::messageAudio> content, QObject *parent = nullptr);

    QString caption() const;
    int duration() const;
    QString title() const;
    QString performer() const;
    QString fileName() const;

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
    Q_PROPERTY(QString fileName READ fileName NOTIFY documentChanged)

public:
    explicit MessageDocument(td::td_api::object_ptr<td::td_api::messageDocument> content, QObject *parent = nullptr);

    QString caption() const;
    QString fileName() const;

signals:
    void documentChanged();

private:
    QString m_caption;
    QString m_fileName;
};

class MessagePhoto : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption NOTIFY photoChanged)
    Q_PROPERTY(bool showCaptionAboveMedia READ showCaptionAboveMedia NOTIFY photoChanged)
    Q_PROPERTY(bool hasSpoiler READ hasSpoiler NOTIFY photoChanged)
    Q_PROPERTY(bool isSecret READ isSecret NOTIFY photoChanged)

public:
    explicit MessagePhoto(td::td_api::object_ptr<td::td_api::messagePhoto> content, QObject *parent = nullptr);

    QString caption() const;
    bool showCaptionAboveMedia() const;
    bool hasSpoiler() const;
    bool isSecret() const;

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
    Q_PROPERTY(QString emoji READ emoji NOTIFY stickerChanged)

public:
    explicit MessageSticker(td::td_api::object_ptr<td::td_api::messageSticker> content, QObject *parent = nullptr);

    QString emoji() const;

signals:
    void stickerChanged();

private:
    QString m_emoji;
};

class MessageVideo : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString caption READ caption NOTIFY videoChanged)
    Q_PROPERTY(bool showCaptionAboveMedia READ showCaptionAboveMedia NOTIFY videoChanged)
    Q_PROPERTY(bool hasSpoiler READ hasSpoiler NOTIFY videoChanged)
    Q_PROPERTY(bool isSecret READ isSecret NOTIFY videoChanged)

public:
    explicit MessageVideo(td::td_api::object_ptr<td::td_api::messageVideo> content, QObject *parent = nullptr);

    QString caption() const;
    bool showCaptionAboveMedia() const;
    bool hasSpoiler() const;
    bool isSecret() const;

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
    explicit MessageVideoNote(td::td_api::object_ptr<td::td_api::messageVideoNote> content, QObject *parent = nullptr);

    bool isViewed() const;
    bool isSecret() const;

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
    explicit MessageVoiceNote(td::td_api::object_ptr<td::td_api::messageVoiceNote> content, QObject *parent = nullptr);

    QString caption() const;
    bool isListened() const;

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
    explicit MessageLocation(td::td_api::object_ptr<td::td_api::messageLocation> content, QObject *parent = nullptr);

    QString location() const;
    int livePeriod() const;
    int expiresIn() const;
    int heading() const;
    int proximityAlertRadius() const;

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
    explicit MessageVenue(td::td_api::object_ptr<td::td_api::messageVenue> content, QObject *parent = nullptr);

    QString venue() const;

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
    explicit MessageContact(td::td_api::object_ptr<td::td_api::messageContact> content, QObject *parent = nullptr);

    QString contact() const;

signals:
    void contactChanged();

private:
    QString m_contact;
};

class MessageAnimatedEmoji : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString emoji READ emoji NOTIFY animatedEmojiChanged)

public:
    explicit MessageAnimatedEmoji(td::td_api::object_ptr<td::td_api::messageAnimatedEmoji> content, QObject *parent = nullptr);

    QString emoji() const;

signals:
    void animatedEmojiChanged();

private:
    QString m_emoji;
};

class MessagePoll : public QObject, public MessageContent
{
    Q_OBJECT
    Q_PROPERTY(QString question READ question NOTIFY pollChanged)

public:
    explicit MessagePoll(td::td_api::object_ptr<td::td_api::messagePoll> content, QObject *parent = nullptr);

    QString question() const;

signals:
    void pollChanged();

private:
    QString m_question;
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
    explicit MessageInvoice(td::td_api::object_ptr<td::td_api::messageInvoice> content, QObject *parent = nullptr);

    QString productInfo() const;
    QString currency() const;
    qint64 totalAmount() const;
    QString startParameter() const;
    bool isTest() const;
    bool needShippingAddress() const;
    qint64 receiptMessageId() const;
    QString extendedMedia() const;

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

class MessageService : public QObject, public MessageContent
{
    Q_OBJECT

    Q_PROPERTY(QString groupTitle READ groupTitle CONSTANT)
    Q_PROPERTY(QString groupPhoto READ groupPhoto CONSTANT)
    Q_PROPERTY(QList<qlonglong> addedMembers READ addedMembers CONSTANT)
    Q_PROPERTY(qlonglong removedMember READ removedMember CONSTANT)
    Q_PROPERTY(qlonglong upgradedToSupergroup READ upgradedToSupergroup CONSTANT)
    Q_PROPERTY(qlonglong upgradedFromGroup READ upgradedFromGroup CONSTANT)
    Q_PROPERTY(qlonglong pinnedMessage READ pinnedMessage CONSTANT)
    Q_PROPERTY(QString background READ background CONSTANT)
    Q_PROPERTY(QString theme READ theme CONSTANT)
    Q_PROPERTY(int autoDeleteTime READ autoDeleteTime CONSTANT)
    Q_PROPERTY(QString customAction READ customAction CONSTANT)
    Q_PROPERTY(QString profilePhotoSuggestion READ profilePhotoSuggestion CONSTANT)

public:
    explicit MessageService(td::td_api::object_ptr<td::td_api::MessageContent> content, QObject *parent = nullptr);

    QString groupTitle() const;
    QString groupPhoto() const;
    QList<qlonglong> addedMembers() const;
    qlonglong removedMember() const;
    qlonglong upgradedToSupergroup() const;
    qlonglong upgradedFromGroup() const;
    qlonglong pinnedMessage() const;
    QString background() const;
    QString theme() const;
    int autoDeleteTime() const;
    QString customAction() const;
    QString profilePhotoSuggestion() const;

private:
    QString m_groupTitle;
    QString m_groupPhoto;
    QList<qlonglong> m_addedMembers;
    qlonglong m_removedMember;
    qlonglong m_upgradedToSupergroup;
    qlonglong m_upgradedFromGroup;
    qlonglong m_pinnedMessage;
    QString m_background;
    QString m_theme;
    int m_autoDeleteTime;
    QString m_customAction;
    QString m_profilePhotoSuggestion;
    qlonglong m_fromUserId;
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
Q_DECLARE_METATYPE(MessageService *)
