#include "SupergroupFullInfo.hpp"

SupergroupFullInfo::SupergroupFullInfo(QObject *parent)
    : QObject(parent)
    , m_memberCount(0)
    , m_administratorCount(0)
    , m_restrictedCount(0)
    , m_bannedCount(0)
    , m_linkedChatId(0)
    , m_slowModeDelay(0)
    , m_slowModeDelayExpiresIn(0)
    , m_canGetMembers(false)
    , m_hasHiddenMembers(false)
    , m_canHideMembers(false)
    , m_canSetStickerSet(false)
    , m_canSetLocation(false)
    , m_canGetStatistics(false)
    , m_canGetRevenueStatistics(false)
    , m_canToggleAggressiveAntiSpam(false)
    , m_isAllHistoryAvailable(false)
    , m_canHaveSponsoredMessages(false)
    , m_hasAggressiveAntiSpamEnabled(false)
    , m_hasPinnedStories(false)
    , m_myBoostCount(0)
    , m_unrestrictBoostCount(0)
    , m_stickerSetId(0)
    , m_customEmojiStickerSetId(0)
    , m_upgradedFromBasicGroupId(0)
    , m_upgradedFromMaxMessageId(0)
{
}

QVariantMap SupergroupFullInfo::photo() const
{
    return m_photo;
}

void SupergroupFullInfo::setPhoto(const QVariantMap &photo)
{
    if (m_photo != photo)
    {
        m_photo = photo;
        emit photoChanged();
    }
}

QString SupergroupFullInfo::description() const
{
    return m_description;
}

void SupergroupFullInfo::setDescription(const QString &description)
{
    if (m_description != description)
    {
        m_description = description;
        emit descriptionChanged();
    }
}

int SupergroupFullInfo::memberCount() const
{
    return m_memberCount;
}

void SupergroupFullInfo::setMemberCount(int memberCount)
{
    if (m_memberCount != memberCount)
    {
        m_memberCount = memberCount;
        emit memberCountChanged();
    }
}

int SupergroupFullInfo::administratorCount() const
{
    return m_administratorCount;
}

void SupergroupFullInfo::setAdministratorCount(int administratorCount)
{
    if (m_administratorCount != administratorCount)
    {
        m_administratorCount = administratorCount;
        emit administratorCountChanged();
    }
}

int SupergroupFullInfo::restrictedCount() const
{
    return m_restrictedCount;
}

void SupergroupFullInfo::setRestrictedCount(int restrictedCount)
{
    if (m_restrictedCount != restrictedCount)
    {
        m_restrictedCount = restrictedCount;
        emit restrictedCountChanged();
    }
}

int SupergroupFullInfo::bannedCount() const
{
    return m_bannedCount;
}

void SupergroupFullInfo::setBannedCount(int bannedCount)
{
    if (m_bannedCount != bannedCount)
    {
        m_bannedCount = bannedCount;
        emit bannedCountChanged();
    }
}

qlonglong SupergroupFullInfo::linkedChatId() const
{
    return m_linkedChatId;
}

void SupergroupFullInfo::setLinkedChatId(qlonglong linkedChatId)
{
    if (m_linkedChatId != linkedChatId)
    {
        m_linkedChatId = linkedChatId;
        emit linkedChatIdChanged();
    }
}

int SupergroupFullInfo::slowModeDelay() const
{
    return m_slowModeDelay;
}

void SupergroupFullInfo::setSlowModeDelay(int slowModeDelay)
{
    if (m_slowModeDelay != slowModeDelay)
    {
        m_slowModeDelay = slowModeDelay;
        emit slowModeDelayChanged();
    }
}

double SupergroupFullInfo::slowModeDelayExpiresIn() const
{
    return m_slowModeDelayExpiresIn;
}

void SupergroupFullInfo::setSlowModeDelayExpiresIn(double slowModeDelayExpiresIn)
{
    if (m_slowModeDelayExpiresIn != slowModeDelayExpiresIn)
    {
        m_slowModeDelayExpiresIn = slowModeDelayExpiresIn;
        emit slowModeDelayExpiresInChanged();
    }
}

