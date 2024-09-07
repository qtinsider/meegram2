#pragma once

#include "Client.hpp"

#include <td/telegram/td_api.h>

#include <QVariant>

#include <functional>
#include <memory>

class Authorization : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
public:
    explicit Authorization(QObject *parent = nullptr);

    [[nodiscard]] bool loading() const noexcept;
    void setLoading(bool value) noexcept;

    Q_INVOKABLE void checkCode(const QString &code) noexcept;
    Q_INVOKABLE void checkPassword(const QString &password) noexcept;
    Q_INVOKABLE void requestQrCode() noexcept;
    Q_INVOKABLE void logOut() noexcept;
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName) noexcept;
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber) noexcept;
    Q_INVOKABLE void resendCode() noexcept;
    Q_INVOKABLE void deleteAccount(const QString &reason) noexcept;

    Q_INVOKABLE QString formatTime(int totalSeconds) const noexcept;

signals:
    void error(int code, const QString &message);
    void qrCodeRequested(const QString &link);
    void codeRequested(const QString &phoneNumber, const QVariantMap &type, const QVariantMap &nextType, int timeout);
    void passwordRequested(const QString &passwordHint, bool hasRecoveryEmailAddress, const QString &recoveryEmailAddressPattern);
    void registrationRequested(const QString &text, int minUserAge, bool showPopup);
    void ready();
    void loadingChanged();

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void handleAuthorizationStateWaitPhoneNumber(const td::td_api::authorizationStateWaitPhoneNumber *state);
    void handleAuthorizationStateWaitCode(const td::td_api::authorizationStateWaitCode *state);
    void handleAuthorizationStateWaitOtherDeviceConfirmation(const td::td_api::authorizationStateWaitOtherDeviceConfirmation *state);
    void handleAuthorizationStateWaitPassword(const td::td_api::authorizationStateWaitPassword *state);
    void handleAuthorizationStateWaitRegistration(const td::td_api::authorizationStateWaitRegistration *state);
    void handleAuthorizationStateReady(const td::td_api::authorizationStateReady *state);

    static QVariantMap getCodeTypeMap(const td::td_api::AuthenticationCodeType &type);

    bool m_loading{false};

    Client* m_client;

    std::function<void(td::td_api::object_ptr<td::td_api::Object>)> m_responseCallback;
};
