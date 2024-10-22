#pragma once

#include <td/telegram/td_api.h>

#include <QVariant>
#include <QObject>

class SupergroupFullInfo : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap photo READ photo NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(QString description READ description NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(int memberCount READ memberCount NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(int administratorCount READ administratorCount NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(int restrictedCount READ restrictedCount NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(int bannedCount READ bannedCount NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(qlonglong linkedChatId READ linkedChatId NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(int slowModeDelay READ slowModeDelay NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(double slowModeDelayExpiresIn READ slowModeDelayExpiresIn NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canGetMembers READ canGetMembers NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool hasHiddenMembers READ hasHiddenMembers NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canHideMembers READ canHideMembers NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canSetStickerSet READ canSetStickerSet NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canSetLocation READ canSetLocation NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canGetStatistics READ canGetStatistics NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canGetRevenueStatistics READ canGetRevenueStatistics NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canToggleAggressiveAntiSpam READ canToggleAggressiveAntiSpam NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool isAllHistoryAvailable READ isAllHistoryAvailable NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool canHaveSponsoredMessages READ canHaveSponsoredMessages NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool hasAggressiveAntiSpamEnabled READ hasAggressiveAntiSpamEnabled NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(bool hasPinnedStories READ hasPinnedStories NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(int myBoostCount READ myBoostCount NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(int unrestrictBoostCount READ unrestrictBoostCount NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(qlonglong stickerSetId READ stickerSetId NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(qlonglong customEmojiStickerSetId READ customEmojiStickerSetId NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(QVariantMap location READ location NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(QVariantMap inviteLink READ inviteLink NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(QVariantList botCommands READ botCommands NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(qlonglong upgradedFromBasicGroupId READ upgradedFromBasicGroupId NOTIFY supergroupFullInfoChanged)
    Q_PROPERTY(qlonglong upgradedFromMaxMessageId READ upgradedFromMaxMessageId NOTIFY supergroupFullInfoChanged)

public:
    explicit SupergroupFullInfo(td::td_api::object_ptr<td::td_api::supergroupFullInfo> groupFullInfo, QObject *parent = nullptr);

    QVariantMap photo() const;
    QString description() const;
    int memberCount() const;
    int administratorCount() const;
    int restrictedCount() const;
    int bannedCount() const;
    qlonglong linkedChatId() const;
    int slowModeDelay() const;
    double slowModeDelayExpiresIn() const;
    bool canGetMembers() const;
    bool hasHiddenMembers() const;
    bool canHideMembers() const;
    bool canSetStickerSet() const;
    bool canSetLocation() const;
    bool canGetStatistics() const;
    bool canGetRevenueStatistics() const;
    bool canToggleAggressiveAntiSpam() const;
    bool isAllHistoryAvailable() const;
    bool canHaveSponsoredMessages() const;
    bool hasAggressiveAntiSpamEnabled() const;
    bool hasPinnedStories() const;
    int myBoostCount() const;
    int unrestrictBoostCount() const;
    qlonglong stickerSetId() const;
    qlonglong customEmojiStickerSetId() const;
    QVariantMap location() const;
    QVariantMap inviteLink() const;
    QVariantList botCommands() const;
    qlonglong upgradedFromBasicGroupId() const;
    qlonglong upgradedFromMaxMessageId() const;

signals:
    void supergroupFullInfoChanged();

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
