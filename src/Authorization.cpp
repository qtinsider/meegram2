#include "Authorization.hpp"

#include "Client.hpp"

Authorization::Authorization(std::shared_ptr<Client> client, QObject *parent)
    : QObject(parent)
    , m_client(std::move(client))
{
    m_responseCallback = [this](auto response) {
        if (response->get_id() == td::td_api::error::ID)
        {
            auto result = td::td_api::move_object_as<td::td_api::error>(response);
            emit error(result->code_, QString::fromStdString(result->message_));
        }
    };

    connect(m_client.get(), SIGNAL(result(td::td_api::Object *)), SLOT(handleResult(td::td_api::Object *)));
}

QString Authorization::state() const noexcept
{
    return m_state;
}

QVariant Authorization::content() const noexcept
{
    return m_content;
}

QObject *Authorization::countryModel() const noexcept
{
    return m_countryModel.get();
}

void Authorization::setState(const QString &value) noexcept
{
    if (m_state != value)
    {
        m_state = value;
        emit stateChanged();
    }
}

void Authorization::checkCode(const QString &code) noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code.toStdString()), m_responseCallback);
}

void Authorization::checkPassword(const QString &password) noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password.toStdString()), m_responseCallback);
}

void Authorization::requestQrCode() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::requestQrCodeAuthentication>(), m_responseCallback);
}

void Authorization::logOut() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::logOut>(), m_responseCallback);
}

void Authorization::registerUser(const QString &firstName, const QString &lastName) noexcept
{
    auto request = td::td_api::make_object<td::td_api::registerUser>();
    request->first_name_ = firstName.toStdString();
    request->last_name_ = lastName.toStdString();

    m_client->send(std::move(request), m_responseCallback);
}

void Authorization::setPhoneNumber(const QString &phoneNumber) noexcept
{
    auto request = td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>();
    request->phone_number_ = phoneNumber.toStdString();

    m_client->send(std::move(request), m_responseCallback);
}

void Authorization::resendCode() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::resendAuthenticationCode>(), m_responseCallback);
}

void Authorization::_destroy() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::destroy>(), m_responseCallback);
}

void Authorization::deleteAccount(const QString &reason) noexcept
{
    auto request = td::td_api::make_object<td::td_api::deleteAccount>();
    request->reason_ = reason.toStdString();

    m_client->send(std::move(request), m_responseCallback);
}

void Authorization::initializeCountryModel()
{
    if (!m_countryModel)
    {
        m_countryModel = std::make_unique<CountryModel>(m_client, this);
    }
}

void Authorization::handleResult(td::td_api::Object *object)
{
    switch (object->get_id())
    {
        case td::td_api::updateAuthorizationState::ID: {
            const auto updateAuthState = static_cast<const td::td_api::updateAuthorizationState *>(object);
            const auto authorizationState = updateAuthState->authorization_state_.get();

            switch (authorizationState->get_id())
            {
                case td::td_api::authorizationStateWaitPhoneNumber::ID:
                    // setState("phone_number");
                    break;
                case td::td_api::authorizationStateWaitCode::ID:
                    handleAuthorizationStateWaitCode(static_cast<const td::td_api::authorizationStateWaitCode *>(authorizationState));
                    setState("code");
                    break;
                case td::td_api::authorizationStateWaitOtherDeviceConfirmation::ID:
                    handleAuthorizationStateWaitOtherDeviceConfirmation(
                        static_cast<const td::td_api::authorizationStateWaitOtherDeviceConfirmation *>(authorizationState));
                    // setState("other_device_confirmation");
                    break;
                case td::td_api::authorizationStateWaitPassword::ID:
                    handleAuthorizationStateWaitPassword(static_cast<const td::td_api::authorizationStateWaitPassword *>(authorizationState));
                    setState("password");
                    break;
                case td::td_api::authorizationStateWaitRegistration::ID:
                    handleAuthorizationStateWaitRegistration(static_cast<const td::td_api::authorizationStateWaitRegistration *>(authorizationState));
                    setState("registration");
                    break;
                case td::td_api::authorizationStateReady::ID:
                    setState("ready");
                    break;
                case td::td_api::authorizationStateClosing::ID:
                    setState("closing");
                    break;
                case td::td_api::authorizationStateClosed::ID:
                    setState("closed");
                    break;
                default:
                    break;
            }
            break;
        }
        case td::td_api::error::ID:
            break;
        default:
            break;
    }
}

