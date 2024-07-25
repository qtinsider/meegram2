#include "Application.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "Serialize.hpp"
#include "Settings.hpp"
#include "StorageManager.hpp"
#include "User.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QStringList>

#include <algorithm>

Application::Application(StorageManager *storageManager, QObject *parent)
    : QObject(parent)
    , m_locale(new Locale(this))
    , m_settings(new Settings(this))
    , m_storageManager(storageManager)
{
    m_client = qobject_cast<Client *>(m_storageManager->client());

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
    connect(m_settings, SIGNAL(languagePackIdChanged()), this, SIGNAL(languageChanged()));
    connect(m_settings, SIGNAL(languagePackIdChanged()), this, SLOT(initializeLanguagePack()));
    connect(m_client, SIGNAL(result(const QVariantMap &)), this, SLOT(handleResult(const QVariantMap &)));
}

bool Application::isAuthorized() const noexcept
{
    return m_isAuthorized;
}

QString Application::emptyString() const noexcept
{
    return {};
}

QObject *Application::client() const noexcept
{
    return m_client;
}

QObject *Application::locale() const noexcept
{
    return m_locale;
}

QObject *Application::settings() const noexcept
{
    return m_settings;
}

QObject *Application::storageManager() const noexcept
{
    return m_storageManager;
}

const QVariantList &Application::countries() const noexcept
{
    return m_countries;
}

const QVariantList &Application::languagePackInfo() const noexcept
{
    return m_languagePackInfo;
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

void Application::initialize() noexcept
{
    QVariantMap request;
    request.insert("@type", "getOption");
    request.insert("name", "version");
    m_client->send(request);

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

    request.insert("@type", "getLanguagePackStrings");
    request.insert("language_pack_id", m_settings->languagePackId());

    m_client->send(request, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "languagePackStrings")
        {
            m_locale->setLanguagePlural(m_settings->languagePluralId());
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
    if (type == QLatin1String("updateConnectionState"))
    {
        handleConnectionState(object.value("state").toMap());
    }
}

void Application::initializeLanguagePackInfo()
{
    QVariantMap request;
    request.insert("@type", "getLocalizationTargetInfo");
    request.insert("only_local", true);

    m_client->send(request, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "localizationTargetInfo")
        {
            m_languagePackInfo = std::move(value.value("language_packs").toList());

            m_initializationStatus[3] = true;
            checkInitializationStatus();
        }
    });
}

void Application::handleAuthorizationState(const QVariantMap &authorizationState)
{
    const auto authorizationStateType = authorizationState.value("@type").toString();

    static const std::unordered_map<QString, std::function<void()>> handlers = {
        {"authorizationStateWaitEncryptionKey",
         [this]() {
             QVariantMap request;
             request.insert("@type", "checkDatabaseEncryptionKey");
             request.insert("encryption_key", "changeMe123");
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

    if (const auto it = handlers.find(authorizationStateType); it != handlers.end())
    {
        it->second();
    }
}

void Application::handleConnectionState(const QVariantMap &connectionState)
{
    const auto connectionStateType = connectionState.value("@type").toString();

    static const std::unordered_map<QString, std::function<void()>> handlers = {
        {"connectionStateWaitingForNetwork",
         [this]() {
             m_connectionStateString = "waiting_for_network";
             emit connectionStateChanged();
         }},
        {"connectionStateConnectingToProxy",
         [this]() {
             m_connectionStateString = "connecting_to_proxy";
             emit connectionStateChanged();
         }},
        {"connectionStateConnecting",
         [this]() {
             m_connectionStateString = "connecting";
             emit connectionStateChanged();
         }},
        {"connectionStateUpdating",
         [this]() {
             m_connectionStateString = "updating";
             emit connectionStateChanged();
         }},
        {"connectionStateReady",
         [this]() {
             m_connectionStateString = "ready";
             emit connectionStateChanged();
         }},
    };

    if (const auto it = handlers.find(connectionStateType); it != handlers.end())
    {
        it->second();
    }
}
