#include "Localization.hpp"

#include <QDebug>
#include <QStringList>

#include <algorithm>
#include <cmath>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>

class PluralRules_Zero : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        return (count == 0 || count == 1) ? QuantityOne : QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Zero>(*this);
    }
};

class PluralRules_Welsh : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        switch (count)
        {
            case 0:
                return QuantityZero;
            case 1:
                return QuantityOne;
            case 2:
                return QuantityTwo;
            case 3:
                return QuantityFew;
            case 6:
                return QuantityMany;
            default:
                return QuantityOther;
        }
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Welsh>(*this);
    }
};

class PluralRules_Two : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        if (count == 1)
            return QuantityOne;
        if (count == 2)
            return QuantityTwo;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Two>(*this);
    }
};

class PluralRules_Tachelhit : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        if (count >= 0 && count <= 1)
            return QuantityOne;
        if (count >= 2 && count <= 10)
            return QuantityFew;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Tachelhit>(*this);
    }
};

class PluralRules_Slovenian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        if (rem100 == 1)
            return QuantityOne;
        if (rem100 == 2)
            return QuantityTwo;
        if (rem100 >= 3 && rem100 <= 4)
            return QuantityFew;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Slovenian>(*this);
    }
};

class PluralRules_Romanian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        if (count == 1)
            return QuantityOne;
        if (count == 0 || (rem100 >= 1 && rem100 <= 19))
            return QuantityFew;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Romanian>(*this);
    }
};

class PluralRules_Polish : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (count == 1)
            return QuantityOne;
        if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
            return QuantityFew;
        if ((rem10 >= 0 && rem10 <= 1) || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 12 && rem100 <= 14))
            return QuantityMany;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Polish>(*this);
    }
};

class PluralRules_One : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        return count == 1 ? QuantityOne : QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_One>(*this);
    }
};

class PluralRules_None : public PluralRules
{
public:
    Quantity quantityForNumber(int /*count*/) const override
    {
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_None>(*this);
    }
};

class PluralRules_Maltese : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        if (count == 1)
            return QuantityOne;
        if (count == 0 || (rem100 >= 2 && rem100 <= 10))
            return QuantityFew;
        if (rem100 >= 11 && rem100 <= 19)
            return QuantityMany;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Maltese>(*this);
    }
};

class PluralRules_Macedonian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        if (count % 10 == 1 && count != 11)
            return QuantityOne;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Macedonian>(*this);
    }
};

class PluralRules_Lithuanian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && !(rem100 >= 11 && rem100 <= 19))
            return QuantityOne;
        if (rem10 >= 2 && rem10 <= 9 && !(rem100 >= 11 && rem100 <= 19))
            return QuantityFew;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Lithuanian>(*this);
    }
};

class PluralRules_Latvian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        if (count == 0)
            return QuantityZero;
        if (count % 10 == 1 && count % 100 != 11)
            return QuantityOne;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Latvian>(*this);
    }
};

class PluralRules_Langi : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        if (count == 0)
            return QuantityZero;
        if (count == 1)
            return QuantityOne;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Langi>(*this);
    }
};

class PluralRules_French : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        return (count >= 0 && count < 2) ? QuantityOne : QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_French>(*this);
    }
};

class PluralRules_Czech : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        if (count == 1)
            return QuantityOne;
        if (count >= 2 && count <= 4)
            return QuantityFew;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Czech>(*this);
    }
};

class PluralRules_Breton : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        switch (count)
        {
            case 0:
                return QuantityZero;
            case 1:
                return QuantityOne;
            case 2:
                return QuantityTwo;
            case 3:
                return QuantityFew;
            case 6:
                return QuantityMany;
            default:
                return QuantityOther;
        }
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Breton>(*this);
    }
};

class PluralRules_Balkan : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && rem100 != 11)
            return QuantityOne;
        if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
            return QuantityFew;
        if ((rem10 == 0 || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 11 && rem100 <= 14)))
            return QuantityMany;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Balkan>(*this);
    }
};

class PluralRules_Serbian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && rem100 != 11)
            return QuantityOne;
        if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
            return QuantityFew;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Serbian>(*this);
    }
};

