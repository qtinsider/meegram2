#include "MessageContent.hpp"

MessageText::MessageText(QObject *parent)
    : QObject(parent)
{
}

QString MessageText::text() const
{
    return m_text;
}

QString MessageText::webPage() const
{
    return m_webPage;
}

QString MessageText::linkPreviewOptions() const
{
    return m_linkPreviewOptions;
}

void MessageText::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto textContent = td::td_api::move_object_as<td::td_api::messageText>(content))
    {
        m_text = QString::fromStdString(textContent->text_->text_);

        if (textContent->web_page_)
        {
            m_webPage = QString::fromStdString(textContent->web_page_->url_);
        }
        else
        {
            m_webPage.clear();
        }
        if (textContent->link_preview_options_)
        {
            // m_linkPreviewOptions = QString::fromStdString(textContent->link_preview_options_->title_);
        }
        else
        {
            m_linkPreviewOptions.clear();
        }
        emit textChanged();
    }
}

MessageAnimation::MessageAnimation(QObject *parent)
    : QObject(parent)
    , m_showCaptionAboveMedia(false)
    , m_hasSpoiler(false)
    , m_isSecret(false)
{
}

QString MessageAnimation::caption() const
{
    return m_caption;
}

bool MessageAnimation::showCaptionAboveMedia() const
{
    return m_showCaptionAboveMedia;
}

bool MessageAnimation::hasSpoiler() const
{
    return m_hasSpoiler;
}

bool MessageAnimation::isSecret() const
{
    return m_isSecret;
}

void MessageAnimation::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto animationContent = td::td_api::move_object_as<td::td_api::messageAnimation>(content))
    {
        m_caption = QString::fromStdString(animationContent->caption_->text_);  // Extract caption text
        m_showCaptionAboveMedia = animationContent->show_caption_above_media_;
        m_hasSpoiler = animationContent->has_spoiler_;
        m_isSecret = animationContent->is_secret_;

        emit animationChanged();
    }
}

MessageAudio::MessageAudio(QObject *parent)
    : QObject(parent)
{
}

QString MessageAudio::caption() const
{
    return m_caption;
}

int MessageAudio::duration() const
{
    return m_duration;
}

QString MessageAudio::title() const
{
    return m_title;
}

QString MessageAudio::performer() const
{
    return m_performer;
}

QString MessageAudio::fileName() const
{
    return m_fileName;
}

void MessageAudio::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto audioContent = td::td_api::move_object_as<td::td_api::messageAudio>(content))
    {
        m_caption = QString::fromStdString(audioContent->caption_->text_);

        if (audioContent->audio_)
        {
            m_duration = audioContent->audio_->duration_;
            m_title  = QString::fromStdString(audioContent->audio_->title_);
            m_performer = QString::fromStdString(audioContent->audio_->performer_);
            m_fileName = QString::fromStdString(audioContent->audio_->file_name_);
        }
        emit audioChanged();
    }
}

MessageDocument::MessageDocument(QObject *parent)
    : QObject(parent)
{
}

QString MessageDocument::caption() const
{
    return m_caption;
}

void MessageDocument::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto documentContent = td::td_api::move_object_as<td::td_api::messageDocument>(content))
    {
        m_caption = QString::fromStdString(documentContent->caption_->text_);
        emit documentChanged();
    }
}

MessagePhoto::MessagePhoto(QObject *parent)
    : QObject(parent)
    , m_showCaptionAboveMedia(false)
    , m_hasSpoiler(false)
    , m_isSecret(false)
{
}

QString MessagePhoto::caption() const
{
    return m_caption;
}

bool MessagePhoto::showCaptionAboveMedia() const
{
    return m_showCaptionAboveMedia;
}

bool MessagePhoto::hasSpoiler() const
{
    return m_hasSpoiler;
}

bool MessagePhoto::isSecret() const
{
    return m_isSecret;
}

void MessagePhoto::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto photoContent = td::td_api::move_object_as<td::td_api::messagePhoto>(content))
    {
        m_caption = QString::fromStdString(photoContent->caption_->text_);
        m_showCaptionAboveMedia = photoContent->show_caption_above_media_;
        m_hasSpoiler = photoContent->has_spoiler_;
        m_isSecret = photoContent->is_secret_;
        emit photoChanged();
    }
}

