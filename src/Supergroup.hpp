#pragma once

#include <td/telegram/td_api.h>

#include <QObject>
#include <QVariant>
#include <QStringList>

class Supergroup : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong id READ id NOTIFY supergroupInfoChanged)
    Q_PROPERTY(QVariantMap usernames READ usernames NOTIFY supergroupInfoChanged)
    Q_PROPERTY(int date READ date NOTIFY supergroupInfoChanged)
    Q_PROPERTY(Status status READ status NOTIFY supergroupInfoChanged)
    Q_PROPERTY(int memberCount READ memberCount NOTIFY supergroupInfoChanged)
    Q_PROPERTY(int boostLevel READ boostLevel NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool hasLinkedChat READ hasLinkedChat NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool hasLocation READ hasLocation NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool signMessages READ signMessages NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool joinToSendMessages READ joinToSendMessages NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool joinByRequest READ joinByRequest NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool isSlowModeEnabled READ isSlowModeEnabled NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool isChannel READ isChannel NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool isBroadcastGroup READ isBroadcastGroup NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool isForum READ isForum NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool isVerified READ isVerified NOTIFY supergroupInfoChanged)
    Q_PROPERTY(QString restrictionReason READ restrictionReason NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool isScam READ isScam NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool isFake READ isFake NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool hasActiveStories READ hasActiveStories NOTIFY supergroupInfoChanged)
    Q_PROPERTY(bool hasUnreadActiveStories READ hasUnreadActiveStories NOTIFY supergroupInfoChanged)

public:
    explicit Supergroup(td::td_api::object_ptr<td::td_api::supergroup> group, QObject *parent = nullptr);

    enum Status { Creator, Administrator, Member, Restricted, Left, Banned };

    qlonglong id() const;
    QVariantMap usernames() const;
    int date() const;
    Status status() const;
    int memberCount() const;
    int boostLevel() const;
    bool hasLinkedChat() const;
    bool hasLocation() const;
    bool signMessages() const;
    bool joinToSendMessages() const;
    bool joinByRequest() const;
    bool isSlowModeEnabled() const;
    bool isChannel() const;
    bool isBroadcastGroup() const;
    bool isForum() const;
    bool isVerified() const;
    QString restrictionReason() const;
    bool isScam() const;
    bool isFake() const;
    bool hasActiveStories() const;
    bool hasUnreadActiveStories() const;

    QStringList activeUsernames() const;

signals:
    void supergroupInfoChanged();

private:
    qlonglong m_id;
    QVariantMap m_usernames;
    int m_date;
    Status m_status;
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

    QStringList m_activeUsernames;
};
