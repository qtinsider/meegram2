#pragma once

#include <QObject>
#include <QString>
#include <QVariant>
#include <QVector>

class User : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 id READ id NOTIFY idChanged)
    Q_PROPERTY(QString firstName READ firstName NOTIFY firstNameChanged)
    Q_PROPERTY(QString lastName READ lastName NOTIFY lastNameChanged)
    Q_PROPERTY(QVariantMap usernames READ usernames NOTIFY usernamesChanged)
    Q_PROPERTY(QString phoneNumber READ phoneNumber NOTIFY phoneNumberChanged)
    Q_PROPERTY(QVariantMap status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariantMap profilePhoto READ profilePhoto NOTIFY profilePhotoChanged)
    Q_PROPERTY(qint32 accentColorId READ accentColorId NOTIFY accentColorIdChanged)
    Q_PROPERTY(qint64 backgroundCustomEmojiId READ backgroundCustomEmojiId NOTIFY backgroundCustomEmojiIdChanged)
    Q_PROPERTY(qint32 profileAccentColorId READ profileAccentColorId NOTIFY profileAccentColorIdChanged)
    Q_PROPERTY(qint64 profileBackgroundCustomEmojiId READ profileBackgroundCustomEmojiId NOTIFY profileBackgroundCustomEmojiIdChanged)
    Q_PROPERTY(QVariantMap emojiStatus READ emojiStatus NOTIFY emojiStatusChanged)
    Q_PROPERTY(bool isContact READ isContact NOTIFY isContactChanged)
    Q_PROPERTY(bool isMutualContact READ isMutualContact NOTIFY isMutualContactChanged)
    Q_PROPERTY(bool isCloseFriend READ isCloseFriend NOTIFY isCloseFriendChanged)
    Q_PROPERTY(bool isVerified READ isVerified NOTIFY isVerifiedChanged)
    Q_PROPERTY(bool isPremium READ isPremium NOTIFY isPremiumChanged)
    Q_PROPERTY(bool isSupport READ isSupport NOTIFY isSupportChanged)
    Q_PROPERTY(QString restrictionReason READ restrictionReason NOTIFY restrictionReasonChanged)
    Q_PROPERTY(bool isScam READ isScam NOTIFY isScamChanged)
    Q_PROPERTY(bool isFake READ isFake NOTIFY isFakeChanged)
    Q_PROPERTY(bool hasActiveStories READ hasActiveStories NOTIFY hasActiveStoriesChanged)
    Q_PROPERTY(bool hasUnreadActiveStories READ hasUnreadActiveStories NOTIFY hasUnreadActiveStoriesChanged)
    Q_PROPERTY(bool restrictsNewChats READ restrictsNewChats NOTIFY restrictsNewChatsChanged)
    Q_PROPERTY(bool haveAccess READ haveAccess NOTIFY haveAccessChanged)
    Q_PROPERTY(QVariantMap type READ type NOTIFY typeChanged)
    Q_PROPERTY(QString languageCode READ languageCode NOTIFY languageCodeChanged)
    Q_PROPERTY(bool addedToAttachmentMenu READ addedToAttachmentMenu NOTIFY addedToAttachmentMenuChanged)

public:
    User(QObject *parent = nullptr);

    qint64 id() const;
    QString firstName() const;
    QString lastName() const;
    QVariantMap usernames() const;
    QString phoneNumber() const;
    QVariantMap status() const;
    QVariantMap profilePhoto() const;
    qint32 accentColorId() const;
    qint64 backgroundCustomEmojiId() const;
    qint32 profileAccentColorId() const;
    qint64 profileBackgroundCustomEmojiId() const;
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
    QVariantMap type() const;
    QString languageCode() const;
    bool addedToAttachmentMenu() const;

    void setId(qint64 id);
    void setFirstName(const QString &firstName);
    void setLastName(const QString &lastName);
    void setUsernames(const QVariantMap &usernames);
    void setPhoneNumber(const QString &phoneNumber);
    void setStatus(const QVariantMap &status);
    void setProfilePhoto(const QVariantMap &profilePhoto);
    void setAccentColorId(qint32 accentColorId);
    void setBackgroundCustomEmojiId(qint64 backgroundCustomEmojiId);
    void setProfileAccentColorId(qint32 profileAccentColorId);
    void setProfileBackgroundCustomEmojiId(qint64 profileBackgroundCustomEmojiId);
    void setEmojiStatus(const QVariantMap &emojiStatus);
    void setIsContact(bool isContact);
    void setIsMutualContact(bool isMutualContact);
    void setIsCloseFriend(bool isCloseFriend);
    void setIsVerified(bool isVerified);
    void setIsPremium(bool isPremium);
    void setIsSupport(bool isSupport);
    void setRestrictionReason(const QString &restrictionReason);
    void setIsScam(bool isScam);
    void setIsFake(bool isFake);
    void setHasActiveStories(bool hasActiveStories);
    void setHasUnreadActiveStories(bool hasUnreadActiveStories);
    void setRestrictsNewChats(bool restrictsNewChats);
    void setHaveAccess(bool haveAccess);
    void setType(const QVariantMap &type);
    void setLanguageCode(const QString &languageCode);
    void setAddedToAttachmentMenu(bool addedToAttachmentMenu);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void idChanged();
    void firstNameChanged();
    void lastNameChanged();
    void usernamesChanged();
    void phoneNumberChanged();
    void statusChanged();
    void profilePhotoChanged();
    void accentColorIdChanged();
    void backgroundCustomEmojiIdChanged();
    void profileAccentColorIdChanged();
    void profileBackgroundCustomEmojiIdChanged();
    void emojiStatusChanged();
    void isContactChanged();
    void isMutualContactChanged();
    void isCloseFriendChanged();
    void isVerifiedChanged();
    void isPremiumChanged();
    void isSupportChanged();
    void restrictionReasonChanged();
    void isScamChanged();
    void isFakeChanged();
    void hasActiveStoriesChanged();
    void hasUnreadActiveStoriesChanged();
    void restrictsNewChatsChanged();
    void haveAccessChanged();
    void typeChanged();
    void languageCodeChanged();
    void addedToAttachmentMenuChanged();

private:
    qint64 m_id;
    QString m_firstName;
    QString m_lastName;
    QVariantMap m_usernames;
    QString m_phoneNumber;
    QVariantMap m_status;
    QVariantMap m_profilePhoto;
    qint32 m_accentColorId;
    qint64 m_backgroundCustomEmojiId;
    qint32 m_profileAccentColorId;
    qint64 m_profileBackgroundCustomEmojiId;
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
    QVariantMap m_type;
    QString m_languageCode;
    bool m_addedToAttachmentMenu;
};