bool SupergroupFullInfo::canGetMembers() const
{
    return m_canGetMembers;
}

void SupergroupFullInfo::setCanGetMembers(bool canGetMembers)
{
    if (m_canGetMembers != canGetMembers)
    {
        m_canGetMembers = canGetMembers;
        emit canGetMembersChanged();
    }
}

bool SupergroupFullInfo::hasHiddenMembers() const
{
    return m_hasHiddenMembers;
}

void SupergroupFullInfo::setHasHiddenMembers(bool hasHiddenMembers)
{
    if (m_hasHiddenMembers != hasHiddenMembers)
    {
        m_hasHiddenMembers = hasHiddenMembers;
        emit hasHiddenMembersChanged();
    }
}

bool SupergroupFullInfo::canHideMembers() const
{
    return m_canHideMembers;
}

void SupergroupFullInfo::setCanHideMembers(bool canHideMembers)
{
    if (m_canHideMembers != canHideMembers)
    {
        m_canHideMembers = canHideMembers;
        emit canHideMembersChanged();
    }
}

bool SupergroupFullInfo::canSetStickerSet() const
{
    return m_canSetStickerSet;
}

void SupergroupFullInfo::setCanSetStickerSet(bool canSetStickerSet)
{
    if (m_canSetStickerSet != canSetStickerSet)
    {
        m_canSetStickerSet = canSetStickerSet;
        emit canSetStickerSetChanged();
    }
}

bool SupergroupFullInfo::canSetLocation() const
{
    return m_canSetLocation;
}

void SupergroupFullInfo::setCanSetLocation(bool canSetLocation)
{
    if (m_canSetLocation != canSetLocation)
    {
        m_canSetLocation = canSetLocation;
        emit canSetLocationChanged();
    }
}

bool SupergroupFullInfo::canGetStatistics() const
{
    return m_canGetStatistics;
}

void SupergroupFullInfo::setCanGetStatistics(bool canGetStatistics)
{
    if (m_canGetStatistics != canGetStatistics)
    {
        m_canGetStatistics = canGetStatistics;
        emit canGetStatisticsChanged();
    }
}

bool SupergroupFullInfo::canGetRevenueStatistics() const
{
    return m_canGetRevenueStatistics;
}

void SupergroupFullInfo::setCanGetRevenueStatistics(bool canGetRevenueStatistics)
{
    if (m_canGetRevenueStatistics != canGetRevenueStatistics)
    {
        m_canGetRevenueStatistics = canGetRevenueStatistics;
        emit canGetRevenueStatisticsChanged();
    }
}

bool SupergroupFullInfo::canToggleAggressiveAntiSpam() const
{
    return m_canToggleAggressiveAntiSpam;
}

void SupergroupFullInfo::setCanToggleAggressiveAntiSpam(bool canToggleAggressiveAntiSpam)
{
    if (m_canToggleAggressiveAntiSpam != canToggleAggressiveAntiSpam)
    {
        m_canToggleAggressiveAntiSpam = canToggleAggressiveAntiSpam;
        emit canToggleAggressiveAntiSpamChanged();
    }
}

bool SupergroupFullInfo::isAllHistoryAvailable() const
{
    return m_isAllHistoryAvailable;
}

void SupergroupFullInfo::setIsAllHistoryAvailable(bool isAllHistoryAvailable)
{
    if (m_isAllHistoryAvailable != isAllHistoryAvailable)
    {
        m_isAllHistoryAvailable = isAllHistoryAvailable;
        emit isAllHistoryAvailableChanged();
    }
}

bool SupergroupFullInfo::canHaveSponsoredMessages() const
{
    return m_canHaveSponsoredMessages;
}

void SupergroupFullInfo::setCanHaveSponsoredMessages(bool canHaveSponsoredMessages)
{
    if (m_canHaveSponsoredMessages != canHaveSponsoredMessages)
    {
        m_canHaveSponsoredMessages = canHaveSponsoredMessages;
        emit canHaveSponsoredMessagesChanged();
    }
}