void Authorization::handleAuthorizationStateWaitCode(const td::td_api::authorizationStateWaitCode *authorizationState)
{
    if (!authorizationState || !authorizationState->code_info_)
        return;

    const auto &codeInfo = *authorizationState->code_info_;

    QVariantMap content;
    content.insert("phoneNumber", QString::fromStdString(codeInfo.phone_number_));
    content.insert("timeout", codeInfo.timeout_);
    content.insert("type", codeInfo.type_ ? getCodeTypeMap(*codeInfo.type_) : QVariantMap());
    content.insert("nextType", codeInfo.next_type_ ? getCodeTypeMap(*codeInfo.next_type_) : QVariantMap());

    m_content = std::move(content);

    emit contentChanged();
}

void Authorization::handleAuthorizationStateWaitOtherDeviceConfirmation(const td::td_api::authorizationStateWaitOtherDeviceConfirmation *authorizationState)
{
    if (!authorizationState)
        return;

    QVariantMap content;
    content.insert("link", QString::fromStdString(authorizationState->link_));

    m_content = std::move(content);

    emit contentChanged();
}

void Authorization::handleAuthorizationStateWaitPassword(const td::td_api::authorizationStateWaitPassword *authorizationState)
{
    if (!authorizationState)
        return;

    QVariantMap content;
    content.insert("passwordHint", QString::fromStdString(authorizationState->password_hint_));
    content.insert("hasRecoveryEmail", authorizationState->has_recovery_email_address_);
    content.insert("recoveryEmailPattern", QString::fromStdString(authorizationState->recovery_email_address_pattern_));

    m_content = std::move(content);

    emit contentChanged();
}

void Authorization::handleAuthorizationStateWaitRegistration(const td::td_api::authorizationStateWaitRegistration *authorizationState)
{
    if (!authorizationState || !authorizationState->terms_of_service_)
        return;

    const auto &termsOfService = *authorizationState->terms_of_service_;
    const auto &text = *termsOfService.text_;

    QVariantMap content;
    content.insert("termsText", QString::fromStdString(text.text_));
    content.insert("minUserAge", termsOfService.min_user_age_);
    content.insert("showPopup", termsOfService.show_popup_);

    m_content = std::move(content);

    emit contentChanged();
}

QVariantMap Authorization::getCodeTypeMap(const td::td_api::AuthenticationCodeType &type)
{
    QVariantMap typeMap;

    switch (type.get_id())
    {
        case td::td_api::authenticationCodeTypeTelegramMessage::ID:
            typeMap.insert("type", "TelegramMessage");
            typeMap.insert("length", static_cast<const td::td_api::authenticationCodeTypeTelegramMessage &>(type).length_);
            break;
        case td::td_api::authenticationCodeTypeSms::ID:
            typeMap.insert("type", "Sms");
            typeMap.insert("length", static_cast<const td::td_api::authenticationCodeTypeSms &>(type).length_);
            break;
        case td::td_api::authenticationCodeTypeCall::ID:
            typeMap.insert("type", "Call");
            typeMap.insert("length", static_cast<const td::td_api::authenticationCodeTypeCall &>(type).length_);
            break;
        case td::td_api::authenticationCodeTypeFlashCall::ID:
            typeMap.insert("type", "FlashCall");
            typeMap.insert("pattern", QString::fromStdString(static_cast<const td::td_api::authenticationCodeTypeFlashCall &>(type).pattern_));
            break;
    }

    return typeMap;
}