class PluralRules_Arabic : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override
    {
        const auto rem100 = count % 100;
        if (count == 0)
            return QuantityZero;
        if (count == 1)
            return QuantityOne;
        if (count == 2)
            return QuantityTwo;
        if (rem100 >= 3 && rem100 <= 10)
            return QuantityFew;
        if (rem100 >= 11 && rem100 <= 99)
            return QuantityMany;
        return QuantityOther;
    }

    std::unique_ptr<PluralRules> clone() const override
    {
        return std::make_unique<PluralRules_Arabic>(*this);
    }
};

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

QString Locale::getString(const QString &key) const
{
    if (!m_languagePack.contains(key))
    {
        qDebug() << "LOC_ERR:" << key;
        return key;
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
    result = std::regex_replace(result, boldRegex, "<b>$1</b>");

    static std::regex italicRegex(R"(\*(.*?)\*)");
    result = std::regex_replace(result, italicRegex, "<i>$1</i>");

    return QString::fromStdString(result);
}

QString Locale::formatPluralString(const QString &key, int plural) const
{
    const auto pluralKey = key + stringForQuantity(m_currentPluralRules->quantityForNumber(plural));
    return getString(pluralKey).arg(plural);
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

QString Locale::formatTtl(int ttl) const
{
    if (ttl < 60)
    {
        return formatPluralString("TTLStringSeconds", std::floor(ttl));
    }
    if (ttl < 60 * 60)
    {
        return formatPluralString("TTLStringMinutes", std::floor(ttl / 60));
    }
    if (ttl < 24 * 60 * 60)
    {
        return formatPluralString("TTLStringHours", std::floor(ttl / 60 / 60));
    }

    const auto days = ttl / 60 / 60 / 24;
    if (ttl % 7 == 0)
    {
        return formatPluralString("TTLStringWeeks", std::floor(days / 7));
    }
    return formatPluralString("TTLStringWeeks", std::floor(days / 7)) + formatPluralString("TTLStringDays", std::floor(days % 7));
}

void Locale::setLanguagePlural(const QString &value)
{
    if (m_languagePlural != value)
    {
        m_languagePlural = value;
    }
}

void Locale::setLanguagePackStrings(td::td_api::object_ptr<td::td_api::languagePackStrings> value)
{
    for ( auto &&string : value->strings_)
    {
        if (string->value_->get_id() == td::td_api::languagePackStringValueOrdinary::ID)
        {
            auto ordinaryValue = td::move_tl_object_as<td::td_api::languagePackStringValueOrdinary>(string->value_);
            m_languagePack.emplace(QString::fromStdString(string->key_), QString::fromStdString(ordinaryValue->value_));
        }
        else if (string->value_->get_id() == td::td_api::languagePackStringValuePluralized::ID)
        {
            auto pluralizedValue = td::move_tl_object_as<td::td_api::languagePackStringValuePluralized>(string->value_);
            const auto keyBase = QString::fromStdString(string->key_);
            if (!pluralizedValue->zero_value_.empty())
            {
                m_languagePack.emplace(keyBase + "_zero_value", QString::fromStdString(pluralizedValue->zero_value_));
            }
            if (!pluralizedValue->one_value_.empty())
            {
                m_languagePack.emplace(keyBase + "_one_value", QString::fromStdString(pluralizedValue->one_value_));
            }
            if (!pluralizedValue->two_value_.empty())
            {
                m_languagePack.emplace(keyBase + "_two_value", QString::fromStdString(pluralizedValue->two_value_));
            }
            if (!pluralizedValue->few_value_.empty())
            {
                m_languagePack.emplace(keyBase + "_few_value", QString::fromStdString(pluralizedValue->few_value_));
            }
            if (!pluralizedValue->many_value_.empty())
            {
                m_languagePack.emplace(keyBase + "_many_value", QString::fromStdString(pluralizedValue->many_value_));
            }
            if (!pluralizedValue->other_value_.empty())
            {
                m_languagePack.emplace(keyBase + "_other_value", QString::fromStdString(pluralizedValue->other_value_));
            }
        }
        else if (string->value_->get_id() == td::td_api::languagePackStringValueDeleted::ID)
        {
            // No action needed for deleted strings
        }

    }

    updatePluralRules();
}

QString Locale::stringForQuantity(PluralRules::Quantity quantity) const
{
    switch (quantity)
    {
        case PluralRules::QuantityZero:
            return "_zero_value";
        case PluralRules::QuantityOne:
            return "_one_value";
        case PluralRules::QuantityTwo:
            return "_two_value";
        case PluralRules::QuantityFew:
            return "_few_value";
        case PluralRules::QuantityMany:
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
