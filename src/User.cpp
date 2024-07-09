#include "User.hpp"

User::User(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_accentColorId(0)
    , m_backgroundCustomEmojiId(0)
    , m_profileAccentColorId(0)
    , m_profileBackgroundCustomEmojiId(0)
    , m_isContact(false)
    , m_isMutualContact(false)
    , m_isCloseFriend(false)
    , m_isVerified(false)
    , m_isPremium(false)
    , m_isSupport(false)
    , m_isScam(false)
    , m_isFake(false)
    , m_hasActiveStories(false)
    , m_hasUnreadActiveStories(false)
    , m_restrictsNewChats(false)
    , m_haveAccess(true)
    ,  // Assuming default accessible
    m_addedToAttachmentMenu(false)
{
}

qint64 User::id() const
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

qint32 User::accentColorId() const
{
    return m_accentColorId;
}

qint64 User::backgroundCustomEmojiId() const
{
    return m_backgroundCustomEmojiId;
}

qint32 User::profileAccentColorId() const
{
    return m_profileAccentColorId;
}

qint64 User::profileBackgroundCustomEmojiId() const
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

void User::setId(qint64 id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}

void User::setFirstName(const QString &firstName)
{
    if (m_firstName != firstName)
    {
        m_firstName = firstName;
        emit firstNameChanged();
    }
}

void User::setLastName(const QString &lastName)
{
    if (m_lastName != lastName)
    {
        m_lastName = lastName;
        emit lastNameChanged();
    }
}

void User::setUsernames(const QVariantMap &usernames)
{
    if (m_usernames != usernames)
    {
        m_usernames = usernames;
        emit usernamesChanged();
    }
}

void User::setPhoneNumber(const QString &phoneNumber)
{
    if (m_phoneNumber != phoneNumber)
    {
        m_phoneNumber = phoneNumber;
        emit phoneNumberChanged();
    }
}

void User::setStatus(const QVariantMap &status)
{
    if (m_status != status)
    {
        m_status = status;
        emit statusChanged();
    }
}

void User::setProfilePhoto(const QVariantMap &profilePhoto)
{
    if (m_profilePhoto != profilePhoto)
    {
        m_profilePhoto = profilePhoto;
        emit profilePhotoChanged();
    }
}

void User::setAccentColorId(qint32 accentColorId)
{
    if (m_accentColorId != accentColorId)
    {
        m_accentColorId = accentColorId;
        emit accentColorIdChanged();
    }
}

void User::setBackgroundCustomEmojiId(qint64 backgroundCustomEmojiId)
{
    if (m_backgroundCustomEmojiId != backgroundCustomEmojiId)
    {
        m_backgroundCustomEmojiId = backgroundCustomEmojiId;
        emit backgroundCustomEmojiIdChanged();
    }
}

void User::setProfileAccentColorId(qint32 profileAccentColorId)
{
    if (m_profileAccentColorId != profileAccentColorId)
    {
        m_profileAccentColorId = profileAccentColorId;
        emit profileAccentColorIdChanged();
    }
}

void User::setProfileBackgroundCustomEmojiId(qint64 profileBackgroundCustomEmojiId)
{
    if (m_profileBackgroundCustomEmojiId != profileBackgroundCustomEmojiId)
    {
        m_profileBackgroundCustomEmojiId = profileBackgroundCustomEmojiId;
        emit profileBackgroundCustomEmojiIdChanged();
    }
}

void User::setEmojiStatus(const QVariantMap &emojiStatus)
{
    if (m_emojiStatus != emojiStatus)
    {
        m_emojiStatus = emojiStatus;
        emit emojiStatusChanged();
    }
}

void User::setIsContact(bool isContact)
{
    if (m_isContact != isContact)
    {
        m_isContact = isContact;
        emit isContactChanged();
    }
}

void User::setIsMutualContact(bool isMutualContact)
{
    if (m_isMutualContact != isMutualContact)
    {
        m_isMutualContact = isMutualContact;
        emit isMutualContactChanged();
    }
}

void User::setIsCloseFriend(bool isCloseFriend)
{
    if (m_isCloseFriend != isCloseFriend)
    {
        m_isCloseFriend = isCloseFriend;
        emit isCloseFriendChanged();
    }
}

void User::setIsVerified(bool isVerified)
{
    if (m_isVerified != isVerified)
    {
        m_isVerified = isVerified;
        emit isVerifiedChanged();
    }
}

void User::setIsPremium(bool isPremium)
{
    if (m_isPremium != isPremium)
    {
        m_isPremium = isPremium;
        emit isPremiumChanged();
    }
}

void User::setIsSupport(bool isSupport)
{
    if (m_isSupport != isSupport)
    {
        m_isSupport = isSupport;
        emit isSupportChanged();
    }
}

void User::setRestrictionReason(const QString &restrictionReason)
{
    if (m_restrictionReason != restrictionReason)
    {
        m_restrictionReason = restrictionReason;
        emit restrictionReasonChanged();
    }
}

void User::setIsScam(bool isScam)
{
    if (m_isScam != isScam)
    {
        m_isScam = isScam;
        emit isScamChanged();
    }
}

void User::setIsFake(bool isFake)
{
    if (m_isFake != isFake)
    {
        m_isFake = isFake;
        emit isFakeChanged();
    }
}

void User::setHasActiveStories(bool hasActiveStories)
{
    if (m_hasActiveStories != hasActiveStories)
    {
        m_hasActiveStories = hasActiveStories;
        emit hasActiveStoriesChanged();
    }
}

void User::setHasUnreadActiveStories(bool hasUnreadActiveStories)
{
    if (m_hasUnreadActiveStories != hasUnreadActiveStories)
    {
        m_hasUnreadActiveStories = hasUnreadActiveStories;
        emit hasUnreadActiveStoriesChanged();
    }
}

void User::setRestrictsNewChats(bool restrictsNewChats)
{
    if (m_restrictsNewChats != restrictsNewChats)
    {
        m_restrictsNewChats = restrictsNewChats;
        emit restrictsNewChatsChanged();
    }
}

void User::setHaveAccess(bool haveAccess)
{
    if (m_haveAccess != haveAccess)
    {
        m_haveAccess = haveAccess;
        emit haveAccessChanged();
    }
}

void User::setType(const QVariantMap &type)
{
    if (m_type != type)
    {
        m_type = type;
        emit typeChanged();
    }
}

void User::setLanguageCode(const QString &languageCode)
{
    if (m_languageCode != languageCode)
    {
        m_languageCode = languageCode;
        emit languageCodeChanged();
    }
}

void User::setAddedToAttachmentMenu(bool addedToAttachmentMenu)
{
    if (m_addedToAttachmentMenu != addedToAttachmentMenu)
    {
        m_addedToAttachmentMenu = addedToAttachmentMenu;
        emit addedToAttachmentMenuChanged();
    }
}

void User::setFromVariantMap(const QVariantMap &map)
{
    m_id = map.value("id").toLongLong();
    m_firstName = map.value("first_name").toString();
    m_lastName = map.value("last_name").toString();
    m_usernames = map.value("usernames").toMap();
    m_phoneNumber = map.value("phone_number").toString();
    m_status = map.value("status").toMap();
    m_profilePhoto = map.value("profile_photo").toMap();
    m_accentColorId = map.value("accent_color_id").toInt();
    m_backgroundCustomEmojiId = map.value("background_custom_emoji_id").toLongLong();
    m_profileAccentColorId = map.value("profile_accent_color_id").toInt();
    m_profileBackgroundCustomEmojiId = map.value("profile_background_custom_emoji_id").toLongLong();
    m_emojiStatus = map.value("emoji_status").toMap();
    m_isContact = map.value("is_contact").toBool();
    m_isMutualContact = map.value("is_mutual_contact").toBool();
    m_isCloseFriend = map.value("is_close_friend").toBool();
    m_isVerified = map.value("is_verified").toBool();
    m_isPremium = map.value("is_premium").toBool();
    m_isSupport = map.value("is_support").toBool();
    m_restrictionReason = map.value("restriction_reason").toString();
    m_isScam = map.value("is_scam").toBool();
    m_isFake = map.value("is_fake").toBool();
    m_hasActiveStories = map.value("has_active_stories").toBool();
    m_hasUnreadActiveStories = map.value("has_unread_active_stories").toBool();
    m_restrictsNewChats = map.value("restricts_new_chats").toBool();
    m_haveAccess = map.value("have_access").toBool();
    m_type = map.value("type").toMap();
    m_languageCode = map.value("language_code").toString();
    m_addedToAttachmentMenu = map.value("added_to_attachment_menu").toBool();
}
