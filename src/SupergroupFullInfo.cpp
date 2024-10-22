#include "SupergroupFullInfo.hpp"

SupergroupFullInfo::SupergroupFullInfo(td::td_api::object_ptr<td::td_api::supergroupFullInfo> groupFullInfo, QObject *parent)
    : QObject(parent)
{
    if (groupFullInfo->photo_)
    {
        m_photo = QVariantMap();
    }

    m_description = QString::fromStdString(groupFullInfo->description_);
    m_memberCount = groupFullInfo->member_count_;
    m_administratorCount = groupFullInfo->administrator_count_;
    m_restrictedCount = groupFullInfo->restricted_count_;
    m_bannedCount = groupFullInfo->banned_count_;
    m_linkedChatId = groupFullInfo->linked_chat_id_;
    m_slowModeDelay = groupFullInfo->slow_mode_delay_;
    m_slowModeDelayExpiresIn = groupFullInfo->slow_mode_delay_expires_in_;
    m_canGetMembers = groupFullInfo->can_get_members_;
    m_hasHiddenMembers = groupFullInfo->has_hidden_members_;
    m_canHideMembers = groupFullInfo->can_hide_members_;
    m_canSetStickerSet = groupFullInfo->can_set_sticker_set_;
    m_canSetLocation = groupFullInfo->can_set_location_;
    m_canGetStatistics = groupFullInfo->can_get_statistics_;
    m_canGetRevenueStatistics = groupFullInfo->can_get_revenue_statistics_;
    m_canToggleAggressiveAntiSpam = groupFullInfo->can_toggle_aggressive_anti_spam_;
    m_isAllHistoryAvailable = groupFullInfo->is_all_history_available_;
    m_canHaveSponsoredMessages = groupFullInfo->can_have_sponsored_messages_;
    m_hasAggressiveAntiSpamEnabled = groupFullInfo->has_aggressive_anti_spam_enabled_;
    m_hasPinnedStories = groupFullInfo->has_pinned_stories_;
    m_myBoostCount = groupFullInfo->my_boost_count_;
    m_unrestrictBoostCount = groupFullInfo->unrestrict_boost_count_;
    m_stickerSetId = groupFullInfo->sticker_set_id_;
    m_customEmojiStickerSetId = groupFullInfo->custom_emoji_sticker_set_id_;
    m_upgradedFromBasicGroupId = groupFullInfo->upgraded_from_basic_group_id_;
    m_upgradedFromMaxMessageId = groupFullInfo->upgraded_from_max_message_id_;
}

QVariantMap SupergroupFullInfo::photo() const
{
    return m_photo;
}

QString SupergroupFullInfo::description() const
{
    return m_description;
}

int SupergroupFullInfo::memberCount() const
{
    return m_memberCount;
}

int SupergroupFullInfo::administratorCount() const
{
    return m_administratorCount;
}

int SupergroupFullInfo::restrictedCount() const
{
    return m_restrictedCount;
}

int SupergroupFullInfo::bannedCount() const
{
    return m_bannedCount;
}

qlonglong SupergroupFullInfo::linkedChatId() const
{
    return m_linkedChatId;
}

int SupergroupFullInfo::slowModeDelay() const
{
    return m_slowModeDelay;
}

double SupergroupFullInfo::slowModeDelayExpiresIn() const
{
    return m_slowModeDelayExpiresIn;
}

bool SupergroupFullInfo::canGetMembers() const
{
    return m_canGetMembers;
}

bool SupergroupFullInfo::hasHiddenMembers() const
{
    return m_hasHiddenMembers;
}

bool SupergroupFullInfo::canHideMembers() const
{
    return m_canHideMembers;
}

bool SupergroupFullInfo::canSetStickerSet() const
{
    return m_canSetStickerSet;
}

bool SupergroupFullInfo::canSetLocation() const
{
    return m_canSetLocation;
}

bool SupergroupFullInfo::canGetStatistics() const
{
    return m_canGetStatistics;
}

bool SupergroupFullInfo::canGetRevenueStatistics() const
{
    return m_canGetRevenueStatistics;
}

bool SupergroupFullInfo::canToggleAggressiveAntiSpam() const
{
    return m_canToggleAggressiveAntiSpam;
}

bool SupergroupFullInfo::isAllHistoryAvailable() const
{
    return m_isAllHistoryAvailable;
}

bool SupergroupFullInfo::canHaveSponsoredMessages() const
{
    return m_canHaveSponsoredMessages;
}

bool SupergroupFullInfo::hasAggressiveAntiSpamEnabled() const
{
    return m_hasAggressiveAntiSpamEnabled;
}

bool SupergroupFullInfo::hasPinnedStories() const
{
    return m_hasPinnedStories;
}

int SupergroupFullInfo::myBoostCount() const
{
    return m_myBoostCount;
}

int SupergroupFullInfo::unrestrictBoostCount() const
{
    return m_unrestrictBoostCount;
}

qlonglong SupergroupFullInfo::stickerSetId() const
{
    return m_stickerSetId;
}

qlonglong SupergroupFullInfo::customEmojiStickerSetId() const
{
    return m_customEmojiStickerSetId;
}

QVariantMap SupergroupFullInfo::location() const
{
    return m_location;
}

QVariantMap SupergroupFullInfo::inviteLink() const
{
    return m_inviteLink;
}

QVariantList SupergroupFullInfo::botCommands() const
{
    return m_botCommands;
}

qlonglong SupergroupFullInfo::upgradedFromBasicGroupId() const
{
    return m_upgradedFromBasicGroupId;
}

qlonglong SupergroupFullInfo::upgradedFromMaxMessageId() const
{
    return m_upgradedFromMaxMessageId;
}
