#include "TdApi.hpp"

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
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));
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

void TdManager::initialLanguagePack()
{
    QSettings settings(QCoreApplication::organizationName(), AppName);

    setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    setOption("localization_target", "android");
    setOption("language_pack_id", settings.value("languagePackId", "en"));

    QVariantMap request;
    request.insert("@type", "getLanguagePackStrings");
    request.insert("language_pack_id", settings.value("languagePackId", "en"));

    sendRequest(request, [this](const auto &r) {
        if (r.value("@type").toByteArray() == "languagePackStrings")
            m_locale->processStrings(r);
    });
}

void TdManager::initialParameters()
{
    QVariantMap request;
    request.insert("@type", "setTdlibParameters");
    request.insert("database_directory", QString(QDir::homePath() % DatabaseDirectory));
    request.insert("use_file_database", true);
    request.insert("use_chat_info_database", true);
    request.insert("use_message_database", true);
    request.insert("use_secret_chats", true);
    request.insert("api_id", ApiId);
    request.insert("api_hash", ApiHash);
    request.insert("system_language_code", QLocale::system().name().left(2));
    request.insert("device_model", DeviceModel);
    request.insert("system_version", SystemVersion);
    request.insert("application_version", AppVersion);

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
    const QString type = object.value(QLatin1String("@type")).toString();

    if (type == QLatin1String("updateAuthorizationState"))
    {
        handleAuthorizationState(object.value("authorization_state").toMap());
    }
}

void TdManager::handleAuthorizationState(const QVariantMap &authorizationState)
{
    static const std::unordered_map<std::string, std::function<void()>> handlers = {
        {"authorizationStateWaitEncryptionKey",
         [this]() {
             m_state = TdApi::AuthorizationStateWaitEncryptionKey;
             QVariantMap request;
             request.insert("@type", "checkDatabaseEncryptionKey");
             request.insert("encryption_key", "");
             sendRequest(request);
         }},
        {"authorizationStateWaitPhoneNumber", [this]() { m_state = TdApi::AuthorizationStateWaitPhoneNumber; }},
        {"authorizationStateWaitCode", [this]() { m_state = TdApi::AuthorizationStateWaitCode; }},
        {"authorizationStateWaitPassword", [this]() { m_state = TdApi::AuthorizationStateWaitPassword; }},
        {"authorizationStateWaitRegistration", [this]() { m_state = TdApi::AuthorizationStateWaitRegistration; }},
        {"authorizationStateReady",
         [this]() {
             m_state = TdApi::AuthorizationStateReady;
             initialReady();
         }},
        {"authorizationStateLoggingOut", [this]() { m_state = TdApi::AuthorizationStateLoggingOut; }},
        {"authorizationStateClosed", [this]() { m_state = TdApi::AuthorizationStateClosed; }},
    };

    const auto authorizationStateType = authorizationState.value("@type").toString().toStdString();

    if (const auto it = handlers.find(authorizationStateType); it != handlers.end())
    {
        it->second();
    }

    emit authorizationStateChanged(m_state);
}
