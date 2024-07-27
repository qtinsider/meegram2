#include "Authorization.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Serialize.hpp"
#include "StorageManager.hpp"
#include "Utils.hpp"

#include <QDebug>

Authorization::Authorization(Client *client, QObject *parent)
    : QObject(parent)
    , m_client(client)
{
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
    m_client->send(td::td_api::make_object<td::td_api::checkAuthenticationCode>(code.toStdString()), {});
}

void Authorization::checkPassword(const QString &password) noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::checkAuthenticationPassword>(password.toStdString()), {});
}

void Authorization::logOut() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::logOut>(), {});
}

void Authorization::registerUser(const QString &firstName, const QString &lastName) noexcept
{
    auto request = td::td_api::make_object<td::td_api::registerUser>();
    request->first_name_ = firstName.toStdString();
    request->last_name_ = lastName.toStdString();
    m_client->send(std::move(request), {});
}

void Authorization::setPhoneNumber(const QString &phoneNumber) noexcept
{
    auto request = td::td_api::make_object<td::td_api::setAuthenticationPhoneNumber>();
    request->phone_number_ = phoneNumber.toStdString();
    m_client->send(std::move(request), {});
}

void Authorization::resendCode() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::resendAuthenticationCode>(), {});
}

void Authorization::deleteAccount(const QString &reason) noexcept
{
    auto request = td::td_api::make_object<td::td_api::deleteAccount>();
    request->reason_ = reason.toStdString();
    m_client->send(std::move(request), {});
}

QString Authorization::formatTime(int totalSeconds) const noexcept
{
    // Placeholder for the actual implementation
    return QString();  // Utils::formatTime(totalSeconds);
}

void Authorization::handleResult(td::td_api::Object *object)
{
    static const std::unordered_map<int, std::function<void(const td::td_api::AuthorizationState *)>> handlers = {
        {td::td_api::authorizationStateWaitPhoneNumber::ID,
         [this](const td::td_api::AuthorizationState *state) {
             handleAuthorizationStateWaitPhoneNumber(static_cast<const td::td_api::authorizationStateWaitPhoneNumber *>(state));
         }},
        {td::td_api::authorizationStateWaitCode::ID,
         [this](const td::td_api::AuthorizationState *state) {
             handleAuthorizationStateWaitCode(static_cast<const td::td_api::authorizationStateWaitCode *>(state));
         }},
        {td::td_api::authorizationStateWaitPassword::ID,
         [this](const td::td_api::AuthorizationState *state) {
             handleAuthorizationStateWaitPassword(static_cast<const td::td_api::authorizationStateWaitPassword *>(state));
         }},
        {td::td_api::authorizationStateWaitRegistration::ID,
         [this](const td::td_api::AuthorizationState *state) {
             handleAuthorizationStateWaitRegistration(static_cast<const td::td_api::authorizationStateWaitRegistration *>(state));
         }},
        {td::td_api::authorizationStateReady::ID,
         [this](const td::td_api::AuthorizationState *state) {
             handleAuthorizationStateReady(static_cast<const td::td_api::authorizationStateReady *>(state));
         }},
    };

    if (object->get_id() == td::td_api::updateAuthorizationState::ID)
    {
        const auto authorizationState = static_cast<const td::td_api::updateAuthorizationState *>(object)->authorization_state_.get();

        if (const auto it = handlers.find(authorizationState->get_id()); it != handlers.end())
            it->second(authorizationState);
    }
    else if (object->get_id() == td::td_api::error::ID)
    {
        // Handle error state here
    }
}

void Authorization::handleAuthorizationStateWaitPhoneNumber(const td::td_api::authorizationStateWaitPhoneNumber *)
{
    // Handle wait phone number state (empty function as per the original code)
}

void Authorization::handleAuthorizationStateWaitCode(const td::td_api::authorizationStateWaitCode *authorizationState)
{
    const auto &codeInfo = authorizationState->code_info_;

    auto phoneNumber = QString::fromStdString(codeInfo->phone_number_);
    auto timeout = codeInfo->timeout_;

    QVariantMap type;
    QVariantMap nextType;

    if (codeInfo->type_)
    {
        switch (codeInfo->type_->get_id())
        {
            case td::td_api::authenticationCodeTypeSms::ID:
                type.insert("length", static_cast<const td::td_api::authenticationCodeTypeSms *>(codeInfo->type_.get())->length_);
                break;
            case td::td_api::authenticationCodeTypeCall::ID:
                type.insert("length", static_cast<const td::td_api::authenticationCodeTypeCall *>(codeInfo->type_.get())->length_);
                break;
            case td::td_api::authenticationCodeTypeFlashCall::ID:
                type.insert("pattern",
                            QString::fromStdString(static_cast<const td::td_api::authenticationCodeTypeFlashCall *>(codeInfo->type_.get())->pattern_));
                break;
        }
    }

    if (codeInfo->next_type_)
    {
        switch (codeInfo->next_type_->get_id())
        {
            case td::td_api::authenticationCodeTypeSms::ID:
                nextType.insert("length", static_cast<const td::td_api::authenticationCodeTypeSms *>(codeInfo->next_type_.get())->length_);
                break;
            case td::td_api::authenticationCodeTypeCall::ID:
                nextType.insert("length", static_cast<const td::td_api::authenticationCodeTypeCall *>(codeInfo->next_type_.get())->length_);
                break;
            case td::td_api::authenticationCodeTypeFlashCall::ID:
                nextType.insert("pattern",
                                QString::fromStdString(static_cast<const td::td_api::authenticationCodeTypeFlashCall *>(codeInfo->next_type_.get())->pattern_));
                break;
        }
    }

    emit codeRequested(phoneNumber, type, nextType, timeout);
}

void Authorization::handleAuthorizationStateWaitPassword(const td::td_api::authorizationStateWaitPassword *authorizationState)
{
    emit passwordRequested(QString::fromStdString(authorizationState->password_hint_), authorizationState->has_recovery_email_address_,
                           QString::fromStdString(authorizationState->recovery_email_address_pattern_));
}

void Authorization::handleAuthorizationStateWaitRegistration(const td::td_api::authorizationStateWaitRegistration *authorizationState)
{
    emit registrationRequested(QString::fromStdString(authorizationState->terms_of_service_->text_->text_),
                               authorizationState->terms_of_service_->min_user_age_, authorizationState->terms_of_service_->show_popup_);
}

void Authorization::handleAuthorizationStateReady(const td::td_api::authorizationStateReady *)
{
    emit ready();
}