MessageSticker::MessageSticker(QObject *parent)
    : QObject(parent)
    , m_isPremium(false)
{
}

bool MessageSticker::isPremium() const
{
    return m_isPremium;
}

void MessageSticker::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto stickerContent = td::td_api::move_object_as<td::td_api::messageSticker>(content))
    {
        m_isPremium = stickerContent->is_premium_;
        emit stickerChanged();
    }
}

MessageVideo::MessageVideo(QObject *parent)
    : QObject(parent)
    , m_showCaptionAboveMedia(false)
    , m_hasSpoiler(false)
    , m_isSecret(false)
{
}

QString MessageVideo::caption() const
{
    return m_caption;
}

bool MessageVideo::showCaptionAboveMedia() const
{
    return m_showCaptionAboveMedia;
}

bool MessageVideo::hasSpoiler() const
{
    return m_hasSpoiler;
}

bool MessageVideo::isSecret() const
{
    return m_isSecret;
}

void MessageVideo::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto videoContent = td::td_api::move_object_as<td::td_api::messageVideo>(content))
    {
        m_caption = QString::fromStdString(videoContent->caption_->text_);
        m_showCaptionAboveMedia = videoContent->show_caption_above_media_;
        m_hasSpoiler = videoContent->has_spoiler_;
        m_isSecret = videoContent->is_secret_;
        emit videoChanged();
    }
}

MessageVideoNote::MessageVideoNote(QObject *parent)
    : QObject(parent)
    , m_isViewed(false)
    , m_isSecret(false)
{
}

bool MessageVideoNote::isViewed() const
{
    return m_isViewed;
}

bool MessageVideoNote::isSecret() const
{
    return m_isSecret;
}

void MessageVideoNote::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto videoNoteContent = td::td_api::move_object_as<td::td_api::messageVideoNote>(content))
    {
        m_isViewed = videoNoteContent->is_viewed_;
        m_isSecret = videoNoteContent->is_secret_;
        emit videoNoteChanged();
    }
}

MessageVoiceNote::MessageVoiceNote(QObject *parent)
    : QObject(parent)
    , m_isListened(false)
{
}

QString MessageVoiceNote::caption() const
{
    return m_caption;
}

bool MessageVoiceNote::isListened() const
{
    return m_isListened;
}

void MessageVoiceNote::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto voiceNoteContent = td::td_api::move_object_as<td::td_api::messageVoiceNote>(content))
    {
        m_caption = QString::fromStdString(voiceNoteContent->caption_->text_);
        m_isListened = voiceNoteContent->is_listened_;
        emit voiceNoteChanged();
    }
}

MessageLocation::MessageLocation(QObject *parent)
    : QObject(parent)
    , m_livePeriod(0)
    , m_expiresIn(0)
    , m_heading(0)
    , m_proximityAlertRadius(0)
{
}

QString MessageLocation::location() const
{
    return m_location;
}

int MessageLocation::livePeriod() const
{
    return m_livePeriod;
}

int MessageLocation::expiresIn() const
{
    return m_expiresIn;
}

int MessageLocation::heading() const
{
    return m_heading;
}

int MessageLocation::proximityAlertRadius() const
{
    return m_proximityAlertRadius;
}

void MessageLocation::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto locationContent = td::td_api::move_object_as<td::td_api::messageLocation>(content))
    {
        // m_location = QString::fromStdString(locationContent->location_->address_);
        m_livePeriod = locationContent->live_period_;
        m_expiresIn = locationContent->expires_in_;
        m_heading = locationContent->heading_;
        m_proximityAlertRadius = locationContent->proximity_alert_radius_;
        emit locationChanged();
    }
}

MessageVenue::MessageVenue(QObject *parent)
    : QObject(parent)
{
}

QString MessageVenue::venue() const
{
    return m_venue;
}

void MessageVenue::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto venueContent = td::td_api::move_object_as<td::td_api::messageVenue>(content))
    {
        m_venue = QString::fromStdString(venueContent->venue_->title_);
        emit venueChanged();
    }
}

