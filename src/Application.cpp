#include "Application.hpp"
#include "ChatModel.hpp"
#include "Client.hpp"
#include "Common.hpp"
#include "ImageProviders.hpp"
#include "Localization.hpp"
#include "SelectionModel.hpp"
#include "Serialize.hpp"
#include "Settings.hpp"
#include "StorageManager.hpp"

#include <QApplication>
#include <QDebug>
#include <QDeclarativeContext>
#include <QDeclarativeEngine>
#include <QDir>
#include <QLocale>
#include <QStringList>

#include <algorithm>
#include <print>

Application::Application(QDeclarativeView *view, QObject *parent)
    : QObject(parent)
    , declarativeView(view)
    , m_client(new Client(this))
    , m_locale(new Locale())
    , m_settings(new Settings(this))
    , m_storageManager(new StorageManager(m_client))
    , chatModel(new ChatModel(m_locale, m_storageManager))
    , countryModel(new CountryModel(this))
    , chatFolderModel(new ChatFolderModel(this))
    , languagePackInfoModel(new LanguagePackInfoModel(this))
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));

    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    connect(m_settings, SIGNAL(languagePackIdChanged()), this, SIGNAL(languageChanged()));
    connect(m_settings, SIGNAL(languagePackIdChanged()), this, SLOT(initializeLanguagePack()));

    declarativeView->rootContext()->setContextProperty("app", this);
    declarativeView->rootContext()->setContextProperty("settings", m_settings);
    declarativeView->rootContext()->setContextProperty("chatModel", chatModel);
    declarativeView->rootContext()->setContextProperty("countryModel", countryModel);
    declarativeView->rootContext()->setContextProperty("chatFolderModel", chatFolderModel);
    declarativeView->rootContext()->setContextProperty("languagePackModel", languagePackInfoModel);

    declarativeView->rootContext()->setContextProperty("AppVersion", AppVersion);
    declarativeView->engine()->addImageProvider("chatPhoto", new ChatPhotoProvider);

    QObject::connect(declarativeView->engine(), SIGNAL(quit()), declarativeView, SLOT(close()));

    declarativeView->setResizeMode(QDeclarativeView::SizeRootObjectToView);
    declarativeView->setSource(QUrl("qrc:/qml/main.qml"));
    declarativeView->showFullScreen();
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

    m_client->send(std::move(request), [this](auto &&value) {
        if (value->get_id() == td::td_api::ok::ID)
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

    m_client->send(std::move(request), [this](auto &&value) {
        if (value->get_id() == td::td_api::languagePackStrings::ID)
        {
            m_locale->setLanguagePlural(m_settings->languagePluralId());
            m_locale->setLanguagePackStrings(td::move_tl_object_as<td::td_api::languagePackStrings>(value));

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
    m_client->send(td::td_api::make_object<td::td_api::getCountries>(), [this](auto &&value) {
        if (value->get_id() == td::td_api::countries::ID)
        {
            countryModel->setCountries(td::move_tl_object_as<td::td_api::countries>(value));

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

    if (object->get_id() == td::td_api::updateChatFolders::ID)
    {
        chatFolderModel->setLocale(m_locale);
        chatFolderModel->setChatFolders(std::move(static_cast<td::td_api::updateChatFolders *>(object)->chat_folders_));
    }
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
    static const std::unordered_map<int, std::function<void()>> handlers = {
        {td::td_api::connectionStateReady::ID, [this]() { m_connectionStateString = "ready"; }},
        {td::td_api::connectionStateUpdating::ID, [this]() { m_connectionStateString = "updating"; }},
        {td::td_api::connectionStateConnecting::ID, [this]() { m_connectionStateString = "connecting"; }},
        {td::td_api::connectionStateWaitingForNetwork::ID, [this]() { m_connectionStateString = "waiting_for_network"; }},
        {td::td_api::connectionStateConnectingToProxy::ID, [this]() { m_connectionStateString = "connecting_to_proxy"; }},
    };

    if (const auto it = handlers.find(connectionState.get_id()); it != handlers.end())
    {
        it->second();
        emit connectionStateChanged();
    }
}

void Application::initializeLanguagePackInfo()
{
    auto request = td::td_api::make_object<td::td_api::getLocalizationTargetInfo>();
    request->only_local_ = true;

    m_client->send(std::move(request), [this](auto &&value) {
        if (value->get_id() == td::td_api::localizationTargetInfo::ID)
        {
            languagePackInfoModel->setLanguagePackInfo(td::move_tl_object_as<td::td_api::localizationTargetInfo>(value));

            m_initializationStatus[3] = true;
            checkInitializationStatus();
        }
    });
}