bool SupergroupFullInfo::hasAggressiveAntiSpamEnabled() const
{
    return m_hasAggressiveAntiSpamEnabled;
}

void SupergroupFullInfo::setHasAggressiveAntiSpamEnabled(bool hasAggressiveAntiSpamEnabled)
{
    if (m_hasAggressiveAntiSpamEnabled != hasAggressiveAntiSpamEnabled)
    {
        m_hasAggressiveAntiSpamEnabled = hasAggressiveAntiSpamEnabled;
        emit hasAggressiveAntiSpamEnabledChanged();
    }
}

bool SupergroupFullInfo::hasPinnedStories() const
{
    return m_hasPinnedStories;
}

void SupergroupFullInfo::setHasPinnedStories(bool hasPinnedStories)
{
    if (m_hasPinnedStories != hasPinnedStories)
    {
        m_hasPinnedStories = hasPinnedStories;
        emit hasPinnedStoriesChanged();
    }
}

int SupergroupFullInfo::myBoostCount() const
{
    return m_myBoostCount;
}

void SupergroupFullInfo::setMyBoostCount(int myBoostCount)
{
    if (m_myBoostCount != myBoostCount)
    {
        m_myBoostCount = myBoostCount;
        emit myBoostCountChanged();
    }
}

int SupergroupFullInfo::unrestrictBoostCount() const
{
    return m_unrestrictBoostCount;
}

void SupergroupFullInfo::setUnrestrictBoostCount(int unrestrictBoostCount)
{
    if (m_unrestrictBoostCount != unrestrictBoostCount)
    {
        m_unrestrictBoostCount = unrestrictBoostCount;
        emit unrestrictBoostCountChanged();
    }
}

qlonglong SupergroupFullInfo::stickerSetId() const
{
    return m_stickerSetId;
}

void SupergroupFullInfo::setStickerSetId(qlonglong stickerSetId)
{
    if (m_stickerSetId != stickerSetId)
    {
        m_stickerSetId = stickerSetId;
        emit stickerSetIdChanged();
    }
}

qlonglong SupergroupFullInfo::customEmojiStickerSetId() const
{
    return m_customEmojiStickerSetId;
}

void SupergroupFullInfo::setCustomEmojiStickerSetId(qlonglong customEmojiStickerSetId)
{
    if (m_customEmojiStickerSetId != customEmojiStickerSetId)
    {
        m_customEmojiStickerSetId = customEmojiStickerSetId;
        emit customEmojiStickerSetIdChanged();
    }
}

QVariantMap SupergroupFullInfo::location() const
{
    return m_location;
}

void SupergroupFullInfo::setLocation(const QVariantMap &location)
{
    if (m_location != location)
    {
        m_location = location;
        emit locationChanged();
    }
}

QVariantMap SupergroupFullInfo::inviteLink() const
{
    return m_inviteLink;
}

void SupergroupFullInfo::setInviteLink(const QVariantMap &inviteLink)
{
    if (m_inviteLink != inviteLink)
    {
        m_inviteLink = inviteLink;
        emit inviteLinkChanged();
    }
}

QVariantList SupergroupFullInfo::botCommands() const
{
    return m_botCommands;
}

void SupergroupFullInfo::setBotCommands(const QVariantList &botCommands)
{
    if (m_botCommands != botCommands)
    {
        m_botCommands = botCommands;
        emit botCommandsChanged();
    }
}

qlonglong SupergroupFullInfo::upgradedFromBasicGroupId() const
{
    return m_upgradedFromBasicGroupId;
}

void SupergroupFullInfo::setUpgradedFromBasicGroupId(qlonglong upgradedFromBasicGroupId)
{
    if (m_upgradedFromBasicGroupId != upgradedFromBasicGroupId)
    {
        m_upgradedFromBasicGroupId = upgradedFromBasicGroupId;
        emit upgradedFromBasicGroupIdChanged();
    }
}

qlonglong SupergroupFullInfo::upgradedFromMaxMessageId() const
{
    return m_upgradedFromMaxMessageId;
}

