#pragma once

#include <QVariant>

class Supergroup : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(QVariantMap usernames READ usernames WRITE setUsernames NOTIFY usernamesChanged)
    Q_PROPERTY(int date READ date WRITE setDate NOTIFY dateChanged)
    Q_PROPERTY(QVariantMap status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(int memberCount READ memberCount WRITE setMemberCount NOTIFY memberCountChanged)
    Q_PROPERTY(int boostLevel READ boostLevel WRITE setBoostLevel NOTIFY boostLevelChanged)
    Q_PROPERTY(bool hasLinked_chat READ hasLinkedChat WRITE setHasLinkedChat NOTIFY hasLinkedChatChanged)
    Q_PROPERTY(bool hasLocation READ hasLocation WRITE setHasLocation NOTIFY hasLocationChanged)
    Q_PROPERTY(bool signMessages READ signMessages WRITE setSignMessages NOTIFY signMessagesChanged)
    Q_PROPERTY(bool joinToSendMessages READ joinToSendMessages WRITE setJoinToSendMessages NOTIFY joinToSendMessagesChanged)
    Q_PROPERTY(bool joinByRequest READ joinByRequest WRITE setJoinByRequest NOTIFY joinByRequestChanged)
    Q_PROPERTY(bool isSlowModeEnabled READ isSlowModeEnabled WRITE setIsSlowModeEnabled NOTIFY isSlowModeEnabledChanged)
    Q_PROPERTY(bool isChannel READ isChannel WRITE setIsChannel NOTIFY isChannelChanged)
    Q_PROPERTY(bool isBroadcast_group READ isBroadcastGroup WRITE setIsBroadcastGroup NOTIFY isBroadcastGroupChanged)
    Q_PROPERTY(bool isForum READ isForum WRITE setIsForum NOTIFY isForumChanged)
    Q_PROPERTY(bool isVerified READ isVerified WRITE setIsVerified NOTIFY isVerifiedChanged)
    Q_PROPERTY(QString restrictionReason READ restrictionReason WRITE setRestrictionReason NOTIFY restrictionReasonChanged)
    Q_PROPERTY(bool isScam READ isScam WRITE setIsScam NOTIFY isScamChanged)
    Q_PROPERTY(bool isFake READ isFake WRITE setIsFake NOTIFY isFakeChanged)
    Q_PROPERTY(bool hasActiveStories READ hasActiveStories WRITE setHasActiveStories NOTIFY hasActiveStoriesChanged)
    Q_PROPERTY(bool hasUnreadActiveStories READ hasUnreadActiveStories WRITE setHasUnreadActiveStories NOTIFY hasUnreadActiveStoriesChanged)

public:
    explicit Supergroup(QObject *parent = nullptr);

    qlonglong id() const;
    void setId(qlonglong id);

    QVariantMap usernames() const;
    void setUsernames(const QVariantMap &usernames);

    int date() const;
    void setDate(int date);

    QVariantMap status() const;
    void setStatus(const QVariantMap &status);

    int memberCount() const;
    void setMemberCount(int memberCount);

    int boostLevel() const;
    void setBoostLevel(int boostLevel);

    bool hasLinkedChat() const;
    void setHasLinkedChat(bool hasLinkedChat);

    bool hasLocation() const;
    void setHasLocation(bool hasLocation);

    bool signMessages() const;
    void setSignMessages(bool signMessages);

    bool joinToSendMessages() const;
    void setJoinToSendMessages(bool joinToSendMessages);

    bool joinByRequest() const;
    void setJoinByRequest(bool joinByRequest);

    bool isSlowModeEnabled() const;
    void setIsSlowModeEnabled(bool isSlowModeEnabled);

    bool isChannel() const;
    void setIsChannel(bool isChannel);

    bool isBroadcastGroup() const;
    void setIsBroadcastGroup(bool isBroadcastGroup);

    bool isForum() const;
    void setIsForum(bool isForum);

    bool isVerified() const;
    void setIsVerified(bool isVerified);

    QString restrictionReason() const;
    void setRestrictionReason(const QString &restrictionReason);

    bool isScam() const;
    void setIsScam(bool isScam);

    bool isFake() const;
    void setIsFake(bool isFake);

    bool hasActiveStories() const;
    void setHasActiveStories(bool hasActiveStories);

    bool hasUnreadActiveStories() const;
    void setHasUnreadActiveStories(bool hasUnreadActiveStories);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void idChanged();
    void usernamesChanged();
    void dateChanged();
    void statusChanged();
    void memberCountChanged();
    void boostLevelChanged();
    void hasLinkedChatChanged();
    void hasLocationChanged();
    void signMessagesChanged();
    void joinToSendMessagesChanged();
    void joinByRequestChanged();
    void isSlowModeEnabledChanged();
    void isChannelChanged();
    void isBroadcastGroupChanged();
    void isForumChanged();
    void isVerifiedChanged();
    void restrictionReasonChanged();
    void isScamChanged();
    void isFakeChanged();
    void hasActiveStoriesChanged();
    void hasUnreadActiveStoriesChanged();

private:
    qlonglong m_id;
    QVariantMap m_usernames;
    int m_date;
    QVariantMap m_status;
    int m_memberCount;
    int m_boostLevel;
    bool m_hasLinkedChat;
    bool m_hasLocation;
    bool m_signMessages;
    bool m_joinToSendMessages;
    bool m_joinByRequest;
    bool m_isSlowModeEnabled;
    bool m_isChannel;
    bool m_isBroadcastGroup;
    bool m_isForum;
    bool m_isVerified;
    QString m_restrictionReason;
    bool m_isScam;
    bool m_isFake;
    bool m_hasActiveStories;
    bool m_hasUnreadActiveStories;
};
