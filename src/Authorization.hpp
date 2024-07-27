#pragma once

#include <td/telegram/td_api.h>

#include <QObject>
#include <QVariant>

class Client;

class Authorization : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool loading READ loading WRITE setLoading NOTIFY loadingChanged)
public:
    explicit Authorization(Client *client, QObject *parent = nullptr);

    bool loading() const;

    Q_INVOKABLE void checkCode(const QString &code) noexcept;
    Q_INVOKABLE void checkPassword(const QString &password) noexcept;
    Q_INVOKABLE void logOut() noexcept;
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName) noexcept;
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber) noexcept;
    Q_INVOKABLE void resendCode() noexcept;
    Q_INVOKABLE void deleteAccount(const QString &reason) noexcept;

signals:
    void codeRequested(const QString &phoneNumber, const QVariantMap &type, const QVariantMap &nextType, int timeout);
    void passwordRequested(const QString &passwordHint, bool hasRecoveryEmailAddress, const QString &recoveryEmailAddressPattern);
    void registrationRequested(const QString &text, int minUserAge, bool showPopup);
    void ready();
    void loggingOut();

    void error(const QString &errorString);

    void loadingChanged();

public slots:
    QString formatTime(int totalSeconds) const noexcept;

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void setLoading(bool value);

    void handleAuthorizationStateWaitPhoneNumber(const td::td_api::authorizationStateWaitPhoneNumber *authorizationState);
    void handleAuthorizationStateWaitCode(const td::td_api::authorizationStateWaitCode *authorizationState);
    void handleAuthorizationStateWaitPassword(const td::td_api::authorizationStateWaitPassword *authorizationState);
    void handleAuthorizationStateWaitRegistration(const td::td_api::authorizationStateWaitRegistration *authorizationState);
    void handleAuthorizationStateReady(const td::td_api::authorizationStateReady *authorizationState);

    Client *m_client{};

    bool m_loading = true;
};
