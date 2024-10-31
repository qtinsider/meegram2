#pragma once

#include "Client.hpp"

#include <td/telegram/td_api.h>

#include <QVariant>

#include <functional>
#include <memory>

class Authorization : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString state READ state WRITE setState NOTIFY stateChanged)
    Q_PROPERTY(QVariant content READ content NOTIFY contentChanged)

public:
    explicit Authorization(QObject *parent = nullptr);

    [[nodiscard]] QString state() const noexcept;
    [[nodiscard]] QVariant content() const noexcept;

    void setState(const QString &value) noexcept;

    Q_INVOKABLE void checkCode(const QString &code) noexcept;
    Q_INVOKABLE void checkPassword(const QString &password) noexcept;
    Q_INVOKABLE void requestQrCode() noexcept;
    Q_INVOKABLE void logOut() noexcept;
    Q_INVOKABLE void registerUser(const QString &firstName, const QString &lastName) noexcept;
    Q_INVOKABLE void setPhoneNumber(const QString &phoneNumber) noexcept;
    Q_INVOKABLE void resendCode() noexcept;
    Q_INVOKABLE void destroy() noexcept;
    Q_INVOKABLE void deleteAccount(const QString &reason) noexcept;

signals:
    void error(int code, const QString &message);

    void stateChanged();
    void contentChanged();

private slots:
    void handleResult(td::td_api::Object *object);

private:
    void handleAuthorizationStateWaitCode(const td::td_api::authorizationStateWaitCode *state);
    void handleAuthorizationStateWaitOtherDeviceConfirmation(const td::td_api::authorizationStateWaitOtherDeviceConfirmation *state);
    void handleAuthorizationStateWaitPassword(const td::td_api::authorizationStateWaitPassword *state);
    void handleAuthorizationStateWaitRegistration(const td::td_api::authorizationStateWaitRegistration *state);

    static QVariantMap getCodeTypeMap(const td::td_api::AuthenticationCodeType &type);

    QString m_state;
    QVariant m_content;

    std::shared_ptr<Client> m_client;

    std::function<void(td::td_api::object_ptr<td::td_api::Object>)> m_responseCallback;
};
