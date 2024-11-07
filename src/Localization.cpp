#include "Localization.hpp"

#include <QDebug>
#include <QStringList>

#include <algorithm>
#include <cmath>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>

Locale::Locale()
{
    // clang-format off
    addRules(QStringList() << "bem" << "brx" << "da" << "de" << "el" << "en" << "eo" << "es" << "et" << "fi" << "fo" << "gl" << "he" << "iw" << "it" << "nb" <<
                 "nl" << "nn" << "no" << "sv" << "af" << "bg" << "bn" << "ca" << "eu" << "fur" << "fy" << "gu" << "ha" << "is" << "ku" <<
                 "lb" << "ml" << "mr" << "nah" << "ne" << "om" << "or" << "pa" << "pap" << "ps" << "so" << "sq" << "sw" << "ta" << "te" <<
                 "tk" << "ur" << "zu" << "mn" << "gsw" << "chr" << "rm" << "pt" << "an" << "ast", std::make_unique<PluralRules_One>());
    addRules(QStringList() << "cs" << "sk", std::make_unique<PluralRules_Czech>());
    addRules(QStringList() << "ff" << "fr" << "kab", std::make_unique<PluralRules_French>());
    addRules(QStringList() << "ru" << "uk" << "be" << "sh", std::make_unique<PluralRules_Balkan>());
    addRules(QStringList() << "sr" << "hr" << "bs", std::make_unique<PluralRules_Serbian>());
    addRules(QStringList() << "lv", std::make_unique<PluralRules_Latvian>());
    addRules(QStringList() << "lt", std::make_unique<PluralRules_Lithuanian>());
    addRules(QStringList() << "pl", std::make_unique<PluralRules_Polish>());
    addRules(QStringList() << "ro" << "mo", std::make_unique<PluralRules_Romanian>());
    addRules(QStringList() << "sl", std::make_unique<PluralRules_Slovenian>());
    addRules(QStringList() << "ar", std::make_unique<PluralRules_Arabic>());
    addRules(QStringList() << "mk", std::make_unique<PluralRules_Macedonian>());
    addRules(QStringList() << "cy", std::make_unique<PluralRules_Welsh>());
    addRules(QStringList() << "br", std::make_unique<PluralRules_Breton>());
    addRules(QStringList() << "lag", std::make_unique<PluralRules_Langi>());
    addRules(QStringList() << "shi", std::make_unique<PluralRules_Tachelhit>());
    addRules(QStringList() << "mt", std::make_unique<PluralRules_Maltese>());
    addRules(QStringList() << "ga" << "se" << "sma" << "smi" << "smj" << "smn" << "sms", std::make_unique<PluralRules_Two>());
    addRules(QStringList() << "ak" << "am" << "bh" << "fil" << "tl" << "guw" << "hi" << "ln" << "mg" << "nso" << "ti" << "wa", std::make_unique<PluralRules_Zero>());
    addRules(QStringList() << "az" << "bm" << "fa" << "ig" << "hu" << "ja" << "kde" << "kea" << "ko" << "my" << "ses" << "sg" << "to" <<
                 "tr" << "vi" << "wo" << "yo" << "zh" << "bo" << "dz" << "id" << "jv" << "jw" << "ka" << "km" << "kn" << "ms" << "th" << "in", std::make_unique<PluralRules_None>());
    // clang-format on
}

Locale &Locale::instance()
{
    static Locale staticObject;
    return staticObject;
}

QString Locale::translate(const char *key, int plural) const
{
    if (plural >= 0)
    {
        return formatPluralString(key, plural);
    }

    return getString(key);
}

QString Locale::getString(const char *key) const
{
    if (!m_languagePack.contains(QString(key)))
    {
        qDebug() << "LOC_ERR:" << key;
        return QString::fromUtf8(key);
    }

    auto it = m_languagePack.find(key);
    const auto &original = (it != m_languagePack.end()) ? it->second.toStdString() : std::string();

    std::string result;
    result.reserve(original.size());  // Reserve space for potential expansion

    auto view = std::string_view(original);
    size_t pos = 0, last_pos = 0;

    // Remove $d placeholders
    while ((pos = view.find("$d", last_pos)) != std::string_view::npos)
    {
        result.append(view.substr(last_pos, pos - last_pos));
        last_pos = pos + 2;
    }
    result.append(view.substr(last_pos));

    last_pos = 0;
    while ((pos = result.find("$s", last_pos)) != std::string::npos)
    {
        result.erase(pos, 2);
        last_pos = pos;
    }

    static const std::unordered_map<std::string_view, std::string_view> replacements = {{"%%", "%"}, {"%s", "%1"}, {"EEEE", "dddd"}, {"EEE", "ddd"}};

    for (const auto &[from, to] : replacements)
    {
        std::string::size_type start_pos = 0;
        while ((start_pos = result.find(from, start_pos)) != std::string::npos)
        {
            result.replace(start_pos, from.length(), to);
            start_pos += to.length();  // Move past the replacement
        }
    }

    static std::regex boldRegex(R"(\*\*(.*?)\*\*)");
    static std::regex italicRegex(R"(\*(.*?)\*)");

    result = std::regex_replace(result, boldRegex, "<b>$1</b>");
    result = std::regex_replace(result, italicRegex, "<i>$1</i>");

    return QString::fromStdString(result);
}

QString Locale::formatPluralString(const char *key, int plural) const
{
    const auto pluralKey = QString(key) + stringForQuantity(m_currentPluralRules->quantityForNumber(plural));
    return getString(pluralKey.toUtf8().constData()).arg(plural);
}

