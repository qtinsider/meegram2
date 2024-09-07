#pragma once

#include <QVariant>

class SupergroupFullInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap photo READ photo WRITE setPhoto NOTIFY photoChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(int memberCount READ memberCount WRITE setMemberCount NOTIFY memberCountChanged)
    Q_PROPERTY(int administratorCount READ administratorCount WRITE setAdministratorCount NOTIFY administratorCountChanged)
    Q_PROPERTY(int restrictedCount READ restrictedCount WRITE setRestrictedCount NOTIFY restrictedCountChanged)
    Q_PROPERTY(int bannedCount READ bannedCount WRITE setBannedCount NOTIFY bannedCountChanged)
    Q_PROPERTY(qlonglong linkedChatId READ linkedChatId WRITE setLinkedChatId NOTIFY linkedChatIdChanged)
    Q_PROPERTY(int slowModeDelay READ slowModeDelay WRITE setSlowModeDelay NOTIFY slowModeDelayChanged)
    Q_PROPERTY(
        double slowModeDelayExpiresIn READ slowModeDelayExpiresIn WRITE setSlowModeDelayExpiresIn NOTIFY slowModeDelayExpiresInChanged)
    Q_PROPERTY(bool canGetMembers READ canGetMembers WRITE setCanGetMembers NOTIFY canGetMembersChanged)
    Q_PROPERTY(bool hasHiddenMembers READ hasHiddenMembers WRITE setHasHiddenMembers NOTIFY hasHiddenMembersChanged)
    Q_PROPERTY(bool canHideMembers READ canHideMembers WRITE setCanHideMembers NOTIFY canHideMembersChanged)
    Q_PROPERTY(bool canSetStickerSet READ canSetStickerSet WRITE setCanSetStickerSet NOTIFY canSetStickerSetChanged)
    Q_PROPERTY(bool canSetLocation READ canSetLocation WRITE setCanSetLocation NOTIFY canSetLocationChanged)
    Q_PROPERTY(bool canGetStatistics READ canGetStatistics WRITE setCanGetStatistics NOTIFY canGetStatisticsChanged)
    Q_PROPERTY(
        bool canGetRevenueStatistics READ canGetRevenueStatistics WRITE setCanGetRevenueStatistics NOTIFY canGetRevenueStatisticsChanged)
    Q_PROPERTY(bool canToggleAggressiveAntiSpam READ canToggleAggressiveAntiSpam WRITE setCanToggleAggressiveAntiSpam NOTIFY
                   canToggleAggressiveAntiSpamChanged)
    Q_PROPERTY(bool isAllHistoryAvailable READ isAllHistoryAvailable WRITE setIsAllHistoryAvailable NOTIFY isAllHistoryAvailableChanged)
    Q_PROPERTY(bool canHaveSponsoredMessages READ canHaveSponsoredMessages WRITE setCanHaveSponsoredMessages NOTIFY
                   canHaveSponsoredMessagesChanged)
    Q_PROPERTY(bool hasAggressiveAntiSpamEnabled READ hasAggressiveAntiSpamEnabled WRITE setHasAggressiveAntiSpamEnabled NOTIFY
                   hasAggressiveAntiSpamEnabledChanged)
    Q_PROPERTY(bool hasPinnedStories READ hasPinnedStories WRITE setHasPinnedStories NOTIFY hasPinnedStoriesChanged)
    Q_PROPERTY(int myBoostCount READ myBoostCount WRITE setMyBoostCount NOTIFY myBoostCountChanged)
    Q_PROPERTY(int unrestrictBoostCount READ unrestrictBoostCount WRITE setUnrestrictBoostCount NOTIFY unrestrictBoostCountChanged)
    Q_PROPERTY(qlonglong stickerSetId READ stickerSetId WRITE setStickerSetId NOTIFY stickerSetIdChanged)
    Q_PROPERTY(
        qlonglong customEmojiStickerSetId READ customEmojiStickerSetId WRITE setCustomEmojiStickerSetId NOTIFY customEmojiStickerSetIdChanged)
    Q_PROPERTY(QVariantMap location READ location WRITE setLocation NOTIFY locationChanged)
    Q_PROPERTY(QVariantMap inviteLink READ inviteLink WRITE setInviteLink NOTIFY inviteLinkChanged)
    Q_PROPERTY(QVariantList botCommands READ botCommands WRITE setBotCommands NOTIFY botCommandsChanged)
    Q_PROPERTY(qlonglong upgradedFromBasicGroupId READ upgradedFromBasicGroupId WRITE setUpgradedFromBasicGroupId NOTIFY
                   upgradedFromBasicGroupIdChanged)
    Q_PROPERTY(qlonglong upgradedFromMaxMessageId READ upgradedFromMaxMessageId WRITE setUpgradedFromMaxMessageId NOTIFY
                   upgradedFromMaxMessageIdChanged)

