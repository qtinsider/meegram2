#include "Application.hpp"

#include "Common.hpp"
#include "Localization.hpp"
#include "Settings.hpp"
#include "StorageManager.hpp"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QLocale>
#include <QStringList>

#include <algorithm>

namespace {

constexpr auto createLocale(const QString &languageCode) -> std::optional<QLocale>
{
    // clang-format off
    static const std::unordered_map<QString, QLocale> localeMap = {
        {"ab", QLocale(QLocale::Abkhazian, QLocale::AnyCountry)},
        {"om", QLocale(QLocale::Afan, QLocale::AnyCountry)},
        {"aa", QLocale(QLocale::Afar, QLocale::AnyCountry)},
        {"af", QLocale(QLocale::Afrikaans, QLocale::AnyCountry)},
        {"sq", QLocale(QLocale::Albanian, QLocale::AnyCountry)},
        {"am", QLocale(QLocale::Amharic, QLocale::AnyCountry)},
        {"ar", QLocale(QLocale::Arabic, QLocale::AnyCountry)},
        {"hy", QLocale(QLocale::Armenian, QLocale::AnyCountry)},
        {"as", QLocale(QLocale::Assamese, QLocale::AnyCountry)},
        {"ay", QLocale(QLocale::Aymara, QLocale::AnyCountry)},
        {"az", QLocale(QLocale::Azerbaijani, QLocale::AnyCountry)},
        {"ba", QLocale(QLocale::Bashkir, QLocale::AnyCountry)},
        {"eu", QLocale(QLocale::Basque, QLocale::AnyCountry)},
        {"bn", QLocale(QLocale::Bengali, QLocale::AnyCountry)},
        {"dz", QLocale(QLocale::Bhutani, QLocale::AnyCountry)},
        {"bh", QLocale(QLocale::Bihari, QLocale::AnyCountry)},
        {"bi", QLocale(QLocale::Bislama, QLocale::AnyCountry)},
        {"br", QLocale(QLocale::Breton, QLocale::AnyCountry)},
        {"bg", QLocale(QLocale::Bulgarian, QLocale::AnyCountry)},
        {"my", QLocale(QLocale::Burmese, QLocale::AnyCountry)},
        {"be", QLocale(QLocale::Byelorussian, QLocale::AnyCountry)},
        {"km", QLocale(QLocale::Cambodian, QLocale::AnyCountry)},
        {"ca", QLocale(QLocale::Catalan, QLocale::AnyCountry)},
        {"zh", QLocale(QLocale::Chinese, QLocale::AnyCountry)},
        {"co", QLocale(QLocale::Corsican, QLocale::AnyCountry)},
        {"hr", QLocale(QLocale::Croatian, QLocale::AnyCountry)},
        {"cs", QLocale(QLocale::Czech, QLocale::AnyCountry)},
        {"da", QLocale(QLocale::Danish, QLocale::AnyCountry)},
        {"nl", QLocale(QLocale::Dutch, QLocale::AnyCountry)},
        {"en", QLocale(QLocale::English, QLocale::AnyCountry)},
        {"eo", QLocale(QLocale::Esperanto, QLocale::AnyCountry)},
        {"et", QLocale(QLocale::Estonian, QLocale::AnyCountry)},
        {"fo", QLocale(QLocale::Faroese, QLocale::AnyCountry)},
        {"fj", QLocale(QLocale::FijiLanguage, QLocale::AnyCountry)},
        {"fi", QLocale(QLocale::Finnish, QLocale::AnyCountry)},
        {"fr", QLocale(QLocale::French, QLocale::AnyCountry)},
        {"fy", QLocale(QLocale::Frisian, QLocale::AnyCountry)},
        {"gd", QLocale(QLocale::Gaelic, QLocale::AnyCountry)},
        {"gl", QLocale(QLocale::Galician, QLocale::AnyCountry)},
        {"ka", QLocale(QLocale::Georgian, QLocale::AnyCountry)},
        {"de", QLocale(QLocale::German, QLocale::AnyCountry)},
        {"el", QLocale(QLocale::Greek, QLocale::AnyCountry)},
        {"kl", QLocale(QLocale::Greenlandic, QLocale::AnyCountry)},
        {"gn", QLocale(QLocale::Guarani, QLocale::AnyCountry)},
        {"gu", QLocale(QLocale::Gujarati, QLocale::AnyCountry)},
        {"ha", QLocale(QLocale::Hausa, QLocale::AnyCountry)},
        {"he", QLocale(QLocale::Hebrew, QLocale::AnyCountry)},
        {"hi", QLocale(QLocale::Hindi, QLocale::AnyCountry)},
        {"hu", QLocale(QLocale::Hungarian, QLocale::AnyCountry)},
        {"is", QLocale(QLocale::Icelandic, QLocale::AnyCountry)},
        {"id", QLocale(QLocale::Indonesian, QLocale::AnyCountry)},
        {"ia", QLocale(QLocale::Interlingua, QLocale::AnyCountry)},
        {"ie", QLocale(QLocale::Interlingue, QLocale::AnyCountry)},
        {"iu", QLocale(QLocale::Inuktitut, QLocale::AnyCountry)},
        {"ik", QLocale(QLocale::Inupiak, QLocale::AnyCountry)},
        {"ga", QLocale(QLocale::Irish, QLocale::AnyCountry)},
        {"it", QLocale(QLocale::Italian, QLocale::AnyCountry)},
        {"ja", QLocale(QLocale::Japanese, QLocale::AnyCountry)},
        {"jv", QLocale(QLocale::Javanese, QLocale::AnyCountry)},
        {"kn", QLocale(QLocale::Kannada, QLocale::AnyCountry)},
        {"ks", QLocale(QLocale::Kashmiri, QLocale::AnyCountry)},
        {"kk", QLocale(QLocale::Kazakh, QLocale::AnyCountry)},
        {"rw", QLocale(QLocale::Kinyarwanda, QLocale::AnyCountry)},
        {"ky", QLocale(QLocale::Kirghiz, QLocale::AnyCountry)},
        {"ko", QLocale(QLocale::Korean, QLocale::AnyCountry)},
        {"ku", QLocale(QLocale::Kurdish, QLocale::AnyCountry)},
        {"rn", QLocale(QLocale::Kurundi, QLocale::AnyCountry)},
        {"lo", QLocale(QLocale::Laothian, QLocale::AnyCountry)},
        {"la", QLocale(QLocale::Latin, QLocale::AnyCountry)},
        {"lv", QLocale(QLocale::Latvian, QLocale::AnyCountry)},
        {"ln", QLocale(QLocale::Lingala, QLocale::AnyCountry)},
        {"lt", QLocale(QLocale::Lithuanian, QLocale::AnyCountry)},
        {"mk", QLocale(QLocale::Macedonian, QLocale::AnyCountry)},
        {"mg", QLocale(QLocale::Malagasy, QLocale::AnyCountry)},
        {"ms-my", QLocale(QLocale::Malay, QLocale::Malaysia)},
        {"ms-sg", QLocale(QLocale::Malay, QLocale::Singapore)},
        {"ml", QLocale(QLocale::Malayalam, QLocale::AnyCountry)},
        {"mt", QLocale(QLocale::Maltese, QLocale::AnyCountry)},
        {"mi", QLocale(QLocale::Maori, QLocale::AnyCountry)},
        {"mr", QLocale(QLocale::Marathi, QLocale::AnyCountry)},
        {"mo", QLocale(QLocale::Moldavian, QLocale::AnyCountry)},
        {"mn", QLocale(QLocale::Mongolian, QLocale::AnyCountry)},
        {"na", QLocale(QLocale::NauruLanguage, QLocale::AnyCountry)},
        {"ne", QLocale(QLocale::Nepali, QLocale::AnyCountry)},
        {"no", QLocale(QLocale::Norwegian, QLocale::AnyCountry)},
        {"oc", QLocale(QLocale::Occitan, QLocale::AnyCountry)},
        {"or", QLocale(QLocale::Oriya, QLocale::AnyCountry)},
        {"ps", QLocale(QLocale::Pashto, QLocale::AnyCountry)},
        {"fa", QLocale(QLocale::Persian, QLocale::AnyCountry)},
        {"pl", QLocale(QLocale::Polish, QLocale::AnyCountry)},
        {"pt-br", QLocale(QLocale::Portuguese, QLocale::Brazil)},    // Brazilian Portuguese
        {"pt-pt", QLocale(QLocale::Portuguese, QLocale::Portugal)},  // European Portuguese
        {"pa", QLocale(QLocale::Punjabi, QLocale::AnyCountry)},
        {"qu", QLocale(QLocale::Quechua, QLocale::AnyCountry)},
        {"rm", QLocale(QLocale::RhaetoRomance, QLocale::AnyCountry)},
        {"ro", QLocale(QLocale::Romanian, QLocale::AnyCountry)},
        {"ru", QLocale(QLocale::Russian, QLocale::AnyCountry)},
        {"sm", QLocale(QLocale::Samoan, QLocale::AnyCountry)},
        {"sg", QLocale(QLocale::Sangho, QLocale::AnyCountry)},
        {"sa", QLocale(QLocale::Sanskrit, QLocale::AnyCountry)},
        {"sr", QLocale(QLocale::Serbian, QLocale::AnyCountry)},
        {"sh", QLocale(QLocale::SerboCroatian, QLocale::AnyCountry)},
        {"st", QLocale(QLocale::Sesotho, QLocale::AnyCountry)},
        {"tn", QLocale(QLocale::Setswana, QLocale::AnyCountry)},
        {"sn", QLocale(QLocale::Shona, QLocale::AnyCountry)},
        {"sd", QLocale(QLocale::Sindhi, QLocale::AnyCountry)},
        {"si", QLocale(QLocale::Singhalese, QLocale::AnyCountry)},
        {"ss", QLocale(QLocale::Siswati, QLocale::AnyCountry)},
        {"sk", QLocale(QLocale::Slovak, QLocale::AnyCountry)},
        {"sl", QLocale(QLocale::Slovenian, QLocale::AnyCountry)},
        {"so", QLocale(QLocale::Somali, QLocale::AnyCountry)},
        {"es", QLocale(QLocale::Spanish, QLocale::AnyCountry)},
        {"su", QLocale(QLocale::Sundanese, QLocale::AnyCountry)},
        {"sw", QLocale(QLocale::Swahili, QLocale::AnyCountry)},
        {"sv", QLocale(QLocale::Swedish, QLocale::AnyCountry)},
        {"tl", QLocale(QLocale::Tagalog, QLocale::AnyCountry)},
        {"tg", QLocale(QLocale::Tajik, QLocale::AnyCountry)},
        {"ta", QLocale(QLocale::Tamil, QLocale::AnyCountry)},
        {"tt", QLocale(QLocale::Tatar, QLocale::AnyCountry)},
        {"te", QLocale(QLocale::Telugu, QLocale::AnyCountry)},
        {"th", QLocale(QLocale::Thai, QLocale::AnyCountry)},
        {"bo", QLocale(QLocale::Tibetan, QLocale::AnyCountry)},
        {"ti", QLocale(QLocale::Tigrinya, QLocale::AnyCountry)},
        {"to", QLocale(QLocale::TongaLanguage, QLocale::AnyCountry)},
        {"tn", QLocale(QLocale::Tsonga, QLocale::AnyCountry)},
        {"tr", QLocale(QLocale::Turkish, QLocale::AnyCountry)},
        {"tk", QLocale(QLocale::Turkmen, QLocale::AnyCountry)},
        {"tw", QLocale(QLocale::Twi, QLocale::AnyCountry)},
        {"ug", QLocale(QLocale::Uigur, QLocale::AnyCountry)},
        {"uk", QLocale(QLocale::Ukrainian, QLocale::AnyCountry)},
        {"ur", QLocale(QLocale::Urdu, QLocale::AnyCountry)},
        {"uz", QLocale(QLocale::Uzbek, QLocale::AnyCountry)},
        {"vi", QLocale(QLocale::Vietnamese, QLocale::AnyCountry)},
        {"vo", QLocale(QLocale::Volapuk, QLocale::AnyCountry)},
        {"cy", QLocale(QLocale::Welsh, QLocale::AnyCountry)},
        {"wo", QLocale(QLocale::Wolof, QLocale::AnyCountry)},
        {"xh", QLocale(QLocale::Xhosa, QLocale::AnyCountry)},
        {"yi", QLocale(QLocale::Yiddish, QLocale::AnyCountry)},
        {"yo", QLocale(QLocale::Yoruba, QLocale::AnyCountry)},
        {"za", QLocale(QLocale::Zhuang, QLocale::AnyCountry)},
        {"zu", QLocale(QLocale::Zulu, QLocale::AnyCountry)},
        {"nn", QLocale(QLocale::NorwegianBokmal, QLocale::AnyCountry)},
        {"nb", QLocale(QLocale::NorwegianBokmal, QLocale::AnyCountry)},
        {"nyn", QLocale(QLocale::NorwegianNynorsk, QLocale::AnyCountry)},  // ### obsolete
        {"bs", QLocale(QLocale::Bosnian, QLocale::AnyCountry)},
        {"dv", QLocale(QLocale::Divehi, QLocale::AnyCountry)},
        {"gv", QLocale(QLocale::Manx, QLocale::AnyCountry)},
        {"kw", QLocale(QLocale::Cornish, QLocale::AnyCountry)},
        {"ak", QLocale(QLocale::Akan, QLocale::AnyCountry)},
        {"kn", QLocale(QLocale::Konkani, QLocale::AnyCountry)},
        {"ga", QLocale(QLocale::Ga, QLocale::AnyCountry)},
        {"ig", QLocale(QLocale::Igbo, QLocale::AnyCountry)},
        {"kam", QLocale(QLocale::Kamba, QLocale::AnyCountry)},
        {"sy", QLocale(QLocale::Syriac, QLocale::AnyCountry)},
        {"ti", QLocale(QLocale::Tigrinya, QLocale::AnyCountry)},
        {"bl", QLocale(QLocale::Blin, QLocale::AnyCountry)},
        {"gez", QLocale(QLocale::Geez, QLocale::AnyCountry)},
        {"kri", QLocale(QLocale::Koro, QLocale::AnyCountry)},
        {"sid", QLocale(QLocale::Sidamo, QLocale::AnyCountry)},
        {"aa", QLocale(QLocale::Atsam, QLocale::AnyCountry)},
        {"ti", QLocale(QLocale::Tigre, QLocale::AnyCountry)},
        {"ju", QLocale(QLocale::Jju, QLocale::AnyCountry)},
        {"fur", QLocale(QLocale::Friulian, QLocale::AnyCountry)},
        {"ve", QLocale(QLocale::Venda, QLocale::AnyCountry)},
        {"ee", QLocale(QLocale::Ewe, QLocale::AnyCountry)},
        {"wl", QLocale(QLocale::Walamo, QLocale::AnyCountry)},
        {"haw", QLocale(QLocale::Hawaiian, QLocale::AnyCountry)},
        {"ty", QLocale(QLocale::Tyap, QLocale::AnyCountry)},
        {"che", QLocale(QLocale::Chewa, QLocale::AnyCountry)},
        {"fil", QLocale(QLocale::Filipino, QLocale::AnyCountry)},
        {"gme", QLocale(QLocale::SwissGerman, QLocale::AnyCountry)},
        {"ii", QLocale(QLocale::SichuanYi, QLocale::AnyCountry)},
        {"kpe", QLocale(QLocale::Kpelle, QLocale::AnyCountry)},
        {"nds", QLocale(QLocale::LowGerman, QLocale::AnyCountry)},
        {"nso", QLocale(QLocale::SouthNdebele, QLocale::AnyCountry)},
        {"nse", QLocale(QLocale::NorthernSotho, QLocale::AnyCountry)},
        {"se", QLocale(QLocale::NorthernSami, QLocale::AnyCountry)},
        {"dav", QLocale(QLocale::Taita, QLocale::AnyCountry)},
        {"cgg", QLocale(QLocale::Chiga, QLocale::AnyCountry)},
        {"tzm", QLocale(QLocale::CentralMoroccoTamazight, QLocale::AnyCountry)},
        {"mas", QLocale(QLocale::Masai, QLocale::AnyCountry)},
        {"seh", QLocale(QLocale::Sena, QLocale::AnyCountry)}
    };

    // clang-format on

    if (auto it = localeMap.find(languageCode); it != localeMap.end())
    {
        return it->second;
    }
    return std::nullopt;  // Return empty optional if not found
}

}  // namespace

