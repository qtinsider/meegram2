#pragma once

#include <QObject>
#include <QVariant>

class UserFullInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap personalPhoto READ personalPhoto WRITE setPersonalPhoto NOTIFY personalPhotoChanged)
    Q_PROPERTY(QVariantMap photo READ photo WRITE setPhoto NOTIFY photoChanged)
    Q_PROPERTY(QVariantMap publicPhoto READ publicPhoto WRITE setPublicPhoto NOTIFY publicPhotoChanged)
    Q_PROPERTY(QVariantMap blockList READ blockList WRITE setBlockList NOTIFY blockListChanged)
    Q_PROPERTY(bool canBeCalled READ canBeCalled WRITE setCanBeCalled NOTIFY canBeCalledChanged)
    Q_PROPERTY(bool supportsVideoCalls READ supportsVideoCalls WRITE setSupportsVideoCalls NOTIFY supportsVideoCallsChanged)
    Q_PROPERTY(bool hasPrivateCalls READ hasPrivateCalls WRITE setHasPrivateCalls NOTIFY hasPrivateCallsChanged)
    Q_PROPERTY(bool hasPrivateForwards READ hasPrivateForwards WRITE setHasPrivateForwards NOTIFY hasPrivateForwardsChanged)
    Q_PROPERTY(bool hasRestrictedVoiceAndVideoNoteMessages READ hasRestrictedVoiceAndVideoNoteMessages WRITE
                   setHasRestrictedVoiceAndVideoNoteMessages NOTIFY hasRestrictedVoiceAndVideoNoteMessagesChanged)
    Q_PROPERTY(bool hasPostedToProfileStories READ hasPostedToProfileStories WRITE setHasPostedToProfileStories NOTIFY
                   hasPostedToProfileStoriesChanged)
    Q_PROPERTY(bool hasSponsoredMessagesEnabled READ hasSponsoredMessagesEnabled WRITE setHasSponsoredMessagesEnabled NOTIFY
                   hasSponsoredMessagesEnabledChanged)
    Q_PROPERTY(bool needPhoneNumberPrivacyException READ needPhoneNumberPrivacyException WRITE setNeedPhoneNumberPrivacyException NOTIFY
                   needPhoneNumberPrivacyExceptionChanged)
    Q_PROPERTY(bool setChatBackground READ setChatBackground WRITE setSetChatBackground NOTIFY setChatBackgroundChanged)
    Q_PROPERTY(QVariantMap bio READ bio WRITE setBio NOTIFY bioChanged)
    Q_PROPERTY(QVariantMap birthdate READ birthdate WRITE setBirthdate NOTIFY birthdateChanged)
    Q_PROPERTY(qint64 personalChatId READ personalChatId WRITE setPersonalChatId NOTIFY personalChatIdChanged)
    Q_PROPERTY(QVariantList premiumGiftOptions READ premiumGiftOptions WRITE setPremiumGiftOptions NOTIFY premiumGiftOptionsChanged)
    Q_PROPERTY(int groupInCommonCount READ groupInCommonCount WRITE setGroupInCommonCount NOTIFY groupInCommonCountChanged)
    Q_PROPERTY(QVariantMap businessInfo READ businessInfo WRITE setBusinessInfo NOTIFY businessInfoChanged)
    Q_PROPERTY(QVariantMap botInfo READ botInfo WRITE setBotInfo NOTIFY botInfoChanged)

public:
    explicit UserFullInfo(QObject *parent = nullptr);

    QVariantMap personalPhoto() const;
    void setPersonalPhoto(const QVariantMap &personalPhoto);

    QVariantMap photo() const;
    void setPhoto(const QVariantMap &photo);

    QVariantMap publicPhoto() const;
    void setPublicPhoto(const QVariantMap &publicPhoto);

    QVariantMap blockList() const;
    void setBlockList(const QVariantMap &blockList);

    bool canBeCalled() const;
    void setCanBeCalled(bool canBeCalled);

    bool supportsVideoCalls() const;
    void setSupportsVideoCalls(bool supportsVideoCalls);

    bool hasPrivateCalls() const;
    void setHasPrivateCalls(bool hasPrivateCalls);

    bool hasPrivateForwards() const;
    void setHasPrivateForwards(bool hasPrivateForwards);

    bool hasRestrictedVoiceAndVideoNoteMessages() const;
    void setHasRestrictedVoiceAndVideoNoteMessages(bool hasRestrictedVoiceAndVideoNoteMessages);

    bool hasPostedToProfileStories() const;
    void setHasPostedToProfileStories(bool hasPostedToProfileStories);

    bool hasSponsoredMessagesEnabled() const;
    void setHasSponsoredMessagesEnabled(bool hasSponsoredMessagesEnabled);

    bool needPhoneNumberPrivacyException() const;
    void setNeedPhoneNumberPrivacyException(bool needPhoneNumberPrivacyException);

    bool setChatBackground() const;
    void setSetChatBackground(bool setChatBackground);

    QVariantMap bio() const;
    void setBio(const QVariantMap &bio);

    QVariantMap birthdate() const;
    void setBirthdate(const QVariantMap &birthdate);

    qint64 personalChatId() const;
    void setPersonalChatId(qint64 personalChatId);

    QVariantList premiumGiftOptions() const;
    void setPremiumGiftOptions(const QVariantList &premiumGiftOptions);

    int groupInCommonCount() const;
    void setGroupInCommonCount(int groupInCommonCount);

    QVariantMap businessInfo() const;
    void setBusinessInfo(const QVariantMap &businessInfo);

    QVariantMap botInfo() const;
    void setBotInfo(const QVariantMap &botInfo);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void personalPhotoChanged();
    void photoChanged();
    void publicPhotoChanged();
    void blockListChanged();
    void canBeCalledChanged();
    void supportsVideoCallsChanged();
    void hasPrivateCallsChanged();
    void hasPrivateForwardsChanged();
    void hasRestrictedVoiceAndVideoNoteMessagesChanged();
    void hasPostedToProfileStoriesChanged();
    void hasSponsoredMessagesEnabledChanged();
    void needPhoneNumberPrivacyExceptionChanged();
    void setChatBackgroundChanged();
    void bioChanged();
    void birthdateChanged();
    void personalChatIdChanged();
    void premiumGiftOptionsChanged();
    void groupInCommonCountChanged();
    void businessInfoChanged();
    void botInfoChanged();

private:
    QVariantMap m_personalPhoto;
    QVariantMap m_photo;
    QVariantMap m_publicPhoto;
    QVariantMap m_blockList;
    bool m_canBeCalled;
    bool m_supportsVideoCalls;
    bool m_hasPrivateCalls;
    bool m_hasPrivateForwards;
    bool m_hasRestrictedVoiceAndVideoNoteMessages;
    bool m_hasPostedToProfileStories;
    bool m_hasSponsoredMessagesEnabled;
    bool m_needPhoneNumberPrivacyException;
    bool m_setChatBackground;
    QVariantMap m_bio;
    QVariantMap m_birthdate;
    qint64 m_personalChatId;
    QVariantList m_premiumGiftOptions;
    int m_groupInCommonCount;
    QVariantMap m_businessInfo;
    QVariantMap m_botInfo;
};