public:
    explicit SupergroupFullInfo(QObject *parent = nullptr);

    QVariantMap photo() const;
    void setPhoto(const QVariantMap &photo);

    QString description() const;
    void setDescription(const QString &description);

    int memberCount() const;
    void setMemberCount(int memberCount);

    int administratorCount() const;
    void setAdministratorCount(int administratorCount);

    int restrictedCount() const;
    void setRestrictedCount(int restrictedCount);

    int bannedCount() const;
    void setBannedCount(int bannedCount);

    qlonglong linkedChatId() const;
    void setLinkedChatId(qlonglong linkedChatId);

    int slowModeDelay() const;
    void setSlowModeDelay(int slowModeDelay);

    double slowModeDelayExpiresIn() const;
    void setSlowModeDelayExpiresIn(double slowModeDelayExpiresIn);

    bool canGetMembers() const;
    void setCanGetMembers(bool canGetMembers);

    bool hasHiddenMembers() const;
    void setHasHiddenMembers(bool hasHiddenMembers);

    bool canHideMembers() const;
    void setCanHideMembers(bool canHideMembers);

    bool canSetStickerSet() const;
    void setCanSetStickerSet(bool canSetStickerSet);

    bool canSetLocation() const;
    void setCanSetLocation(bool canSetLocation);

    bool canGetStatistics() const;
    void setCanGetStatistics(bool canGetStatistics);

    bool canGetRevenueStatistics() const;
    void setCanGetRevenueStatistics(bool canGetRevenueStatistics);

    bool canToggleAggressiveAntiSpam() const;
    void setCanToggleAggressiveAntiSpam(bool canToggleAggressiveAntiSpam);

    bool isAllHistoryAvailable() const;
    void setIsAllHistoryAvailable(bool isAllHistoryAvailable);

    bool canHaveSponsoredMessages() const;
    void setCanHaveSponsoredMessages(bool canHaveSponsoredMessages);

    bool hasAggressiveAntiSpamEnabled() const;
    void setHasAggressiveAntiSpamEnabled(bool hasAggressiveAntiSpamEnabled);

    bool hasPinnedStories() const;
    void setHasPinnedStories(bool hasPinnedStories);

    int myBoostCount() const;
    void setMyBoostCount(int myBoostCount);

    int unrestrictBoostCount() const;
    void setUnrestrictBoostCount(int unrestrictBoostCount);

    qlonglong stickerSetId() const;
    void setStickerSetId(qlonglong stickerSetId);

    qlonglong customEmojiStickerSetId() const;
    void setCustomEmojiStickerSetId(qlonglong customEmojiStickerSetId);

    QVariantMap location() const;
    void setLocation(const QVariantMap &location);

    QVariantMap inviteLink() const;
    void setInviteLink(const QVariantMap &inviteLink);

    QVariantList botCommands() const;
    void setBotCommands(const QVariantList &botCommands);

    qlonglong upgradedFromBasicGroupId() const;
    void setUpgradedFromBasicGroupId(qlonglong upgradedFromBasicGroupId);

    qlonglong upgradedFromMaxMessageId() const;
    void setUpgradedFromMaxMessageId(qlonglong upgradedFromMaxMessageId);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void photoChanged();
    void descriptionChanged();
    void memberCountChanged();
    void administratorCountChanged();
    void restrictedCountChanged();
    void bannedCountChanged();
    void linkedChatIdChanged();
    void slowModeDelayChanged();
    void slowModeDelayExpiresInChanged();
    void canGetMembersChanged();
    void hasHiddenMembersChanged();
    void canHideMembersChanged();
    void canSetStickerSetChanged();
    void canSetLocationChanged();
    void canGetStatisticsChanged();
    void canGetRevenueStatisticsChanged();
    void canToggleAggressiveAntiSpamChanged();
    void isAllHistoryAvailableChanged();
    void canHaveSponsoredMessagesChanged();
    void hasAggressiveAntiSpamEnabledChanged();
    void hasPinnedStoriesChanged();
    void myBoostCountChanged();
    void unrestrictBoostCountChanged();
    void stickerSetIdChanged();
    void customEmojiStickerSetIdChanged();
    void locationChanged();
    void inviteLinkChanged();
    void botCommandsChanged();
    void upgradedFromBasicGroupIdChanged();
    void upgradedFromMaxMessageIdChanged();

private:
    QVariantMap m_photo;
    QString m_description;
    int m_memberCount;
    int m_administratorCount;
    int m_restrictedCount;
    int m_bannedCount;
    qlonglong m_linkedChatId;
    int m_slowModeDelay;
    double m_slowModeDelayExpiresIn;
    bool m_canGetMembers;
    bool m_hasHiddenMembers;
    bool m_canHideMembers;
    bool m_canSetStickerSet;
    bool m_canSetLocation;
    bool m_canGetStatistics;
    bool m_canGetRevenueStatistics;
    bool m_canToggleAggressiveAntiSpam;
    bool m_isAllHistoryAvailable;
    bool m_canHaveSponsoredMessages;
    bool m_hasAggressiveAntiSpamEnabled;
    bool m_hasPinnedStories;
    int m_myBoostCount;
    int m_unrestrictBoostCount;
    qlonglong m_stickerSetId;
    qlonglong m_customEmojiStickerSetId;
    QVariantMap m_location;
    QVariantMap m_inviteLink;
    QVariantList m_botCommands;
    qlonglong m_upgradedFromBasicGroupId;
    qlonglong m_upgradedFromMaxMessageId;
};
