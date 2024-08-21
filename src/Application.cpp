#include "Application.hpp"

#include "Common.hpp"
#include "StorageManager.hpp"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QStringList>

#include <algorithm>

Application::Application(QObject *parent)
    : QObject(parent)
{
    m_storageManager = &StorageManager::instance();

    m_client = m_storageManager->client();
    m_locale = m_storageManager->locale();
    m_settings = m_storageManager->settings();

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));

    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    connect(m_settings, SIGNAL(languagePackIdChanged()), this, SIGNAL(languageChanged()));
    connect(m_settings, SIGNAL(languagePackIdChanged()), this, SLOT(initializeLanguagePack()));
}

bool Application::isAuthorized() const noexcept
{
    return m_isAuthorized;
}

QString Application::emptyString() const noexcept
{
    return {};
}

const QString &Application::connectionStateString() const noexcept
{
    return m_connectionStateString;
}

QString Application::getString(const QString &key) const noexcept
{
    return m_locale->getString(key);
}

void Application::close() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::close>(), {});
}

void Application::setOption(const QString &name, const QVariant &value)
{
    td::td_api::object_ptr<td::td_api::OptionValue> optionValue;

    switch (value.type())
    {
        case QVariant::Bool:
            optionValue = td::td_api::make_object<td::td_api::optionValueBoolean>(value.toBool());
            break;
        case QVariant::Int:
        case QVariant::LongLong:
            optionValue = td::td_api::make_object<td::td_api::optionValueInteger>(value.toLongLong());
            break;
        case QVariant::String:
            optionValue = td::td_api::make_object<td::td_api::optionValueString>(value.toString().toStdString());
            break;
        default:
            optionValue = td::td_api::make_object<td::td_api::optionValueEmpty>();
    }

    m_client->send(td::td_api::make_object<td::td_api::setOption>(name.toStdString(), std::move(optionValue)), {});
}

void Application::initialize() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::getOption>("version"), {});

    setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    setOption("localization_target", "android");
    setOption("language_pack_id", m_settings->languagePackId());

    initializeParameters();
    initializeLanguagePack();
    initializeCountries();
    initializeLanguagePackInfo();
}

void Application::initializeParameters() noexcept
{
    auto request = td::td_api::make_object<td::td_api::setTdlibParameters>();

    request->database_directory_ = QString(QDir::homePath() + DatabaseDirectory).toStdString();
    request->use_file_database_ = true;
    request->use_chat_info_database_ = true;
    request->use_message_database_ = true;
    request->use_secret_chats_ = true;
    request->api_id_ = ApiId;
    request->api_hash_ = ApiHash;
    request->system_language_code_ = "en";
    request->device_model_ = DeviceModel;
    request->system_version_ = SystemVersion;
    request->application_version_ = AppVersion;

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::ok::ID)
        {
            m_initializationStatus[0] = true;
            checkInitializationStatus();
        }
    });
}

void Application::initializeLanguagePack() noexcept
{
    auto request = td::td_api::make_object<td::td_api::getLanguagePackStrings>();

    request->language_pack_id_ = m_settings->languagePackId().toStdString();

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::languagePackStrings::ID)
        {
            m_locale->setLanguagePlural(m_settings->languagePluralId());
            m_locale->setLanguagePackStrings(td::move_tl_object_as<td::td_api::languagePackStrings>(response));

            if (!m_initializationStatus[1])
            {
                m_initializationStatus[1] = true;
                checkInitializationStatus();
            }
        }
    });
}

void Application::initializeCountries() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::getCountries>(), [this](auto &&response) {
        if (response->get_id() == td::td_api::countries::ID)
        {
            m_storageManager->setCountries(td::move_tl_object_as<td::td_api::countries>(response));

            m_initializationStatus[2] = true;
            checkInitializationStatus();
        }
    });
}

void Application::checkInitializationStatus() noexcept
{
    if (std::all_of(m_initializationStatus.begin(), m_initializationStatus.end(), [](bool status) { return status; }))
    {
        emit appInitialized();
    }
}

void Application::handleResult(td::td_api::Object *object)
{
    if (object->get_id() == td::td_api::updateAuthorizationState::ID)
    {
        handleAuthorizationState(*static_cast<const td::td_api::updateAuthorizationState *>(object)->authorization_state_);
    }

    if (object->get_id() == td::td_api::updateConnectionState::ID)
    {
        handleConnectionState(*static_cast<const td::td_api::updateConnectionState *>(object)->state_);
    }
}

void Application::initializeLanguagePackInfo()
{
    auto request = td::td_api::make_object<td::td_api::getLocalizationTargetInfo>();
    request->only_local_ = true;

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::localizationTargetInfo::ID)
        {
            m_storageManager->setLanguagePackInfo(td::move_tl_object_as<td::td_api::localizationTargetInfo>(response));

            m_initializationStatus[3] = true;
            checkInitializationStatus();
        }
    });
}

void Application::handleAuthorizationState(const td::td_api::AuthorizationState &authorizationState)
{
    if (authorizationState.get_id() == td::td_api::authorizationStateReady::ID)
    {
        auto request = td::td_api::make_object<td::td_api::loadChats>();

        request->limit_ = ChatSliceLimit;

        m_client->send(std::move(request), {});

        m_isAuthorized = true;
        emit authorizedChanged();
    }
}

void Application::handleConnectionState(const td::td_api::ConnectionState &connectionState)
{
    static const std::unordered_map<int, std::string> stateMap = {{td::td_api::connectionStateReady::ID, "Ready"},
                                                                  {td::td_api::connectionStateConnecting::ID, "Connecting"},
                                                                  {td::td_api::connectionStateUpdating::ID, "Updating"},
                                                                  {td::td_api::connectionStateWaitingForNetwork::ID, "WaitingForNetwork"}};

    if (const auto it = stateMap.find(connectionState.get_id()); it != stateMap.end())
    {
        m_connectionStateString = QString::fromStdString(it->second);
        emit connectionStateChanged();
    }
}
