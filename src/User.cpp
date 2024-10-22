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
    , m_type(determineType(userFullInfo->type_))
{
    if (userFullInfo->usernames_)
    {
        for (const auto &activeUsername : userFullInfo->usernames_->active_usernames_)
        {
            m_activeUsernames.append(QString::fromStdString(activeUsername));
        }
    }

    setStatus(std::move(userFullInfo->status_));
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

User::Status User::status() const
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

User::Type User::type() const
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

QDateTime User::wasOnline() const
{
    return m_wasOnline;
}

QStringList User::activeUsernames() const
{
    return m_activeUsernames;
}

void User::setStatus(td::td_api::object_ptr<td::td_api::UserStatus> status)
{
    if (!status)
    {
        m_status = Status::Empty;
        emit userInfoChanged();
        return;
    }

    switch (status->get_id())
    {
        case td::td_api::userStatusEmpty::ID:
            m_status = Status::Empty;
            break;
        case td::td_api::userStatusOnline::ID:
            m_status = Status::Online;
            break;
        case td::td_api::userStatusOffline::ID: {
            const auto *offlineStatus = static_cast<const td::td_api::userStatusOffline *>(status.get());
            m_wasOnline = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(offlineStatus->was_online_) * 1000);
            m_status = Status::Offline;
            break;
        }
        case td::td_api::userStatusRecently::ID:
            m_status = Status::Recently;
            break;
        case td::td_api::userStatusLastWeek::ID:
            m_status = Status::LastWeek;
            break;
        case td::td_api::userStatusLastMonth::ID:
            m_status = Status::LastMonth;
            break;
        default:
            m_status = Status::Empty;
            break;
    }

    emit userInfoChanged();
}

User::Type User::determineType(const td::td_api::object_ptr<td::td_api::UserType> &type) const
{
    if (!type)
        return Type::Unknown;

    switch (type->get_id())
    {
        case td::td_api::userTypeBot::ID:
            return Type::Bot;
        case td::td_api::userTypeDeleted::ID:
            return Type::Deleted;
        case td::td_api::userTypeRegular::ID:
            return Type::Regular;
        case td::td_api::userTypeUnknown::ID:
            return Type::Unknown;
        default:
            return Type::Unknown;
    }
}
