#include "UserFullInfo.hpp"

UserFullInfo::UserFullInfo(QObject *parent)
    : QObject(parent)
    , m_canBeCalled(false)
    , m_supportsVideoCalls(false)
    , m_hasPrivateCalls(false)
    , m_hasPrivateForwards(false)
    , m_hasRestrictedVoiceAndVideoNoteMessages(false)
    , m_hasPostedToProfileStories(false)
    , m_hasSponsoredMessagesEnabled(false)
    , m_needPhoneNumberPrivacyException(false)
    , m_setChatBackground(false)
    , m_personalChatId(0)
    , m_groupInCommonCount(0)
{
}

QVariantMap UserFullInfo::personalPhoto() const
{
    return m_personalPhoto;
}

void UserFullInfo::setPersonalPhoto(const QVariantMap &personalPhoto)
{
    if (m_personalPhoto != personalPhoto)
    {
        m_personalPhoto = personalPhoto;
        emit personalPhotoChanged();
    }
}

QVariantMap UserFullInfo::photo() const
{
    return m_photo;
}

void UserFullInfo::setPhoto(const QVariantMap &photo)
{
    if (m_photo != photo)
    {
        m_photo = photo;
        emit photoChanged();
    }
}

QVariantMap UserFullInfo::publicPhoto() const
{
    return m_publicPhoto;
}

void UserFullInfo::setPublicPhoto(const QVariantMap &publicPhoto)
{
    if (m_publicPhoto != publicPhoto)
    {
        m_publicPhoto = publicPhoto;
        emit publicPhotoChanged();
    }
}

QVariantMap UserFullInfo::blockList() const
{
    return m_blockList;
}

void UserFullInfo::setBlockList(const QVariantMap &blockList)
{
    if (m_blockList != blockList)
    {
        m_blockList = blockList;
        emit blockListChanged();
    }
}

bool UserFullInfo::canBeCalled() const
{
    return m_canBeCalled;
}

void UserFullInfo::setCanBeCalled(bool canBeCalled)
{
    if (m_canBeCalled != canBeCalled)
    {
        m_canBeCalled = canBeCalled;
        emit canBeCalledChanged();
    }
}

bool UserFullInfo::supportsVideoCalls() const
{
    return m_supportsVideoCalls;
}

void UserFullInfo::setSupportsVideoCalls(bool supportsVideoCalls)
{
    if (m_supportsVideoCalls != supportsVideoCalls)
    {
        m_supportsVideoCalls = supportsVideoCalls;
        emit supportsVideoCallsChanged();
    }
}

bool UserFullInfo::hasPrivateCalls() const
{
    return m_hasPrivateCalls;
}

void UserFullInfo::setHasPrivateCalls(bool hasPrivateCalls)
{
    if (m_hasPrivateCalls != hasPrivateCalls)
    {
        m_hasPrivateCalls = hasPrivateCalls;
        emit hasPrivateCallsChanged();
    }
}

bool UserFullInfo::hasPrivateForwards() const
{
    return m_hasPrivateForwards;
}

void UserFullInfo::setHasPrivateForwards(bool hasPrivateForwards)
{
    if (m_hasPrivateForwards != hasPrivateForwards)
    {
        m_hasPrivateForwards = hasPrivateForwards;
        emit hasPrivateForwardsChanged();
    }
}

bool UserFullInfo::hasRestrictedVoiceAndVideoNoteMessages() const
{
    return m_hasRestrictedVoiceAndVideoNoteMessages;
}

void UserFullInfo::setHasRestrictedVoiceAndVideoNoteMessages(bool hasRestrictedVoiceAndVideoNoteMessages)
{
    if (m_hasRestrictedVoiceAndVideoNoteMessages != hasRestrictedVoiceAndVideoNoteMessages)
    {
        m_hasRestrictedVoiceAndVideoNoteMessages = hasRestrictedVoiceAndVideoNoteMessages;
        emit hasRestrictedVoiceAndVideoNoteMessagesChanged();
    }
}

bool UserFullInfo::hasPostedToProfileStories() const
{
    return m_hasPostedToProfileStories;
}

void UserFullInfo::setHasPostedToProfileStories(bool hasPostedToProfileStories)
{
    if (m_hasPostedToProfileStories != hasPostedToProfileStories)
    {
        m_hasPostedToProfileStories = hasPostedToProfileStories;
        emit hasPostedToProfileStoriesChanged();
    }
}

bool UserFullInfo::hasSponsoredMessagesEnabled() const
{
    return m_hasSponsoredMessagesEnabled;
}

void UserFullInfo::setHasSponsoredMessagesEnabled(bool hasSponsoredMessagesEnabled)
{
    if (m_hasSponsoredMessagesEnabled != hasSponsoredMessagesEnabled)
    {
        m_hasSponsoredMessagesEnabled = hasSponsoredMessagesEnabled;
        emit hasSponsoredMessagesEnabledChanged();
    }
}

bool UserFullInfo::needPhoneNumberPrivacyException() const
{
    return m_needPhoneNumberPrivacyException;
}

