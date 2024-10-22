#include "UserFullInfo.hpp"

UserFullInfo::UserFullInfo(td::td_api::object_ptr<td::td_api::userFullInfo> userFullInfo, QObject *parent)
    : QObject(parent)
    , m_canBeCalled(userFullInfo->can_be_called_)
    , m_supportsVideoCalls(userFullInfo->supports_video_calls_)
    , m_hasPrivateCalls(userFullInfo->has_private_calls_)
    , m_hasPrivateForwards(userFullInfo->has_private_forwards_)
    , m_hasRestrictedVoiceAndVideoNoteMessages(userFullInfo->has_restricted_voice_and_video_note_messages_)
    , m_hasPostedToProfileStories(userFullInfo->has_posted_to_profile_stories_)
    , m_hasSponsoredMessagesEnabled(userFullInfo->has_sponsored_messages_enabled_)
    , m_needPhoneNumberPrivacyException(userFullInfo->need_phone_number_privacy_exception_)
    , m_setChatBackground(userFullInfo->set_chat_background_)
    , m_personalChatId(userFullInfo->personal_chat_id_)
    , m_groupInCommonCount(userFullInfo->group_in_common_count_)
{
}

QVariantMap UserFullInfo::personalPhoto() const
{
    return m_personalPhoto;
}
QVariantMap UserFullInfo::photo() const
{
    return m_photo;
}
QVariantMap UserFullInfo::publicPhoto() const
{
    return m_publicPhoto;
}
QVariantMap UserFullInfo::blockList() const
{
    return m_blockList;
}
bool UserFullInfo::canBeCalled() const
{
    return m_canBeCalled;
}
bool UserFullInfo::supportsVideoCalls() const
{
    return m_supportsVideoCalls;
}
bool UserFullInfo::hasPrivateCalls() const
{
    return m_hasPrivateCalls;
}
bool UserFullInfo::hasPrivateForwards() const
{
    return m_hasPrivateForwards;
}
bool UserFullInfo::hasRestrictedVoiceAndVideoNoteMessages() const
{
    return m_hasRestrictedVoiceAndVideoNoteMessages;
}
bool UserFullInfo::hasPostedToProfileStories() const
{
    return m_hasPostedToProfileStories;
}
bool UserFullInfo::hasSponsoredMessagesEnabled() const
{
    return m_hasSponsoredMessagesEnabled;
}
bool UserFullInfo::needPhoneNumberPrivacyException() const
{
    return m_needPhoneNumberPrivacyException;
}
bool UserFullInfo::setChatBackground() const
{
    return m_setChatBackground;
}
QVariantMap UserFullInfo::bio() const
{
    return m_bio;
}
QVariantMap UserFullInfo::birthdate() const
{
    return m_birthdate;
}
qlonglong UserFullInfo::personalChatId() const
{
    return m_personalChatId;
}
QVariantList UserFullInfo::premiumGiftOptions() const
{
    return m_premiumGiftOptions;
}
int UserFullInfo::groupInCommonCount() const
{
    return m_groupInCommonCount;
}
QVariantMap UserFullInfo::businessInfo() const
{
    return m_businessInfo;
}
QVariantMap UserFullInfo::botInfo() const
{
    return m_botInfo;
}