MessageContact::MessageContact(QObject *parent)
    : QObject(parent)
{
}

QString MessageContact::contact() const
{
    return m_contact;
}

void MessageContact::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto contactContent = td::td_api::move_object_as<td::td_api::messageContact>(content))
    {
        m_contact = QString::fromStdString(contactContent->contact_->phone_number_);
        emit contactChanged();
    }
}

MessageAnimatedEmoji::MessageAnimatedEmoji(QObject *parent)
    : QObject(parent)
{
}

QString MessageAnimatedEmoji::animatedEmoji() const
{
    return m_animatedEmoji;
}

QString MessageAnimatedEmoji::emoji() const
{
    return m_emoji;
}

void MessageAnimatedEmoji::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto animatedEmojiContent = td::td_api::move_object_as<td::td_api::messageAnimatedEmoji>(content))
    {
        // m_animatedEmoji = QString::fromStdString(animatedEmojiContent->animated_emoji_->animation_->file_name_);
        m_emoji = QString::fromStdString(animatedEmojiContent->emoji_);
        emit animatedEmojiChanged();
    }
}

MessagePoll::MessagePoll(QObject *parent)
    : QObject(parent)
{
}

QString MessagePoll::poll() const
{
    return m_poll;
}

void MessagePoll::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto pollContent = td::td_api::move_object_as<td::td_api::messagePoll>(content))
    {
        // m_poll = QString::fromStdString(pollContent->poll_->question_);
        emit pollChanged();
    }
}

MessageInvoice::MessageInvoice(QObject *parent)
    : QObject(parent)
    , m_totalAmount(0)
    , m_isTest(false)
    , m_needShippingAddress(false)
    , m_receiptMessageId(0)
{
}

QString MessageInvoice::productInfo() const
{
    return m_productInfo;
}

QString MessageInvoice::currency() const
{
    return m_currency;
}

qint64 MessageInvoice::totalAmount() const
{
    return m_totalAmount;
}

QString MessageInvoice::startParameter() const
{
    return m_startParameter;
}

bool MessageInvoice::isTest() const
{
    return m_isTest;
}

bool MessageInvoice::needShippingAddress() const
{
    return m_needShippingAddress;
}

qint64 MessageInvoice::receiptMessageId() const
{
    return m_receiptMessageId;
}

QString MessageInvoice::extendedMedia() const
{
    return m_extendedMedia;
}

void MessageInvoice::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto invoiceContent = td::td_api::move_object_as<td::td_api::messageInvoice>(content))
    {
        // m_productInfo = QString::fromStdString(invoiceContent->product_info_->description_);
        m_currency = QString::fromStdString(invoiceContent->currency_);
        m_totalAmount = invoiceContent->total_amount_;
        m_startParameter = QString::fromStdString(invoiceContent->start_parameter_);
        m_isTest = invoiceContent->is_test_;
        m_needShippingAddress = invoiceContent->need_shipping_address_;
        m_receiptMessageId = invoiceContent->receipt_message_id_;
        if (invoiceContent->extended_media_)
        {
            // m_extendedMedia = QString::fromStdString(invoiceContent->extended_media_->type_);
        }
        else
        {
            m_extendedMedia.clear();
        }
        emit invoiceChanged();
    }
}

MessageCall::MessageCall(QObject *parent)
    : QObject(parent)
    , m_isVideo(false)
    , m_duration(0)
{
}

bool MessageCall::isVideo() const
{
    return m_isVideo;
}

QString MessageCall::discardReason() const
{
    return m_discardReason;
}

int MessageCall::duration() const
{
    return m_duration;
}

void MessageCall::handleContent(td::td_api::object_ptr<td::td_api::MessageContent> content)
{
    if (auto callContent = td::td_api::move_object_as<td::td_api::messageCall>(content))
    {
        m_isVideo = callContent->is_video_;
        // m_discardReason = QString::fromStdString(callContent->discard_reason_->description_);
        m_duration = callContent->duration_;
        emit callChanged();
    }
}
