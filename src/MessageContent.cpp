#include "MessageContent.hpp"

#include "Utils.hpp"

MessageText::MessageText(td::td_api::object_ptr<td::td_api::messageText> content, QObject *parent)
    : QObject(parent)
{
    m_text = QString::fromStdString(content->text_->text_);
    m_formattedText = Utils::formattedText(content->text_);
}

QString MessageText::text() const
{
    return m_text;
}

QString MessageText::formattedText() const
{
    return m_formattedText;
}

MessageAnimation::MessageAnimation(td::td_api::object_ptr<td::td_api::messageAnimation> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);
}

QString MessageAnimation::caption() const
{
    return m_caption;
}

MessageAudio::MessageAudio(td::td_api::object_ptr<td::td_api::messageAudio> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);

    if (content->audio_)
    {
        m_duration = content->audio_->duration_;
        m_title = QString::fromStdString(content->audio_->title_);
        m_performer = QString::fromStdString(content->audio_->performer_);
        m_fileName = QString::fromStdString(content->audio_->file_name_);
    }
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

MessageDocument::MessageDocument(td::td_api::object_ptr<td::td_api::messageDocument> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);
    m_fileName = QString::fromStdString(content->document_->file_name_);
}

QString MessageDocument::caption() const
{
    return m_caption;
}

QString MessageDocument::fileName() const
{
    return m_fileName;
}

MessagePhoto::MessagePhoto(td::td_api::object_ptr<td::td_api::messagePhoto> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);
}

QString MessagePhoto::caption() const
{
    return m_caption;
}

MessageSticker::MessageSticker(td::td_api::object_ptr<td::td_api::messageSticker> content, QObject *parent)
    : QObject(parent)
{
    m_emoji = QString::fromStdString(content->sticker_->emoji_);
}

QString MessageSticker::emoji() const
{
    return m_emoji;
}

MessageVideo::MessageVideo(td::td_api::object_ptr<td::td_api::messageVideo> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);
}

QString MessageVideo::caption() const
{
    return m_caption;
}

MessageVideoNote::MessageVideoNote(td::td_api::object_ptr<td::td_api::messageVideoNote> content, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(content)
}

MessageVoiceNote::MessageVoiceNote(td::td_api::object_ptr<td::td_api::messageVoiceNote> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);
}

QString MessageVoiceNote::caption() const
{
    return m_caption;
}

MessageLocation::MessageLocation(td::td_api::object_ptr<td::td_api::messageLocation> content, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(content)
}

MessageVenue::MessageVenue(td::td_api::object_ptr<td::td_api::messageVenue> content, QObject *parent)
    : QObject(parent)
{
    m_venue = QString::fromStdString(content->venue_->title_);
}

QString MessageVenue::venue() const
{
    return m_venue;
}

MessageContact::MessageContact(td::td_api::object_ptr<td::td_api::messageContact> content, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(content)
}

MessageAnimatedEmoji::MessageAnimatedEmoji(td::td_api::object_ptr<td::td_api::messageAnimatedEmoji> content, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(content)
}

MessagePoll::MessagePoll(td::td_api::object_ptr<td::td_api::messagePoll> content, QObject *parent)
    : QObject(parent)
{
    m_question = QString::fromStdString(content->poll_->question_->text_);
}

QString MessagePoll::question() const
{
    return m_question;
}

MessageInvoice::MessageInvoice(td::td_api::object_ptr<td::td_api::messageInvoice> content, QObject *parent)
    : QObject(parent)
{
    Q_UNUSED(content)
}

MessageCall::MessageCall(td::td_api::object_ptr<td::td_api::messageCall> content, QObject *parent)
    : QObject(parent)
{
    m_isVideo = content->is_video_;

    switch (content->discard_reason_->get_id())
    {
        case td::td_api::callDiscardReasonEmpty::ID:
            m_discardReason = DiscardReason::Empty;
            break;
        case td::td_api::callDiscardReasonMissed::ID:
            m_discardReason = DiscardReason::Missed;
            break;
        case td::td_api::callDiscardReasonDeclined::ID:
            m_discardReason = DiscardReason::Declined;
            break;
        case td::td_api::callDiscardReasonDisconnected::ID:
            m_discardReason = DiscardReason::Disconnected;
            break;
        case td::td_api::callDiscardReasonHungUp::ID:
            m_discardReason = DiscardReason::HungUp;
            break;
        default:
            break;
    }

    m_duration = content->duration_;
}

bool MessageCall::isVideo() const
{
    return m_isVideo;
}

MessageCall::DiscardReason MessageCall::discardReason() const
{
    return m_discardReason;
}

int MessageCall::duration() const
{
    return m_duration;
}