QString Locale::formatCallDuration(int duration) const
{
    if (duration > 3600)
    {
        auto result = formatPluralString("CallDurationHours", std::floor(duration / 3600));
        const auto minutes = std::floor(duration % 3600 / 60);
        if (minutes > 0)
        {
            result.append(", ").append(formatPluralString("CallDurationMinutes", minutes));
        }

        return result;
    }
    if (duration > 60)
    {
        return formatPluralString("CallDurationMinutes", std::floor(duration / 60));
    }

    return formatPluralString("CallDurationSeconds", std::floor(duration));
}

QString Locale::formatTtl(int ttl, bool shorter) const
{
    if (shorter)
    {
        return (ttl < 60) ? QString("%1s").arg(ttl) : QString("%1m").arg(ttl / 60);
    }

    constexpr int minute = 60;
    constexpr int hour = 60 * minute;
    constexpr int day = 24 * hour;
    constexpr int week = 7 * day;
    constexpr int month = 31 * day;
    constexpr int year = 365 * day;

    if (ttl < minute)
    {
        return formatPluralString("Seconds", ttl);
    }
    if (ttl < hour)
    {
        return formatPluralString("Minutes", ttl / minute);
    }
    if (ttl < day)
    {
        return formatPluralString("Hours", ttl / hour);
    }
    if (ttl < week)
    {
        return formatPluralString("Days", ttl / day);
    }
    if (ttl < month)
    {
        int days = ttl / day;
        int weeks = days / 7;
        int remainingDays = days % 7;

        return remainingDays == 0 ? formatPluralString("Weeks", weeks)
                                  : QString("%1 %2").arg(formatPluralString("Weeks", weeks)).arg(formatPluralString("Days", remainingDays));
    }
    if (ttl < year)
    {
        return formatPluralString("Months", ttl / month);
    }

    return formatPluralString("Years", ttl / year);
}

void Locale::setLanguagePlural(const QString &value)
{
    if (m_languagePlural != value)
    {
        m_languagePlural = value;
    }
}

void Locale::setLanguagePackStrings(td::td_api::object_ptr<td::td_api::languagePackStrings> languagePackData)
{
    m_languagePack.reserve(languagePackData->strings_.size());  // Reserve to avoid reallocations

    for (auto &languageString : languagePackData->strings_)
    {
        const auto stringKey = QString::fromStdString(languageString->key_);

        switch (languageString->value_->get_id())
        {
            case td::td_api::languagePackStringValueOrdinary::ID: {
                if (auto ordinaryValue = td::td_api::move_object_as<td::td_api::languagePackStringValueOrdinary>(languageString->value_))
                    m_languagePack.emplace(stringKey, QString::fromStdString(ordinaryValue->value_));
                break;
            }

            case td::td_api::languagePackStringValuePluralized::ID: {
                if (auto pluralizedValue = td::td_api::move_object_as<td::td_api::languagePackStringValuePluralized>(languageString->value_))
                {
                    const std::array<std::pair<QString, std::string_view>, 6> pluralForms = {{{"_zero_value", pluralizedValue->zero_value_},
                                                                                              {"_one_value", pluralizedValue->one_value_},
                                                                                              {"_two_value", pluralizedValue->two_value_},
                                                                                              {"_few_value", pluralizedValue->few_value_},
                                                                                              {"_many_value", pluralizedValue->many_value_},
                                                                                              {"_other_value", pluralizedValue->other_value_}}};

                    std::ranges::for_each(pluralForms | std::views::filter([](const auto &form) { return !form.second.empty(); }) |
                                              std::views::transform([&stringKey](const auto &form) {
                                                  return std::pair{stringKey + form.first, QString::fromStdString(std::string (form.second))};
                                              }),
                                          [this](auto &&entry) { m_languagePack.emplace(std::move(entry)); });
                }
                break;
            }
            case td::td_api::languagePackStringValueDeleted::ID:
                // No action needed for deleted strings
                break;
            default:
                break;
        }
    }

    updatePluralRules();
}

QString Locale::stringForQuantity(PluralRules::Quantity quantity) const
{
    switch (quantity)
    {
        case PluralRules::Quantity::QuantityZero:
            return "_zero_value";
        case PluralRules::Quantity::QuantityOne:
            return "_one_value";
        case PluralRules::Quantity::QuantityTwo:
            return "_two_value";
        case PluralRules::Quantity::QuantityFew:
            return "_few_value";
        case PluralRules::Quantity::QuantityMany:
            return "_many_value";
        default:
            return "_other_value";
    }
}

void Locale::addRules(const QStringList &languages, std::unique_ptr<PluralRules> rules)
{
    std::ranges::for_each(languages, [&](const auto &x) { m_allRules.emplace(x, rules->clone()); });
}

void Locale::updatePluralRules()
{
    if (auto it = m_allRules.find(m_languagePlural); it != m_allRules.end())
    {
        m_currentPluralRules = it->second->clone();
    }
    else
    {
        m_currentPluralRules = m_allRules.at("en")->clone();  // Default to English rules if not found
    }
}

Translator::Translator(QObject *parent)
    : QTranslator(parent)
{
}

QString Translator::translate(const char *context, const char *sourceText, const char *disambiguation) const
{
    Q_UNUSED(context);
    Q_UNUSED(disambiguation);

    // if (n >= 0)
    // {
    //     return formatPluralString(sourceText, n);
    // }
    // Plural logic that never saw the light of day... 😢

    return Locale::instance().getString(sourceText);
}