Application::Application(QObject *parent)
    : QObject(parent)
    , m_client(StorageManager::instance().client())
    , m_locale(&Locale::instance())
    , m_settings(&Settings::instance())
    , m_storageManager(&StorageManager::instance())
{
    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(close()));

    connect(m_client, SIGNAL(result(td::td_api::Object *)), this, SLOT(handleResult(td::td_api::Object *)));

    connect(m_settings, SIGNAL(languagePackIdChanged()), this, SLOT(loadLanguagePack()));
}

bool Application::isAuthorized() const noexcept
{
    return m_isAuthorized;
}

const QString &Application::connectionStateString() const noexcept
{
    return m_connectionStateString;
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

    m_client->send(td::td_api::make_object<td::td_api::setOption>(name.toStdString(), std::move(optionValue)));
}

void Application::initialize() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::getOption>("version"), {});

    setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    setOption("localization_target", "android");
    setOption("language_pack_id", m_settings->languagePackId());

    setParameters();
    loadLanguagePack();
}

void Application::setParameters() noexcept
{
    auto request = td::td_api::make_object<td::td_api::setTdlibParameters>();

    request->database_directory_ = QString(QDir::homePath() + DatabaseDirectory).toStdString();
    request->use_file_database_ = true;
    request->use_chat_info_database_ = true;
    request->use_message_database_ = true;
    request->use_secret_chats_ = true;
    request->api_id_ = ApiId;
    request->api_hash_ = ApiHash;
    request->system_language_code_ = DefaultLanguageCode.toStdString();
    request->device_model_ = DeviceModel;
    request->system_version_ = SystemVersion;
    request->application_version_ = AppVersion;

    // request->use_test_dc_ = true;

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::ok::ID)
        {
            m_initializationStatus[0] = true;
            checkInitializationStatus();
        }
    });
}

