#include "TdManager.hpp"

#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "Serialize.hpp"
#include "StorageManager.hpp"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QSettings>
#include <QStringBuilder>
#include <QTimer>

TdManager::TdManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_networkConfigManager(new QNetworkConfigurationManager(this))
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
    
    connect(m_networkConfigManager, configurationChanged(const QNetworkConfiguration &), this, SLOT(handleNetworkConfiguration(const QNetworkConfiguration &)))
}

void TdManager::sendRequest(const QVariantMap &request, std::function<void(const QVariantMap &)> callback)
{
    m_client->send(request, callback);
}

TdApi::AuthorizationState TdManager::getAuthorizationState() const noexcept
{
    return m_state;
}

StorageManager *TdManager::storageManager() const
{
    return m_storageManager;
}

void TdManager::setStorageManager(StorageManager *storageManager)
{
    m_storageManager = storageManager;
    m_client = m_storageManager->client();

    connect(m_client, SIGNAL(result(const QVariantMap &)), this, SLOT(handleResult(const QVariantMap &)));

    initialParameters();
    initialLanguagePack();
}

Locale *TdManager::locale() const
{
    return m_locale;
}

void TdManager::setLocale(Locale *locale)
{
    m_locale = locale;
}

bool TdManager::loading() const
{
    return m_loading;
}

void TdManager::addFileToDownloads(int fileId, qint64 chatId, qint64 messageId, int priority)
{
}

void TdManager::close() noexcept
{
    QVariantMap request;
    request.insert("@type", "close");

    sendRequest(request);

    // 1 sec delay
    QEventLoop loop;
    QTimer::singleShot(1000, &loop, SLOT(quit()));
    loop.exec();
}

void TdManager::downloadFile(qint32 fileId, qint32 priority, qint32 offset, qint32 limit, bool synchronous)
{
    QVariantMap request;
    request.insert("@type", "downloadFile");
    request.insert("file_id", fileId);
    request.insert("priority", priority);
    request.insert("offset", offset);
    request.insert("limit", limit);
    request.insert("synchronous", synchronous);

    sendRequest(request);
}

void TdManager::setLogVerbosityLevel(qint32 newVerbosityLevel)
{
    QVariantMap request;
    request.insert("@type", "setLogVerbosityLevel");
    request.insert("new_verbosity_level", newVerbosityLevel);

    sendRequest(request);
}

void TdManager::setOption(const QString &name, const QVariant &value)
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

    sendRequest(request);
}

void TdManager::setNetworkType(TdApi::NetworkType value)
{
    QVariantMap networkType;
    
    switch(value)
    {
        case TdApi::NetworkTypeMobile
            networkType.insert("@type", "networkTypeMobile");
        break;
    }

    QVariantMap request;
    request.insert("@type", "setNetworkType");
    request.insert("type", networkType);

    sendRequest(request);
}

void TdManager::initialLanguagePack()
{
    QSettings settings(QCoreApplication::organizationName(), AppName);

    setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    setOption("localization_target", "android");
    setOption("language_pack_id", settings.value("languagePackId", "en"));

    QVariantMap request;
    request.insert("@type", "getLanguagePackStrings");
    request.insert("language_pack_id", settings.value("languagePackId", "en"));

    m_client->send(request, [this](const auto &strings) {
        if (strings.value("@type").toByteArray() == "languagePackStrings")
            emit localizationStringChanged(strings);
    });
}

void TdManager::initialParameters()
{
    QVariantMap parameters;
    parameters.insert("database_directory", QString(QDir::homePath() % DatabaseDirectory));
    parameters.insert("use_file_database", true);
    parameters.insert("use_chat_info_database", true);
    parameters.insert("use_message_database", true);
    parameters.insert("use_secret_chats", true);
    parameters.insert("api_id", ApiId);
    parameters.insert("api_hash", ApiHash);
    parameters.insert("system_language_code", QLocale::system().name().left(2));
    parameters.insert("device_model", DeviceModel);
    parameters.insert("system_version", SystemVersion);
    parameters.insert("application_version", AppVersion);

    QVariantMap request;
    request.insert("@type", "setTdlibParameters");
    request.insert("parameters", parameters);

    sendRequest(request);
}

void TdManager::initialReady()
{
    QVariantMap request;
    request.insert("@type", "loadChats");
    request.insert("chat_list", {});
    request.insert("limit", ChatSliceLimit);

    sendRequest(request);
}

void TdManager::handleResult(const QVariantMap &object)
{
    switch (const auto objectType = object.value("@type").toByteArray(); fnv::hashRuntime(objectType.constData()))
    {
        case fnv::hash("updateAuthorizationState"): {
            handleAuthorizationState(object.value("authorization_state").toMap());
            break;
        }
    }
}

void TdManager::handleNetworkConfiguration(const QNetworkConfiguration &config)
{

}

void TdManager::handleAuthorizationState(const QVariantMap &authorizationState)
{
    const auto authorizationStateType = authorizationState.value("@type").toByteArray();

    switch (fnv::hashRuntime(authorizationStateType.constData()))
    {
        case fnv::hash("authorizationStateWaitEncryptionKey"): {
            m_state = TdApi::AuthorizationStateWaitEncryptionKey;

            QVariantMap request;
            request.insert("@type", "checkDatabaseEncryptionKey");
            request.insert("encryption_key", "");

            sendRequest(request);
            break;
        }
        case fnv::hash("authorizationStateWaitPhoneNumber"): {
            m_state = TdApi::AuthorizationStateWaitPhoneNumber;

            break;
        }
        case fnv::hash("authorizationStateWaitCode"): {
            m_state = TdApi::AuthorizationStateWaitCode;

            break;
        }
        case fnv::hash("authorizationStateWaitPassword"): {
            m_state = TdApi::AuthorizationStateWaitPassword;

            break;
        }
        case fnv::hash("authorizationStateWaitRegistration"): {
            m_state = TdApi::AuthorizationStateWaitRegistration;

            break;
        }
        case fnv::hash("authorizationStateReady"): {
            m_state = TdApi::AuthorizationStateReady;

            initialReady();

            break;
        }
        case fnv::hash("authorizationStateLoggingOut"): {
            m_state = TdApi::AuthorizationStateLoggingOut;

            break;
        }
        case fnv::hash("authorizationStateClosed"): {
            m_state = TdApi::AuthorizationStateClosed;

            break;
        }
    }

    emit authorizationStateChanged(m_state);
}
