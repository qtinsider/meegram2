#include "Application.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "Localization.hpp"
#include "Serialize.hpp"
#include "Settings.hpp"
#include "StorageManager.hpp"
#include "User.hpp"

#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QPainter>
#include <QScopedPointer>
#include <QSettings>
#include <QStringList>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

#include <algorithm>

namespace {
QString getUserFullName(qint64 userId, StorageManager *store, Locale *locale) noexcept
{
    const auto user = store->getUser(userId);
    const auto userType = user->type().value("@type").toByteArray();

    if (userType == "userTypeBot" || userType == "userTypeRegular")
    {
        return QString(user->firstName() + " " + user->lastName()).trimmed();
    }
    else if (userType == "userTypeDeleted" || userType == "userTypeUnknown")
    {
        return locale->getString("HiddenName");
    }

    return QString();
}

}  // namespace

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
}

bool Application::isAuthorized() const noexcept
{
    return m_isAuthorized;
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

const QString &Application::connectionStateString() const noexcept
{
    return m_connectionStateString;
}

void Application::initialize()
{
    initializeParameters();
    initializeLanguagePack();
    initializeCountries();
}

QString Application::getFormattedText(const QVariantMap &formattedText) const noexcept
{
    static const std::unordered_map<std::string, std::function<void(QTextCharFormat &, const QString &, const QVariantMap &)>> formatters =
        {{"textEntityTypeBold", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontWeight(QFont::Bold); }},
         {"textEntityTypeBotCommand",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("botCommand:" + entityText);
          }},
         {"textEntityTypeEmailAddress",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("mailto:" + entityText);
          }},
         {"textEntityTypeItalic", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontItalic(true); }},
         {"textEntityTypeMentionName",
          [&](QTextCharFormat &format, const QString &, const QVariantMap &type) {
              auto userId = type.value("user_id").toLongLong();
              auto title = getUserFullName(userId, m_storageManager, m_locale);
              format.setAnchor(true);
              format.setAnchorHref("userId:" + title);
          }},
         {"textEntityTypeMention",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("username:" + entityText);
          }},
         {"textEntityTypePhoneNumber",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("tel:" + entityText);
          }},
         {"textEntityTypeCode", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypePre", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypePreCode", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypeStrikethrough",
          [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontStrikeOut(true); }},
         {"textEntityTypeTextUrl",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &type) {
              QString url = type.value("url").toString();
              if (url.isEmpty())
              {
                  url = entityText;
              }
              format.setAnchor(true);
              format.setAnchorHref(url);
              format.setFontUnderline(true);
          }},
         {"textEntityTypeUrl",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref(entityText);
              format.setFontUnderline(true);
          }},
         {"textEntityTypeUnderline", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontUnderline(true); }}};

    const auto text = formattedText.value("text").toString();
    const auto entities = formattedText.value("entities").toList();

    QFont font;
    font.setPixelSize(23);
    font.setWeight(QFont::Light);

    QTextDocument *doc = new QTextDocument();

    doc->setDefaultFont(font);
    doc->setPlainText(text);

    QTextCursor cursor(doc);

    for (const auto &entity : entities)
    {
        const auto offset = entity.toMap().value("offset").toInt();
        const auto length = entity.toMap().value("length").toInt();
        const auto type = entity.toMap().value("type").toMap();
        const auto entityType = type.value("@type").toString().toStdString();
        const auto entityText = text.mid(offset, length);

        cursor.setPosition(offset);
        cursor.setPosition(offset + length, QTextCursor::KeepAnchor);

        QTextCharFormat format;

        if (auto it = formatters.find(entityType); it != formatters.end())
        {
            it->second(format, entityText, type);
            cursor.mergeCharFormat(format);
        }
    }

    QString result = doc->toHtml();
    delete doc;  // Clean up if you allocated doc dynamically
    return result;
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
        else
        {
            nlohmann::json json(value);
            qDebug() << QString::fromStdString(json.dump());
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

void Application::handleAuthorizationState(const QVariantMap &authorizationState)
{
    const auto authorizationStateType = authorizationState.value("@type").toString().toStdString();

    static const std::unordered_map<std::string, std::function<void()>> handlers = {
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
    const auto connectionStateType = connectionState.value("@type").toString().toStdString();

    static const std::unordered_map<std::string, std::function<void()>> handlers = {
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
