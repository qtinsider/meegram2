#include "Authorization.hpp"

#include "StorageManager.hpp"
#include "Utils.hpp"

Authorization::Authorization(QObject *parent)
    : QObject(parent)
{
    m_client = StorageManager::instance().client();

    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));
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
    m_client->send(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code.toStdString()), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            emit error(QString::fromStdString(td::move_tl_object_as<td::td_api::error>(response)->message_));
        }

        setLoading(false);
    });
}

void Authorization::checkPassword(const QString &password) noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password.toStdString()), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            emit error(QString::fromStdString(td::move_tl_object_as<td::td_api::error>(response)->message_));
        }

        setLoading(false);
    });
}

void Authorization::logOut() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::logOut>(), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            emit error(QString::fromStdString(td::move_tl_object_as<td::td_api::error>(response)->message_));
        }

        setLoading(false);
    });
}

void Authorization::registerUser(const QString &firstName, const QString &lastName) noexcept
{
    auto request = td::td_api::make_object<td::td_api::registerUser>();
    request->first_name_ = firstName.toStdString();
    request->last_name_ = lastName.toStdString();
    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            emit error(QString::fromStdString(td::move_tl_object_as<td::td_api::error>(response)->message_));
        }

        setLoading(false);
    });
}

void Authorization::setPhoneNumber(const QString &phoneNumber) noexcept
{
    auto request = td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>();
    request->phone_number_ = phoneNumber.toStdString();
    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            emit error(QString::fromStdString(td::move_tl_object_as<td::td_api::error>(response)->message_));
        }

        setLoading(false);
    });
}

void Authorization::resendCode() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::resendAuthenticationCode>(), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            emit error(QString::fromStdString(td::move_tl_object_as<td::td_api::error>(response)->message_));
        }

        setLoading(false);
    });
}

void Authorization::deleteAccount(const QString &reason) noexcept
{
    auto request = td::td_api::make_object<td::td_api::deleteAccount>();
    request->reason_ = reason.toStdString();
    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            emit error(QString::fromStdString(td::move_tl_object_as<td::td_api::error>(response)->message_));
        }

        setLoading(false);
    });
}

QString Authorization::formatTime(int totalSeconds) const noexcept
{
    return Utils::formatTime(totalSeconds);
}

void Authorization::handleResult(td::td_api::Object *object)
{
    if (object->get_id() == td::td_api::updateAuthorizationState::ID)
    {
        const auto *updateAuthState = static_cast<const td::td_api::updateAuthorizationState *>(object);
        const auto *authorizationState = updateAuthState->authorization_state_.get();

        switch (authorizationState->get_id())
        {
            case td::td_api::authorizationStateWaitPhoneNumber::ID:
                handleAuthorizationStateWaitPhoneNumber(static_cast<const td::td_api::authorizationStateWaitPhoneNumber *>(authorizationState));
                break;

            case td::td_api::authorizationStateWaitCode::ID:
                handleAuthorizationStateWaitCode(static_cast<const td::td_api::authorizationStateWaitCode *>(authorizationState));
                break;

            case td::td_api::authorizationStateWaitPassword::ID:
                handleAuthorizationStateWaitPassword(static_cast<const td::td_api::authorizationStateWaitPassword *>(authorizationState));
                break;

            case td::td_api::authorizationStateWaitRegistration::ID:
                handleAuthorizationStateWaitRegistration(static_cast<const td::td_api::authorizationStateWaitRegistration *>(authorizationState));
                break;

            case td::td_api::authorizationStateReady::ID:
                handleAuthorizationStateReady(static_cast<const td::td_api::authorizationStateReady *>(authorizationState));
                break;

            default:
                break;
        }
    }
    else if (object->get_id() == td::td_api::error::ID)
    {
    }
}

void Authorization::handleAuthorizationStateWaitPhoneNumber(const td::td_api::authorizationStateWaitPhoneNumber *)
{
    // Handle wait phone number state (empty function as per the original code)
}

namespace {
QVariantMap getCodeTypeMap(const td::td_api::AuthenticationCodeType &type)
{
    QVariantMap typeMap;

    switch (type.get_id())
    {
        case td::td_api::authenticationCodeTypeSms::ID:
            typeMap.insert("length", static_cast<const td::td_api::authenticationCodeTypeSms &>(type).length_);
            break;
        case td::td_api::authenticationCodeTypeCall::ID:
            typeMap.insert("length", static_cast<const td::td_api::authenticationCodeTypeCall &>(type).length_);
            break;
        case td::td_api::authenticationCodeTypeFlashCall::ID:
            typeMap.insert("pattern", QString::fromStdString(static_cast<const td::td_api::authenticationCodeTypeFlashCall &>(type).pattern_));
            break;
    }

    return typeMap;
}
}  // namespace

void Authorization::handleAuthorizationStateWaitCode(const td::td_api::authorizationStateWaitCode *authorizationState)
{
    if (!authorizationState || !authorizationState->code_info_)
        return;

    const auto &codeInfo = *authorizationState->code_info_;

    auto phoneNumber = QString::fromStdString(codeInfo.phone_number_);
    auto timeout = codeInfo.timeout_;

    QVariantMap type = codeInfo.type_ ? getCodeTypeMap(*codeInfo.type_) : QVariantMap();
    QVariantMap nextType = codeInfo.next_type_ ? getCodeTypeMap(*codeInfo.next_type_) : QVariantMap();

    emit codeRequested(phoneNumber, type, nextType, timeout);
}

void Authorization::handleAuthorizationStateWaitPassword(const td::td_api::authorizationStateWaitPassword *authorizationState)
{
    if (!authorizationState)
        return;

    QString passwordHint = QString::fromStdString(authorizationState->password_hint_);
    bool hasRecoveryEmail = authorizationState->has_recovery_email_address_;
    QString recoveryEmailPattern = QString::fromStdString(authorizationState->recovery_email_address_pattern_);

    emit passwordRequested(passwordHint, hasRecoveryEmail, recoveryEmailPattern);
}

void Authorization::handleAuthorizationStateWaitRegistration(const td::td_api::authorizationStateWaitRegistration *authorizationState)
{
    if (!authorizationState || !authorizationState->terms_of_service_)
        return;

    const auto &termsOfService = *authorizationState->terms_of_service_;
    const auto &text = *termsOfService.text_;

    QString termsText = QString::fromStdString(text.text_);
    int minUserAge = termsOfService.min_user_age_;
    bool showPopup = termsOfService.show_popup_;

    emit registrationRequested(termsText, minUserAge, showPopup);
}

void Authorization::handleAuthorizationStateReady(const td::td_api::authorizationStateReady *)
{
    emit ready();
}
