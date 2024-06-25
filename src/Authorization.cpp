#include "Authorization.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "StorageManager.hpp"
#include "TdApi.hpp"

#include <QDebug>

Authorization::Authorization(QObject *parent)
    : QObject(parent)
{
}

TdManager *Authorization::manager() const
{
    return m_manager;
}

void Authorization::setManager(TdManager *manager)
{
    m_manager = manager;
    m_client = m_manager->storageManager()->client();

    connect(m_client, SIGNAL(result(const QVariantMap &)), SLOT(handleResult(const QVariantMap &)));
}

bool Authorization::loading() const
{
    return m_loading;
}

void Authorization::setLoading(bool value)
{
    if (m_loading != value)
    {
        m_loading = value;
        emit loadingChanged();
    }
}

void Authorization::checkCode(const QString &code) noexcept
{
    QVariantMap request;
    request.insert("@type", "checkAuthenticationCode");
    request.insert("code", code);

    m_manager->sendRequest(request);
}

void Authorization::checkPassword(const QString &password) noexcept
{
    QVariantMap request;
    request.insert("@type", "checkAuthenticationPassword");
    request.insert("password", password);

    m_manager->sendRequest(request);
}

void Authorization::logOut() noexcept
{
    QVariantMap request;
    request.insert("@type", "logOut");

    m_manager->sendRequest(request);
}

void Authorization::registerUser(const QString &firstName, const QString &lastName) noexcept
{
    QVariantMap request;
    request.insert("@type", "registerUser");
    request.insert("first_name", firstName);
    request.insert("last_name", lastName);

    m_manager->sendRequest(request);
}

void Authorization::setPhoneNumber(const QString &phoneNumber) noexcept
{
    QVariantMap request;
    request.insert("@type", "setAuthenticationPhoneNumber");
    request.insert("phone_number", phoneNumber);

    m_manager->sendRequest(request);
}

void Authorization::resendCode() noexcept
{
    QVariantMap request;
    request.insert("@type", "resendAuthenticationCode");

    m_manager->sendRequest(request);
}

void Authorization::deleteAccount(const QString &reason) noexcept
{
    QVariantMap request;
    request.insert("@type", "deleteAccount");
    request.insert("reason", reason);

    m_manager->sendRequest(request);
}

QString Authorization::formatTime(int totalSeconds) const noexcept
{
    QString result;
    QTextStream stream(&result);

    auto appendDuration = [&stream](int count, QChar order) {
        if (count > 0)
        {
            if (!stream.string()->isEmpty())
            {
                stream << " ";
            }
            stream << count << order;
        }
    };

    int seconds = totalSeconds % 60;
    int timeoutMinutes = totalSeconds / 60;
    int minutes = timeoutMinutes % 60;
    int timeoutHours = timeoutMinutes / 60;
    int hours = timeoutHours % 24;
    int days = timeoutHours / 24;

    appendDuration(days, 'd');
    appendDuration(hours, 'h');
    appendDuration(minutes, 'm');
    appendDuration(seconds, 's');

    return result;
}

void Authorization::handleResult(const QVariantMap &object)
{
    if (const auto objectType = object.value("@type").toByteArray(); objectType != "updateAuthorizationState")
        return;

    const auto authorizationState = object.value("authorization_state").toMap();
    const auto authorizationStateType = authorizationState.value("@type").toString().toStdString();

    static const std::unordered_map<std::string, std::function<void(const QVariantMap &)>> handlers = {
        {"authorizationStateWaitPhoneNumber", [this](const QVariantMap &state) { handleAuthorizationStateWaitPhoneNumber(state); }},
        {"authorizationStateWaitCode", [this](const QVariantMap &state) { handleAuthorizationStateWaitCode(state); }},
        {"authorizationStateWaitPassword", [this](const QVariantMap &state) { handleAuthorizationStateWaitPassword(state); }},
        {"authorizationStateWaitRegistration", [this](const QVariantMap &state) { handleAuthorizationStateWaitRegistration(state); }},
        {"authorizationStateReady", [this](const QVariantMap &state) { handleAuthorizationStateReady(state); }},
    };

    if (const auto it = handlers.find(authorizationStateType); it != handlers.end())
    {
        it->second(authorizationState);
    }
}

void Authorization::handleAuthorizationStateWaitPhoneNumber(const QVariantMap &)
{
    // Handle wait phone number state (empty function as per the original code)
}

void Authorization::handleAuthorizationStateWaitCode(const QVariantMap &authorizationState)
{
    const auto codeInfo = authorizationState.value("code_info").toMap();
    const auto phoneNumber = codeInfo.value("phone_number").toString();
    const auto timeout = codeInfo.value("timeout").toInt();

    QVariantMap type;
    QVariantMap nextType;

    const auto typeMap = codeInfo.value("type").toMap();
    type.insert("type", typeMap.value("@type").toString());
    type.insert("length", typeMap.value("length").toString());

    if (!codeInfo.value("next_type").isNull())
    {
        const auto nextTypeMap = codeInfo.value("next_type").toMap();
        nextType.insert("type", nextTypeMap.value("@type").toString());
        nextType.insert("length", nextTypeMap.value("length").toString());
    }

    emit codeRequested(phoneNumber, type, nextType, timeout);
}

void Authorization::handleAuthorizationStateWaitPassword(const QVariantMap &authorizationState)
{
    const auto password = authorizationState.value("password").toMap();
    const auto passwordHint = password.value("password_hint").toString();
    const auto hasRecoveryEmailAddress = password.value("has_recovery_email_address").toBool();
    const auto recoveryEmailAddressPattern = password.value("recovery_email_address_pattern").toString();

    emit passwordRequested(passwordHint, hasRecoveryEmailAddress, recoveryEmailAddressPattern);
}

void Authorization::handleAuthorizationStateWaitRegistration(const QVariantMap &authorizationState)
{
    const auto termsOfService = authorizationState.value("terms_of_service").toMap();
    const auto text = termsOfService.value("text").toMap().value("text").toString();
    const auto minUserAge = termsOfService.value("min_user_age").toInt();
    const auto showPopup = termsOfService.value("show_popup").toBool();

    emit registrationRequested(text, minUserAge, showPopup);
}

void Authorization::handleAuthorizationStateReady(const QVariantMap &)
{
    emit ready();
}
