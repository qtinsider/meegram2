#include "MessageContent.hpp"

MessageText::MessageText(td::td_api::object_ptr<td::td_api::messageText> content, QObject *parent)
    : QObject(parent)
{
    m_text = QString::fromStdString(content->text_->text_);

    if (content->web_page_)
    {
        m_webPage = QString::fromStdString(content->web_page_->url_);
    }
    else
    {
        m_webPage.clear();
    }
    if (content->link_preview_options_)
    {
        m_linkPreviewOptions = QString::fromStdString(content->link_preview_options_->url_);
    }
    else
    {
        m_linkPreviewOptions.clear();
    }
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

MessageAnimation::MessageAnimation(td::td_api::object_ptr<td::td_api::messageAnimation> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);  // Extract caption text
    m_showCaptionAboveMedia = content->show_caption_above_media_;
    m_hasSpoiler = content->has_spoiler_;
    m_isSecret = content->is_secret_;
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
    m_showCaptionAboveMedia = content->show_caption_above_media_;
    m_hasSpoiler = content->has_spoiler_;
    m_isSecret = content->is_secret_;
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
    m_showCaptionAboveMedia = content->show_caption_above_media_;
    m_hasSpoiler = content->has_spoiler_;
    m_isSecret = content->is_secret_;
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

MessageVideoNote::MessageVideoNote(td::td_api::object_ptr<td::td_api::messageVideoNote> content, QObject *parent)
    : QObject(parent)
{
    m_isViewed = content->is_viewed_;
    m_isSecret = content->is_secret_;
}

bool MessageVideoNote::isViewed() const
{
    return m_isViewed;
}

bool MessageVideoNote::isSecret() const
{
    return m_isSecret;
}

MessageVoiceNote::MessageVoiceNote(td::td_api::object_ptr<td::td_api::messageVoiceNote> content, QObject *parent)
    : QObject(parent)
{
    m_caption = QString::fromStdString(content->caption_->text_);
    m_isListened = content->is_listened_;
}

QString MessageVoiceNote::caption() const
{
    return m_caption;
}

bool MessageVoiceNote::isListened() const
{
    return m_isListened;
}

MessageLocation::MessageLocation(td::td_api::object_ptr<td::td_api::messageLocation> content, QObject *parent)
    : QObject(parent)
{
    // m_location = QString::fromStdString(content->location_->address_);
    m_livePeriod = content->live_period_;
    m_expiresIn = content->expires_in_;
    m_heading = content->heading_;
    m_proximityAlertRadius = content->proximity_alert_radius_;
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
    m_contact = QString::fromStdString(content->contact_->phone_number_);
}

QString MessageContact::contact() const
{
    return m_contact;
}

MessageAnimatedEmoji::MessageAnimatedEmoji(td::td_api::object_ptr<td::td_api::messageAnimatedEmoji> content, QObject *parent)
    : QObject(parent)
{
    m_emoji = QString::fromStdString(content->emoji_);
}

QString MessageAnimatedEmoji::emoji() const
{
    return m_emoji;
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
    // m_productInfo = QString::fromStdString(content->product_info_->description_);
    m_currency = QString::fromStdString(content->currency_);
    m_totalAmount = content->total_amount_;
    m_startParameter = QString::fromStdString(content->start_parameter_);
    m_isTest = content->is_test_;
    m_needShippingAddress = content->need_shipping_address_;
    m_receiptMessageId = content->receipt_message_id_;
    if (content->extended_media_)
    {
        // m_extendedMedia = QString::fromStdString(content->extended_media_->type_);
    }
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

MessageCall::MessageCall(td::td_api::object_ptr<td::td_api::messageCall> content, QObject *parent)
    : QObject(parent)
{
    m_isVideo = content->is_video_;

    if (content->discard_reason_->get_id() == td::td_api::callDiscardReasonEmpty::ID)
    {
        m_discardReason = DiscardReason::Empty;
    }
    else if (content->discard_reason_->get_id() == td::td_api::callDiscardReasonMissed::ID)
    {
        m_discardReason = DiscardReason::Missed;
    }
    else if (content->discard_reason_->get_id() == td::td_api::callDiscardReasonDeclined::ID)
    {
        m_discardReason = DiscardReason::Declined;
    }
    else if (content->discard_reason_->get_id() == td::td_api::callDiscardReasonDisconnected::ID)
    {
        m_discardReason = DiscardReason::Disconnected;
    }
    else if (content->discard_reason_->get_id() == td::td_api::callDiscardReasonHungUp::ID)
    {
        m_discardReason = DiscardReason::HungUp;
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

MessageService::MessageService(td::td_api::object_ptr<td::td_api::MessageContent> content, QObject *parent)
    : QObject(parent)
{
    switch (content->get_id())
    {
        case td::td_api::messageExpiredPhoto::ID:
            break;
        case td::td_api::messageExpiredVideo::ID:
            break;
        case td::td_api::messageBasicGroupChatCreate::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageBasicGroupChatCreate>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            m_addedMembers.clear();
            for (const auto &member : message->member_user_ids_)
            {
                m_addedMembers << member;
            }
            break;
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageSupergroupChatCreate>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            break;
        }
        case td::td_api::messageChatChangeTitle::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatChangeTitle>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            break;
        }
        case td::td_api::messageChatChangePhoto::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatChangePhoto>(content);
            // m_groupPhoto = QString::fromStdString(message->photo_->sizes_[0]->photo_->file_name_);
            break;
        }
        case td::td_api::messageChatDeletePhoto::ID: {
            m_groupPhoto.clear();
            break;
        }
        case td::td_api::messageChatAddMembers::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatAddMembers>(content);
            m_addedMembers.clear();
            for (auto member : message->member_user_ids_)
            {
                m_addedMembers << member;
            }
            break;
        }
        case td::td_api::messageChatJoinByLink::ID: {
            break;
        }
        case td::td_api::messageChatJoinByRequest::ID: {
            break;
        }
        case td::td_api::messageChatDeleteMember::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatDeleteMember>(content);
            m_removedMember = message->user_id_;
            break;
        }
        case td::td_api::messageChatUpgradeTo::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatUpgradeTo>(content);
            m_upgradedToSupergroup = message->supergroup_id_;
            break;
        }
        case td::td_api::messageChatUpgradeFrom::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatUpgradeFrom>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            m_upgradedFromGroup = message->basic_group_id_;
            break;
        }
        case td::td_api::messagePinMessage::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messagePinMessage>(content);
            m_pinnedMessage = message->message_id_;
            break;
        }
        case td::td_api::messageScreenshotTaken::ID: {
            break;
        }
        case td::td_api::messageChatSetBackground::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatSetBackground>(content);
            // m_background = QString::fromStdString(message->background_->file_->file_name_);
            break;
        }
        case td::td_api::messageChatSetTheme::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatSetTheme>(content);
            m_theme = QString::fromStdString(message->theme_name_);
            break;
        }
        case td::td_api::messageChatSetMessageAutoDeleteTime::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatSetMessageAutoDeleteTime>(content);
            m_autoDeleteTime = message->message_auto_delete_time_;
            m_fromUserId = message->from_user_id_;
            break;
        }
        case td::td_api::messageChatBoost::ID: {
            break;
        }
        case td::td_api::messageForumTopicCreated::ID: {
            break;
        }
        case td::td_api::messageForumTopicEdited::ID: {
            break;
        }
        case td::td_api::messageForumTopicIsClosedToggled::ID: {
            break;
        }
        case td::td_api::messageForumTopicIsHiddenToggled::ID: {
            break;
        }
        case td::td_api::messageSuggestProfilePhoto::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageSuggestProfilePhoto>(content);
            // m_profilePhotoSuggestion = QString::fromStdString(message->suggested_photo_->sizes_[0]->photo_->file_name_);
            break;
        }
        case td::td_api::messageCustomServiceAction::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageCustomServiceAction>(content);
            m_customAction = QString::fromStdString(message->text_);
            break;
        }
        case td::td_api::messageContactRegistered::ID: {
            break;
        }
        case td::td_api::messageInviteVideoChatParticipants::ID: {
            break;
        }
        case td::td_api::messageVideoChatScheduled::ID: {
            break;
        }
        case td::td_api::messageVideoChatStarted::ID: {
            break;
        }
        case td::td_api::messageVideoChatEnded::ID: {
            break;
        }
        case td::td_api::messageGiftedPremium::ID: {
            break;
        }
        case td::td_api::messagePremiumGiftCode::ID: {
            break;
        }
        case td::td_api::messagePaymentSuccessful::ID: {
            break;
        }
        case td::td_api::messagePaymentSuccessfulBot::ID: {
            break;
        }
        case td::td_api::messageUsersShared::ID: {
            break;
        }
        case td::td_api::messageChatShared::ID: {
            break;
        }
        case td::td_api::messageBotWriteAccessAllowed::ID: {
            break;
        }
        case td::td_api::messageWebAppDataSent::ID: {
            break;
        }
        case td::td_api::messageWebAppDataReceived::ID: {
            break;
        }
        case td::td_api::messagePassportDataSent::ID: {
            break;
        }
        case td::td_api::messagePassportDataReceived::ID: {
            break;
        }
        case td::td_api::messageProximityAlertTriggered::ID: {
            break;
        }
        case td::td_api::messageUnsupported::ID: {
            break;
        }
        default: {
            break;
        }
    }
}

QString MessageService::groupTitle() const
{
    return m_groupTitle;
}

QString MessageService::groupPhoto() const
{
    return m_groupPhoto;
}

QList<qlonglong> MessageService::addedMembers() const
{
    return m_addedMembers;
}

qlonglong MessageService::removedMember() const
{
    return m_removedMember;
}

qlonglong MessageService::upgradedToSupergroup() const
{
    return m_upgradedToSupergroup;
}

qlonglong MessageService::upgradedFromGroup() const
{
    return m_upgradedFromGroup;
}

qlonglong MessageService::pinnedMessage() const
{
    return m_pinnedMessage;
}

QString MessageService::background() const
{
    return m_background;
}

QString MessageService::theme() const
{
    return m_theme;
}

int MessageService::autoDeleteTime() const
{
    return m_autoDeleteTime;
}

QString MessageService::customAction() const
{
    return m_customAction;
}

QString MessageService::profilePhotoSuggestion() const
{
    return m_profilePhotoSuggestion;
}