void SupergroupFullInfo::setUpgradedFromMaxMessageId(qlonglong upgradedFromMaxMessageId)
{
    if (m_upgradedFromMaxMessageId != upgradedFromMaxMessageId)
    {
        m_upgradedFromMaxMessageId = upgradedFromMaxMessageId;
        emit upgradedFromMaxMessageIdChanged();
    }
}

void SupergroupFullInfo::setFromVariantMap(const QVariantMap &map)
{
    if (map.contains("photo"))
        setPhoto(map["photo"].toMap());
    if (map.contains("description"))
        setDescription(map["description"].toString());
    if (map.contains("member_count"))
        setMemberCount(map["member_count"].toInt());
    if (map.contains("administrator_count"))
        setAdministratorCount(map["administrator_count"].toInt());
    if (map.contains("restricted_count"))
        setRestrictedCount(map["restricted_count"].toInt());
    if (map.contains("banned_count"))
        setBannedCount(map["banned_count"].toInt());
    if (map.contains("linked_chat_id"))
        setLinkedChatId(map["linked_chat_id"].toLongLong());
    if (map.contains("slow_mode_delay"))
        setSlowModeDelay(map["slow_mode_delay"].toInt());
    if (map.contains("slow_mode_delay_expires_in"))
        setSlowModeDelayExpiresIn(map["slow_mode_delay_expires_in"].toDouble());
    if (map.contains("can_get_members"))
        setCanGetMembers(map["can_get_members"].toBool());
    if (map.contains("has_hidden_members"))
        setHasHiddenMembers(map["has_hidden_members"].toBool());
    if (map.contains("can_hide_members"))
        setCanHideMembers(map["can_hide_members"].toBool());
    if (map.contains("can_set_sticker_set"))
        setCanSetStickerSet(map["can_set_sticker_set"].toBool());
    if (map.contains("can_set_location"))
        setCanSetLocation(map["can_set_location"].toBool());
    if (map.contains("can_get_statistics"))
        setCanGetStatistics(map["can_get_statistics"].toBool());
    if (map.contains("can_get_revenue_statistics"))
        setCanGetRevenueStatistics(map["can_get_revenue_statistics"].toBool());
    if (map.contains("can_toggle_aggressive_anti_spam"))
        setCanToggleAggressiveAntiSpam(map["can_toggle_aggressive_anti_spam"].toBool());
    if (map.contains("is_all_history_available"))
        setIsAllHistoryAvailable(map["is_all_history_available"].toBool());
    if (map.contains("can_have_sponsored_messages"))
        setCanHaveSponsoredMessages(map["can_have_sponsored_messages"].toBool());
    if (map.contains("has_aggressive_anti_spam_enabled"))
        setHasAggressiveAntiSpamEnabled(map["has_aggressive_anti_spam_enabled"].toBool());
    if (map.contains("has_pinned_stories"))
        setHasPinnedStories(map["has_pinned_stories"].toBool());
    if (map.contains("my_boost_count"))
        setMyBoostCount(map["my_boost_count"].toInt());
    if (map.contains("unrestrict_boost_count"))
        setUnrestrictBoostCount(map["unrestrict_boost_count"].toInt());
    if (map.contains("sticker_set_id"))
        setStickerSetId(map["sticker_set_id"].toLongLong());
    if (map.contains("custom_emoji_sticker_set_id"))
        setCustomEmojiStickerSetId(map["custom_emoji_sticker_set_id"].toLongLong());
    if (map.contains("location"))
        setLocation(map["location"].toMap());
    if (map.contains("invite_link"))
        setInviteLink(map["invite_link"].toMap());
    if (map.contains("bot_commands"))
        setBotCommands(map["bot_commands"].toList());
    if (map.contains("upgraded_from_basic_group_id"))
        setUpgradedFromBasicGroupId(map["upgraded_from_basic_group_id"].toLongLong());
    if (map.contains("upgraded_from_max_message_id"))
        setUpgradedFromMaxMessageId(map["upgraded_from_max_message_id"].toLongLong());
}
