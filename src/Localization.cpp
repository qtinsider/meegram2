#include "Localization.hpp"

#include "Common.hpp"
#include "Serialize.hpp"

#include <QDebug>
#include <QStringList>

#include <algorithm>
#include <cmath>
#include <ranges>
#include <regex>
#include <string>
#include <string_view>
#include <unordered_map>

class PluralRules
{
public:
    virtual Locale::Quantity quantityForNumber(int count) = 0;
};

class PluralRules_Zero : public PluralRules
{
    Locale::Quantity quantityForNumber(int count)
    {
        if (count == 0 || count == 1)
        {
            return Locale::QuantityOne;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Welsh : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Locale::QuantityZero;
        }
        else if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if (count == 2)
        {
            return Locale::QuantityTwo;
        }
        else if (count == 3)
        {
            return Locale::QuantityFew;
        }
        else if (count == 6)
        {
            return Locale::QuantityMany;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Two : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if (count == 2)
        {
            return Locale::QuantityTwo;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Tachelhit : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count >= 0 && count <= 1)
        {
            return Locale::QuantityOne;
        }
        else if (count >= 2 && count <= 10)
        {
            return Locale::QuantityFew;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Slovenian : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (rem100 == 1)
        {
            return Locale::QuantityOne;
        }
        else if (rem100 == 2)
        {
            return Locale::QuantityTwo;
        }
        else if (rem100 >= 3 && rem100 <= 4)
        {
            return Locale::QuantityFew;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Romanian : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if ((count == 0 || (rem100 >= 1 && rem100 <= 19)))
        {
            return Locale::QuantityFew;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Polish : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        {
            return Locale::QuantityFew;
        }
        else if ((rem10 >= 0 && rem10 <= 1) || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 12 && rem100 <= 14))
        {
            return Locale::QuantityMany;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_One : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        return count == 1 ? Locale::QuantityOne : Locale::QuantityOther;
    }
};

class PluralRules_None : public PluralRules
{
    Locale::Quantity quantityForNumber(int /*count*/) override
    {
        return Locale::QuantityOther;
    }
};

class PluralRules_Maltese : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if (count == 0 || (rem100 >= 2 && rem100 <= 10))
        {
            return Locale::QuantityFew;
        }
        else if (rem100 >= 11 && rem100 <= 19)
        {
            return Locale::QuantityMany;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Macedonian : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count % 10 == 1 && count != 11)
        {
            return Locale::QuantityOne;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Lithuanian : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && !(rem100 >= 11 && rem100 <= 19))
        {
            return Locale::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 9 && !(rem100 >= 11 && rem100 <= 19))
        {
            return Locale::QuantityFew;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Latvian : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Locale::QuantityZero;
        }
        else if (count % 10 == 1 && count % 100 != 11)
        {
            return Locale::QuantityOne;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Langi : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Locale::QuantityZero;
        }
        else if (count == 1)
        {
            return Locale::QuantityOne;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_French : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count >= 0 && count < 2)
        {
            return Locale::QuantityOne;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Czech : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if (count >= 2 && count <= 4)
        {
            return Locale::QuantityFew;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Breton : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Locale::QuantityZero;
        }
        else if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if (count == 2)
        {
            return Locale::QuantityTwo;
        }
        else if (count == 3)
        {
            return Locale::QuantityFew;
        }
        else if (count == 6)
        {
            return Locale::QuantityMany;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Balkan : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && rem100 != 11)
        {
            return Locale::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        {
            return Locale::QuantityFew;
        }
        else if ((rem10 == 0 || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 11 && rem100 <= 14)))
        {
            return Locale::QuantityMany;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Serbian : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && rem100 != 11)
        {
            return Locale::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        {
            return Locale::QuantityFew;
        }

        return Locale::QuantityOther;
    }
};

class PluralRules_Arabic : public PluralRules
{
    Locale::Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (count == 0)
        {
            return Locale::QuantityZero;
        }
        else if (count == 1)
        {
            return Locale::QuantityOne;
        }
        else if (count == 2)
        {
            return Locale::QuantityTwo;
        }
        else if (rem100 >= 3 && rem100 <= 10)
        {
            return Locale::QuantityFew;
        }
        else if (rem100 >= 11 && rem100 <= 99)
        {
            return Locale::QuantityMany;
        }

        return Locale::QuantityOther;
    }
};

namespace {
void replaceAll(std::string &str, const std::string &from, const std::string &to)
{
    if (from.empty())
        return;
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos)
    {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();  // Handles case where 'to' is a substring of 'from'
    }
}
}  // namespace

Locale::Locale(QObject *parent)
    : QObject(parent)
{
    // clang-format off
    addRules(QStringList() <<"bem" << "brx" << "da" << "de" << "el" << "en" << "eo" << "es" << "et" << "fi" << "fo" << "gl" << "he" << "iw" << "it" << "nb" <<
                 "nl" << "nn" << "no" << "sv" << "af" << "bg" << "bn" << "ca" << "eu" << "fur" << "fy" << "gu" << "ha" << "is" << "ku" <<
                 "lb" << "ml" << "mr" << "nah" << "ne" << "om" << "or" << "pa" << "pap" << "ps" << "so" << "sq" << "sw" << "ta" << "te" <<
                 "tk" << "ur" << "zu" << "mn" << "gsw" << "chr" << "rm" << "pt" << "an" << "ast", new PluralRules_One());
    addRules(QStringList() <<"cs" << "sk", new PluralRules_Czech());
    addRules(QStringList() <<"ff" << "fr" << "kab", new PluralRules_French());
    addRules(QStringList() <<"ru" << "uk" << "be" << "sh", new PluralRules_Balkan());
    addRules(QStringList() <<"sr" << "hr" << "bs", new PluralRules_Serbian());
    addRules(QStringList() <<"lv", new PluralRules_Latvian());
    addRules(QStringList() <<"lt", new PluralRules_Lithuanian());
    addRules(QStringList() <<"pl", new PluralRules_Polish());
    addRules(QStringList() <<"ro" << "mo", new PluralRules_Romanian());
    addRules(QStringList() <<"sl", new PluralRules_Slovenian());
    addRules(QStringList() <<"ar", new PluralRules_Arabic());
    addRules(QStringList() <<"mk", new PluralRules_Macedonian());
    addRules(QStringList() <<"cy", new PluralRules_Welsh());
    addRules(QStringList() <<"br", new PluralRules_Breton());
    addRules(QStringList() <<"lag", new PluralRules_Langi());
    addRules(QStringList() <<"shi", new PluralRules_Tachelhit());
    addRules(QStringList() <<"mt", new PluralRules_Maltese());
    addRules(QStringList() <<"ga" << "se" << "sma" << "smi" << "smj" << "smn" << "sms", new PluralRules_Two());
    addRules(QStringList() <<"ak" << "am" << "bh" << "fil" << "tl" << "guw" << "hi" << "ln" << "mg" << "nso" << "ti" << "wa", new PluralRules_Zero());
    addRules(QStringList() <<"az" << "bm" << "fa" << "ig" << "hu" << "ja" << "kde" << "kea" << "ko" << "my" << "ses" << "sg" << "to" <<
                 "tr" << "vi" << "wo" << "yo" << "zh" << "bo" << "dz" << "id" << "jv" << "jw" << "ka" << "km" << "kn" << "ms" << "th" << "in", new PluralRules_None());
    // clang-format on
}

QString Locale::getEmptyString() const
{
    return {};
}

QString Locale::getString(const QString &key) const
{
    if (!m_languagePack.contains(key))
    {
        qDebug() << "LOC_ERR:" << key;
        return key;
    }

    const std::string &original = m_languagePack.value(key).toStdString();

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
    }

    static const std::unordered_map<std::string, std::string> replacements = {{"%%", "%"}, {"%s", "%1"}, {"EEEE", "dddd"}, {"EEE", "ddd"}};

    for (const auto &[from, to] : replacements)
    {
        replaceAll(result, from, to);
    }

    static std::regex boldRegex(R"(\*\*(.*?)\*\*)");
    result = std::regex_replace(result, boldRegex, "<b>$1</b>");

    static std::regex italicRegex(R"(\*(.*?)\*)");
    result = std::regex_replace(result, italicRegex, "<i>$1</i>");

    // qDebug() << QString::fromStdString(result);

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
        const auto minutes = std::floor(duration % 3600 / 3600);
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

QString Locale::languagePlural() const
{
    return m_languagePlural;
}

void Locale::setLanguagePlural(const QString &value)
{
    if (m_languagePlural != value)
    {
        m_languagePlural = value;
        emit languageChanged();
    }
}

QString Locale::stringForQuantity(Locale::Quantity quantity) const
{
    switch (quantity)
    {
        case Locale::QuantityZero:
            return "_zero_value";
        case Locale::QuantityOne:
            return "_one_value";
        case Locale::QuantityTwo:
            return "_two_value";
        case Locale::QuantityFew:
            return "_few_value";
        case Locale::QuantityMany:
            return "_many_value";
        default:
            return "_other_value";
    }
}

void Locale::addRules(const QStringList &languages, PluralRules *rules)
{
    std::ranges::for_each(languages, [&, this](const auto &x) { m_allRules.insert(x, rules); });
}

void Locale::processStrings(const QVariantMap &languagePackStrings)
{
    static const std::unordered_map<std::string, std::function<void(const QVariantMap &)>> stringTypeHandlers = {
        {"languagePackStringValueOrdinary",
         [this](const QVariantMap &value) {
             m_languagePack.insert(value.value("key").toString(), value.value("value").toMap().value("value").toString());
         }},
        {"languagePackStringValuePluralized",
         [this](const QVariantMap &value) {
             const auto &pluralValues = value.value("value").toMap();
             if (!pluralValues.isEmpty())
             {
                 const auto keyBase = value.value("key").toString();
                 for (const char *suffix : {"zero_value", "one_value", "two_value", "few_value", "many_value", "other_value"})
                 {
                     if (const auto &stringValue = pluralValues.value(suffix).toString(); !stringValue.isEmpty())
                     {
                         m_languagePack.insert(keyBase + "_" + suffix, stringValue);
                     }
                 }
             }
         }},
        {"languagePackStringValueDeleted", [](const QVariantMap &) {
             // No action needed for deleted strings
         }}};

    const auto strings = languagePackStrings.value("strings").toList();
    for (const auto &value : strings)
    {
        const auto valueType = value.toMap().value("value").toMap().value("@type").toString();

        if (auto it = stringTypeHandlers.find(valueType.toStdString()); it != stringTypeHandlers.end())
        {
            it->second(value.toMap());
        }
    }

    updatePluralRules();
}

void Locale::updatePluralRules()
{
    if (m_allRules.contains(m_languagePlural))
        m_currentPluralRules = m_allRules.value(m_languagePlural);
    else
        m_currentPluralRules = m_allRules.value("en");

    emit languageChanged();
}