void UserFullInfo::setNeedPhoneNumberPrivacyException(bool needPhoneNumberPrivacyException)
{
    if (m_needPhoneNumberPrivacyException != needPhoneNumberPrivacyException)
    {
        m_needPhoneNumberPrivacyException = needPhoneNumberPrivacyException;
        emit needPhoneNumberPrivacyExceptionChanged();
    }
}

bool UserFullInfo::setChatBackground() const
{
    return m_setChatBackground;
}

void UserFullInfo::setSetChatBackground(bool setChatBackground)
{
    if (m_setChatBackground != setChatBackground)
    {
        m_setChatBackground = setChatBackground;
        emit setChatBackgroundChanged();
    }
}

QVariantMap UserFullInfo::bio() const
{
    return m_bio;
}

void UserFullInfo::setBio(const QVariantMap &bio)
{
    if (m_bio != bio)
    {
        m_bio = bio;
        emit bioChanged();
    }
}

QVariantMap UserFullInfo::birthdate() const
{
    return m_birthdate;
}

void UserFullInfo::setBirthdate(const QVariantMap &birthdate)
{
    if (m_birthdate != birthdate)
    {
        m_birthdate = birthdate;
        emit birthdateChanged();
    }
}

qlonglong UserFullInfo::personalChatId() const
{
    return m_personalChatId;
}

void UserFullInfo::setPersonalChatId(qlonglong personalChatId)
{
    if (m_personalChatId != personalChatId)
    {
        m_personalChatId = personalChatId;
        emit personalChatIdChanged();
    }
}

QVariantList UserFullInfo::premiumGiftOptions() const
{
    return m_premiumGiftOptions;
}

void UserFullInfo::setPremiumGiftOptions(const QVariantList &premiumGiftOptions)
{
    if (m_premiumGiftOptions != premiumGiftOptions)
    {
        m_premiumGiftOptions = premiumGiftOptions;
        emit premiumGiftOptionsChanged();
    }
}

int UserFullInfo::groupInCommonCount() const
{
    return m_groupInCommonCount;
}

void UserFullInfo::setGroupInCommonCount(int groupInCommonCount)
{
    if (m_groupInCommonCount != groupInCommonCount)
    {
        m_groupInCommonCount = groupInCommonCount;
        emit groupInCommonCountChanged();
    }
}

QVariantMap UserFullInfo::businessInfo() const
{
    return m_businessInfo;
}

void UserFullInfo::setBusinessInfo(const QVariantMap &businessInfo)
{
    if (m_businessInfo != businessInfo)
    {
        m_businessInfo = businessInfo;
        emit businessInfoChanged();
    }
}

QVariantMap UserFullInfo::botInfo() const
{
    return m_botInfo;
}

void UserFullInfo::setBotInfo(const QVariantMap &botInfo)
{
    if (m_botInfo != botInfo)
    {
        m_botInfo = botInfo;
        emit botInfoChanged();
    }
}

void UserFullInfo::setFromVariantMap(const QVariantMap &map)
{
    if (map.contains("personal_photo"))
        setPersonalPhoto(map["personal_photo"].toMap());
    if (map.contains("photo"))
        setPhoto(map["photo"].toMap());
    if (map.contains("public_photo"))
        setPublicPhoto(map["public_photo"].toMap());
    if (map.contains("block_list"))
        setBlockList(map["block_list"].toMap());
    if (map.contains("can_be_called"))
        setCanBeCalled(map["can_be_called"].toBool());
    if (map.contains("supports_video_calls"))
        setSupportsVideoCalls(map["supports_video_calls"].toBool());
    if (map.contains("has_private_calls"))
        setHasPrivateCalls(map["has_private_calls"].toBool());
    if (map.contains("has_private_forwards"))
        setHasPrivateForwards(map["has_private_forwards"].toBool());
    if (map.contains("has_restricted_voice_and_video_note_messages"))
        setHasRestrictedVoiceAndVideoNoteMessages(map["has_restricted_voice_and_video_note_messages"].toBool());
    if (map.contains("has_posted_to_profile_stories"))
        setHasPostedToProfileStories(map["has_posted_to_profile_stories"].toBool());
    if (map.contains("has_sponsored_messages_enabled"))
        setHasSponsoredMessagesEnabled(map["has_sponsored_messages_enabled"].toBool());
    if (map.contains("need_phone_number_privacy_exception"))
        setNeedPhoneNumberPrivacyException(map["need_phone_number_privacy_exception"].toBool());
    if (map.contains("set_chat_background"))
        setSetChatBackground(map["set_chat_background"].toBool());
    if (map.contains("bio"))
        setBio(map["bio"].toMap());
    if (map.contains("birthdate"))
        setBirthdate(map["birthdate"].toMap());
    if (map.contains("personal_chat_id"))
        setPersonalChatId(map["personal_chat_id"].toLongLong());
    if (map.contains("premium_gift_options"))
        setPremiumGiftOptions(map["premium_gift_options"].toList());
    if (map.contains("group_in_common_count"))
        setGroupInCommonCount(map["group_in_common_count"].toInt());
    if (map.contains("business_info"))
        setBusinessInfo(map["business_info"].toMap());
    if (map.contains("bot_info"))
        setBotInfo(map["bot_info"].toMap());
}