void Application::loadLanguagePack() noexcept
{
    const auto &languageCode = m_settings->languagePackId();

    qDebug() << "Requesting language pack with ID:" << languageCode;

    auto request = td::td_api::make_object<td::td_api::getLanguagePackStrings>();
    request->language_pack_id_ = languageCode.toStdString();

    m_client->send(std::move(request), [this, languageCode](auto &&response) noexcept {
        if (response->get_id() == td::td_api::languagePackStrings::ID)
        {
            const auto &languagePlural = m_settings->languagePluralId();

            qDebug() << "Setting language plural for code:" << languagePlural;

            m_locale->setLanguagePlural(languagePlural);
            m_locale->setLanguagePackStrings(td::move_tl_object_as<td::td_api::languagePackStrings>(response));

            if (auto locale = createLocale(languageCode); locale)
            {
                QLocale::setDefault(*locale);
                qDebug() << "Default Locale:" << QLocale::languageToString(locale->language()) << QLocale::countryToString(locale->country());
            }
            else
            {
                qDebug() << "Locale not found for language code:" << languageCode;
            }

            if (!m_initializationStatus[1])
            {
                m_initializationStatus[1] = true;
                checkInitializationStatus();
            }
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

void Application::handleAuthorizationState(const td::td_api::AuthorizationState &authorizationState)
{
    if (authorizationState.get_id() == td::td_api::authorizationStateReady::ID)
    {
        m_client->send(td::td_api::make_object<td::td_api::loadChats>(), {});

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
