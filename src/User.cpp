#include "User.hpp"

User::User(td::td_api::object_ptr<td::td_api::user> userFullInfo, QObject *parent)
    : QObject(parent)
    , m_id(userFullInfo->id_)
    , m_firstName(QString::fromStdString(userFullInfo->first_name_))
    , m_lastName(QString::fromStdString(userFullInfo->last_name_))
    , m_phoneNumber(QString::fromStdString(userFullInfo->phone_number_))
    , m_accentColorId(userFullInfo->accent_color_id_)
    , m_backgroundCustomEmojiId(userFullInfo->background_custom_emoji_id_)
    , m_profileAccentColorId(userFullInfo->profile_accent_color_id_)
    , m_profileBackgroundCustomEmojiId(userFullInfo->profile_background_custom_emoji_id_)
    , m_isContact(userFullInfo->is_contact_)
    , m_isMutualContact(userFullInfo->is_mutual_contact_)
    , m_isCloseFriend(userFullInfo->is_close_friend_)
    , m_isVerified(userFullInfo->is_verified_)
    , m_isPremium(userFullInfo->is_premium_)
    , m_isSupport(userFullInfo->is_support_)
    , m_restrictionReason(QString::fromStdString(userFullInfo->restriction_reason_))
    , m_isScam(userFullInfo->is_scam_)
    , m_isFake(userFullInfo->is_fake_)
    , m_hasActiveStories(userFullInfo->has_active_stories_)
    , m_hasUnreadActiveStories(userFullInfo->has_unread_active_stories_)
    , m_restrictsNewChats(userFullInfo->restricts_new_chats_)
    , m_haveAccess(userFullInfo->have_access_)
    , m_addedToAttachmentMenu(userFullInfo->added_to_attachment_menu_)
{
}

qlonglong User::id() const
{
    return m_id;
}

QString User::firstName() const
{
    return m_firstName;
}

QString User::lastName() const
{
    return m_lastName;
}

QVariantMap User::usernames() const
{
    return m_usernames;
}

QString User::phoneNumber() const
{
    return m_phoneNumber;
}

QVariantMap User::status() const
{
    return m_status;
}

QVariantMap User::profilePhoto() const
{
    return m_profilePhoto;
}

int User::accentColorId() const
{
    return m_accentColorId;
}

qlonglong User::backgroundCustomEmojiId() const
{
    return m_backgroundCustomEmojiId;
}

int User::profileAccentColorId() const
{
    return m_profileAccentColorId;
}

qlonglong User::profileBackgroundCustomEmojiId() const
{
    return m_profileBackgroundCustomEmojiId;
}

QVariantMap User::emojiStatus() const
{
    return m_emojiStatus;
}

bool User::isContact() const
{
    return m_isContact;
}

bool User::isMutualContact() const
{
    return m_isMutualContact;
}

bool User::isCloseFriend() const
{
    return m_isCloseFriend;
}

bool User::isVerified() const
{
    return m_isVerified;
}

bool User::isPremium() const
{
    return m_isPremium;
}

bool User::isSupport() const
{
    return m_isSupport;
}

QString User::restrictionReason() const
{
    return m_restrictionReason;
}

bool User::isScam() const
{
    return m_isScam;
}

bool User::isFake() const
{
    return m_isFake;
}

bool User::hasActiveStories() const
{
    return m_hasActiveStories;
}

bool User::hasUnreadActiveStories() const
{
    return m_hasUnreadActiveStories;
}

bool User::restrictsNewChats() const
{
    return m_restrictsNewChats;
}

bool User::haveAccess() const
{
    return m_haveAccess;
}

QVariantMap User::type() const
{
    return m_type;
}

QString User::languageCode() const
{
    return m_languageCode;
}

bool User::addedToAttachmentMenu() const
{
    return m_addedToAttachmentMenu;
}
