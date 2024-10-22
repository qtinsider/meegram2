#pragma once

#include <td/telegram/td_api.h>

#include <QDateTime>
#include <QObject>
#include <QVariant>

#include <QStringList>

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qlonglong id READ id NOTIFY userInfoChanged)
    Q_PROPERTY(QString firstName READ firstName NOTIFY userInfoChanged)
    Q_PROPERTY(QString lastName READ lastName NOTIFY userInfoChanged)
    Q_PROPERTY(QVariantMap usernames READ usernames NOTIFY userInfoChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber NOTIFY userInfoChanged)
    Q_PROPERTY(Status status READ status NOTIFY userInfoChanged)
    Q_PROPERTY(QVariantMap profilePhoto READ profilePhoto NOTIFY userInfoChanged)
    Q_PROPERTY(int accentColorId READ accentColorId NOTIFY userInfoChanged)
    Q_PROPERTY(qlonglong backgroundCustomEmojiId READ backgroundCustomEmojiId NOTIFY userInfoChanged)
    Q_PROPERTY(int profileAccentColorId READ profileAccentColorId NOTIFY userInfoChanged)
    Q_PROPERTY(qlonglong profileBackgroundCustomEmojiId READ profileBackgroundCustomEmojiId NOTIFY userInfoChanged)
    Q_PROPERTY(QVariantMap emojiStatus READ emojiStatus NOTIFY userInfoChanged)
    Q_PROPERTY(bool isContact READ isContact NOTIFY userInfoChanged)
    Q_PROPERTY(bool isMutualContact READ isMutualContact NOTIFY userInfoChanged)
    Q_PROPERTY(bool isCloseFriend READ isCloseFriend NOTIFY userInfoChanged)
    Q_PROPERTY(bool isVerified READ isVerified NOTIFY userInfoChanged)
    Q_PROPERTY(bool isPremium READ isPremium NOTIFY userInfoChanged)
    Q_PROPERTY(bool isSupport READ isSupport NOTIFY userInfoChanged)
    Q_PROPERTY(QString restrictionReason READ restrictionReason NOTIFY userInfoChanged)
    Q_PROPERTY(bool isScam READ isScam NOTIFY userInfoChanged)
    Q_PROPERTY(bool isFake READ isFake NOTIFY userInfoChanged)
    Q_PROPERTY(bool hasActiveStories READ hasActiveStories NOTIFY userInfoChanged)
    Q_PROPERTY(bool hasUnreadActiveStories READ hasUnreadActiveStories NOTIFY userInfoChanged)
    Q_PROPERTY(bool restrictsNewChats READ restrictsNewChats NOTIFY userInfoChanged)
    Q_PROPERTY(bool haveAccess READ haveAccess NOTIFY userInfoChanged)
    Q_PROPERTY(Type type READ type NOTIFY userInfoChanged)
    Q_PROPERTY(QString languageCode READ languageCode NOTIFY userInfoChanged)
    Q_PROPERTY(bool addedToAttachmentMenu READ addedToAttachmentMenu NOTIFY userInfoChanged)

    Q_ENUMS(Type)
public:
    explicit User(td::td_api::object_ptr<td::td_api::user> user, QObject *parent = nullptr);

    enum Status { Empty, Online, Offline, Recently, LastWeek, LastMonth };
    enum Type { Bot, Regular, Deleted, Unknown };

    qlonglong id() const;
    QString firstName() const;
    QString lastName() const;
    QVariantMap usernames() const;
    QString phoneNumber() const;
    Status status() const;
    QVariantMap profilePhoto() const;
    int accentColorId() const;
    qlonglong backgroundCustomEmojiId() const;
    int profileAccentColorId() const;
    qlonglong profileBackgroundCustomEmojiId() const;
    QVariantMap emojiStatus() const;
    bool isContact() const;
    bool isMutualContact() const;
    bool isCloseFriend() const;
    bool isVerified() const;
    bool isPremium() const;
    bool isSupport() const;
    QString restrictionReason() const;
    bool isScam() const;
    bool isFake() const;
    bool hasActiveStories() const;
    bool hasUnreadActiveStories() const;
    bool restrictsNewChats() const;
    bool haveAccess() const;
    Type type() const;
    QString languageCode() const;
    bool addedToAttachmentMenu() const;

    QDateTime wasOnline() const;
    QStringList activeUsernames() const;

    void setStatus(td::td_api::object_ptr<td::td_api::UserStatus> status);

signals:
    void userInfoChanged();

private:
    Type determineType(const td::td_api::object_ptr<td::td_api::UserType> &type) const;

    qlonglong m_id;
    QString m_firstName;
    QString m_lastName;
    QVariantMap m_usernames;
    QString m_phoneNumber;
    Status m_status;
    QVariantMap m_profilePhoto;
    int m_accentColorId;
    qlonglong m_backgroundCustomEmojiId;
    int m_profileAccentColorId;
    qlonglong m_profileBackgroundCustomEmojiId;
    QVariantMap m_emojiStatus;
    bool m_isContact;
    bool m_isMutualContact;
    bool m_isCloseFriend;
    bool m_isVerified;
    bool m_isPremium;
    bool m_isSupport;
    QString m_restrictionReason;
    bool m_isScam;
    bool m_isFake;
    bool m_hasActiveStories;
    bool m_hasUnreadActiveStories;
    bool m_restrictsNewChats;
    bool m_haveAccess;
    Type m_type;
    QString m_languageCode;
    bool m_addedToAttachmentMenu;

    QDateTime m_wasOnline;
    QStringList m_activeUsernames;
};
