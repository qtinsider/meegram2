#include "Application.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "Serialize.hpp"
#include "Settings.hpp"
#include "StorageManager.hpp"

#include <QDir>
#include <QLocale>
#include <QSettings>
#include <QTimer>

Application::Application(QObject *parent)
    : QObject(parent)
    , m_client(new Client())
    , m_locale(new Locale())
    , m_settings(new Settings())
    , m_storageManager(new StorageManager())
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
    connect(m_client, SIGNAL(result(const QVariantMap &)), this, SLOT(handleResult(const QVariantMap &)));
    connect(m_client, SIGNAL(result(const QVariantMap &)), m_storageManager, SLOT(handleResult(const QVariantMap &)));

    m_storageManager->setClient(m_client);

    initializeParameters();
    initializeLanguagePack();
    initializeCountries();
}

bool Application::isAuthorized() const noexcept
{
    return m_isAuthorized;
}

Client *Application::client() const noexcept
{
    return m_client;
}

Locale *Application::locale() const noexcept
{
    return m_locale;
}

Settings *Application::settings() const noexcept
{
    return m_settings;
}

StorageManager *Application::storageManager() const noexcept
{
    return m_storageManager;
}

const QVariantList &Application::countries() const noexcept
{
    return m_countries;
}

void Application::close() noexcept
{
    QVariantMap request;
    request.insert("@type", "close");

    m_client->send(request);
}

void Application::setOption(const QString &name, const QVariant &value)
{
    QVariantMap optionValue;

    switch (value.type())
    {
        case QVariant::Bool:
            optionValue.insert("@type", "optionValueBoolean");
            optionValue.insert("value", value.toBool());
            break;
        case QVariant::Int:
        case QVariant::LongLong:
            optionValue.insert("@type", "optionValueInteger");
            optionValue.insert("value", value.toInt());
            break;
        case QVariant::String:
            optionValue.insert("@type", "optionValueString");
            optionValue.insert("value", value.toString());
            break;
        default:
            optionValue.insert("@type", "optionValueEmpty");
            optionValue.insert("value", QVariant());
    }

    QVariantMap request;
    request.insert("@type", "setOption");
    request.insert("name", name);
    request.insert("value", optionValue);

    m_client->send(request);
}

void Application::initializeParameters() noexcept
{
    QVariantMap request;
    request.insert("@type", "setTdlibParameters");
    request.insert("database_directory", QString(QDir::homePath() + DatabaseDirectory));
    request.insert("use_file_database", true);
    request.insert("use_chat_info_database", true);
    request.insert("use_message_database", true);
    request.insert("use_secret_chats", true);
    request.insert("api_id", ApiId);
    request.insert("api_hash", ApiHash);
    request.insert("system_language_code", DefaultLanguageCode);
    request.insert("device_model", DeviceModel);
    request.insert("system_version", SystemVersion);
    request.insert("application_version", AppVersion);

    m_client->send(request, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "ok")
        {
            m_initializationStatus[0] = true;
            checkInitializationStatus();
        }
    });
}

void Application::initializeLanguagePack() noexcept
{
    QVariantMap request;

    setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    setOption("localization_target", "android");
    setOption("language_pack_id", m_settings->languagePluralId());

    request.insert("@type", "getLanguagePackStrings");
    request.insert("language_pack_id", m_settings->languagePluralId());

    m_client->send(request, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "languagePackStrings")
        {
            m_locale->processStrings(value);

            m_initializationStatus[1] = true;
            checkInitializationStatus();
        }
    });
}

void Application::initializeCountries() noexcept
{
    QVariantMap request;
    request.insert("@type", "getCountries");

    m_client->send(request, [this](auto &&value) {
        if (value.value("@type").toByteArray() == "countries")
        {
            m_countries = std::move(value.value("countries").toList());
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

void Application::handleResult(const QVariantMap &object)
{
    const auto type = object.value("@type").toString();

    if (type == QLatin1String("updateAuthorizationState"))
    {
        handleAuthorizationState(object.value("authorization_state").toMap());
    }
}

void Application::handleAuthorizationState(const QVariantMap &authorizationState)
{
    static const std::unordered_map<std::string, std::function<void()>> handlers = {
        {"authorizationStateWaitEncryptionKey",
         [this]() {
             QVariantMap request;
             request.insert("@type", "checkDatabaseEncryptionKey");
             request.insert("encryption_key", "");
             m_client->send(request);
         }},
        {"authorizationStateReady",
         [this]() {
             QVariantMap request;
             request.insert("@type", "loadChats");
             request.insert("chat_list", {});
             request.insert("limit", ChatSliceLimit);
             m_client->send(request);

             m_isAuthorized = true;
             emit authorizedChanged();
         }},
    };

    const auto authorizationStateType = authorizationState.value("@type").toString().toStdString();

    if (const auto it = handlers.find(authorizationStateType); it != handlers.end())
    {
        it->second();
    }
}
