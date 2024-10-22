#pragma once

#include <td/telegram/td_api.h>

#include <QObject>
#include <QVariant>

class UserFullInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QVariantMap personalPhoto READ personalPhoto NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantMap photo READ photo NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantMap publicPhoto READ publicPhoto NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantMap blockList READ blockList NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool canBeCalled READ canBeCalled NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool supportsVideoCalls READ supportsVideoCalls NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool hasPrivateCalls READ hasPrivateCalls NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool hasPrivateForwards READ hasPrivateForwards NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool hasRestrictedVoiceAndVideoNoteMessages READ hasRestrictedVoiceAndVideoNoteMessages NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool hasPostedToProfileStories READ hasPostedToProfileStories NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool hasSponsoredMessagesEnabled READ hasSponsoredMessagesEnabled NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool needPhoneNumberPrivacyException READ needPhoneNumberPrivacyException NOTIFY userFullInfoChanged)
    Q_PROPERTY(bool setChatBackground READ setChatBackground NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantMap bio READ bio NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantMap birthdate READ birthdate NOTIFY userFullInfoChanged)
    Q_PROPERTY(qlonglong personalChatId READ personalChatId NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantList premiumGiftOptions READ premiumGiftOptions NOTIFY userFullInfoChanged)
    Q_PROPERTY(int groupInCommonCount READ groupInCommonCount NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantMap businessInfo READ businessInfo NOTIFY userFullInfoChanged)
    Q_PROPERTY(QVariantMap botInfo READ botInfo NOTIFY userFullInfoChanged)

public:
    explicit UserFullInfo(td::td_api::object_ptr<td::td_api::userFullInfo> userFullInfo, QObject *parent = nullptr);

    QVariantMap personalPhoto() const;
    QVariantMap photo() const;
    QVariantMap publicPhoto() const;
    QVariantMap blockList() const;
    bool canBeCalled() const;
    bool supportsVideoCalls() const;
    bool hasPrivateCalls() const;
    bool hasPrivateForwards() const;
    bool hasRestrictedVoiceAndVideoNoteMessages() const;
    bool hasPostedToProfileStories() const;
    bool hasSponsoredMessagesEnabled() const;
    bool needPhoneNumberPrivacyException() const;
    bool setChatBackground() const;
    QVariantMap bio() const;
    QVariantMap birthdate() const;
    qlonglong personalChatId() const;
    QVariantList premiumGiftOptions() const;
    int groupInCommonCount() const;
    QVariantMap businessInfo() const;
    QVariantMap botInfo() const;

signals:
    void userFullInfoChanged();

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
    qlonglong m_personalChatId;
    QVariantList m_premiumGiftOptions;
    int m_groupInCommonCount;
    QVariantMap m_businessInfo;
    QVariantMap m_botInfo;
};
