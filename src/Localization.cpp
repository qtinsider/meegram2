#include "Localization.hpp"

#include "Common.hpp"
#include "Serialize.hpp"
#include "Settings.hpp"
#include "TdApi.hpp"

#include <QDebug>
#include <QDir>
#include <QStringBuilder>

class PluralRules
{
public:
    virtual Quantity quantityForNumber(int count) = 0;
};

class PluralRules_Zero : public PluralRules
{
    Quantity quantityForNumber(int count)
    {
        if (count == 0 || count == 1)
        {
            return Quantity::QuantityOne;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Welsh : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Quantity::QuantityZero;
        }
        else if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (count == 2)
        {
            return Quantity::QuantityTwo;
        }
        else if (count == 3)
        {
            return Quantity::QuantityFew;
        }
        else if (count == 6)
        {
            return Quantity::QuantityMany;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Two : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (count == 2)
        {
            return Quantity::QuantityTwo;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Tachelhit : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count >= 0 && count <= 1)
        {
            return Quantity::QuantityOne;
        }
        else if (count >= 2 && count <= 10)
        {
            return Quantity::QuantityFew;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Slovenian : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (rem100 == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (rem100 == 2)
        {
            return Quantity::QuantityTwo;
        }
        else if (rem100 >= 3 && rem100 <= 4)
        {
            return Quantity::QuantityFew;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Romanian : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if ((count == 0 || (rem100 >= 1 && rem100 <= 19)))
        {
            return Quantity::QuantityFew;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Polish : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        {
            return Quantity::QuantityFew;
        }
        else if ((rem10 >= 0 && rem10 <= 1) || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 12 && rem100 <= 14))
        {
            return Quantity::QuantityMany;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_One : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        return count == 1 ? Quantity::QuantityOne : Quantity::QuantityOther;
    }
};

class PluralRules_None : public PluralRules
{
    Quantity quantityForNumber(int /*count*/) override
    {
        return Quantity::QuantityOther;
    }
};

class PluralRules_Maltese : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (count == 0 || (rem100 >= 2 && rem100 <= 10))
        {
            return Quantity::QuantityFew;
        }
        else if (rem100 >= 11 && rem100 <= 19)
        {
            return Quantity::QuantityMany;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Macedonian : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count % 10 == 1 && count != 11)
        {
            return Quantity::QuantityOne;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Lithuanian : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && !(rem100 >= 11 && rem100 <= 19))
        {
            return Quantity::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 9 && !(rem100 >= 11 && rem100 <= 19))
        {
            return Quantity::QuantityFew;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Latvian : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Quantity::QuantityZero;
        }
        else if (count % 10 == 1 && count % 100 != 11)
        {
            return Quantity::QuantityOne;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Langi : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Quantity::QuantityZero;
        }
        else if (count == 1)
        {
            return Quantity::QuantityOne;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_French : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count >= 0 && count < 2)
        {
            return Quantity::QuantityOne;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Czech : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (count >= 2 && count <= 4)
        {
            return Quantity::QuantityFew;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Breton : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        if (count == 0)
        {
            return Quantity::QuantityZero;
        }
        else if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (count == 2)
        {
            return Quantity::QuantityTwo;
        }
        else if (count == 3)
        {
            return Quantity::QuantityFew;
        }
        else if (count == 6)
        {
            return Quantity::QuantityMany;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Balkan : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && rem100 != 11)
        {
            return Quantity::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        {
            return Quantity::QuantityFew;
        }
        else if ((rem10 == 0 || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 11 && rem100 <= 14)))
        {
            return Quantity::QuantityMany;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Serbian : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        const auto rem10 = count % 10;
        if (rem10 == 1 && rem100 != 11)
        {
            return Quantity::QuantityOne;
        }
        else if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        {
            return Quantity::QuantityFew;
        }

        return Quantity::QuantityOther;
    }
};

class PluralRules_Arabic : public PluralRules
{
    Quantity quantityForNumber(int count) override
    {
        const auto rem100 = count % 100;
        if (count == 0)
        {
            return Quantity::QuantityZero;
        }
        else if (count == 1)
        {
            return Quantity::QuantityOne;
        }
        else if (count == 2)
        {
            return Quantity::QuantityTwo;
        }
        else if (rem100 >= 3 && rem100 <= 10)
        {
            return Quantity::QuantityFew;
        }
        else if (rem100 >= 11 && rem100 <= 99)
        {
            return Quantity::QuantityMany;
        }

        return Quantity::QuantityOther;
    }
};

Localization::Localization()
{
    connect(&TdApi::getInstance(), SIGNAL(updateLanguagePackStrings(const QString &, const QString &, const QVariantList &)),
            SLOT(handleLanguagePackStrings(const QString &, const QString &, const QVariantList &)));

    connect(&Settings::getInstance(), SIGNAL(languageChanged()), SIGNAL(languageChanged()));

    TdApi::getInstance().setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    TdApi::getInstance().setOption("localization_target", "android");
    TdApi::getInstance().setOption("language_pack_id", Settings::getInstance().language());

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

    loadLanguage();
}

Localization &Localization::getInstance()
{
    static Localization staticObject;
    return staticObject;
}

QString Localization::getEmptyString() const
{
    return {};
}

QString Localization::getString(const QString &key) const
{
    if (!m_languagePack.contains(key))
    {
        qDebug() << "LOC_ERR: " + key;
        return key;
    }

    auto result = m_languagePack.value(key);

    result.remove(QRegExp("\\$[ds]"));

    result.replace(QString::fromUtf8("%%"), QString::fromUtf8("%"));
    result.replace(QString::fromUtf8("%s"), QString::fromUtf8("%1"));

    result.replace(QString::fromUtf8("EEEE"), QString::fromUtf8("dddd"));
    result.replace(QString::fromUtf8("EEE"), QString::fromUtf8("ddd"));

    return result;
}

QString Localization::formatPluralString(const QString &key, int plural) const
{
    const auto pluralKey = key + stringForQuantity(m_currentPluralRules->quantityForNumber(plural));

    return getString(pluralKey).arg(plural);
}

QString Localization::formatCallDuration(int duration) const
{
    if (duration > 3600)
    {
        auto result = formatPluralString("CallDurationHours", std::floor(duration / 3600));
        auto minutes = std::floor(duration % 3600 / 3600);
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

QString Localization::formatTtl(int ttl) const
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

    auto days = ttl / 60 / 60 / 24;
    if (ttl % 7 == 0)
    {
        return formatPluralString("TTLStringWeeks", std::floor(days / 7));
    }

    return formatPluralString("TTLStringWeeks", std::floor(days / 7)) % formatPluralString("TTLStringDays", std::floor(days % 7));
}

void Localization::loadLanguage()
{
    QVariantMap request;
    request.insert("@type", "getLanguagePackStrings");
    request.insert("language_pack_id", Settings::getInstance().language());

    TdApi::getInstance().sendRequest(request, [this](const auto &value) { processStrings(value); });

    updatePluralRules();
}

void Localization::handleLanguagePackStrings(const QString &localizationTarget, const QString &languagePackId, const QVariantList &strings)
{
}

QString Localization::stringForQuantity(Quantity quantity) const
{
    switch (quantity)
    {
        case Quantity::QuantityZero:
            return "_zero";
        case Quantity::QuantityOne:
            return "_one";
        case Quantity::QuantityTwo:
            return "_two";
        case Quantity::QuantityFew:
            return "_few";
        case Quantity::QuantityMany:
            return "_many";
        default:
            return "_other";
    }
}

void Localization::addRules(const QStringList &languages, PluralRules *rules)
{
    std::ranges::for_each(languages, [&, this](const auto &x) { m_allRules.insert(x, rules); });
}

void Localization::processStrings(const QVariantMap &languagePackStrings)
{
    if (languagePackStrings.value("@type").toByteArray() != "languagePackStrings")
        return;

    for (auto strings = languagePackStrings.value("strings").toList(); const auto &value : strings)
    {
        auto valueType = value.toMap().value("value").toMap().value("@type").toByteArray();

        switch (fnv::hashRuntime(valueType.constData()))
        {
            case fnv::hash("languagePackStringValueOrdinary"): {
                m_languagePack.insert(value.toMap().value("key").toString(),
                                      value.toMap().value("value").toMap().value("value").toString());
                break;
            }
            case fnv::hash("languagePackStringValuePluralized"): {
                if (!value.toMap().value("value").toMap().value("zero_value").toString().isEmpty())
                {
                    m_languagePack.insert(value.toMap().value("key").toString().append("_zero"),
                                          value.toMap().value("value").toMap().value("zero_value").toString());
                }
                if (!value.toMap().value("value").toMap().value("one_value").toString().isEmpty())
                {
                    m_languagePack.insert(value.toMap().value("key").toString().append("_one"),
                                          value.toMap().value("value").toMap().value("one_value").toString());
                }
                if (!value.toMap().value("value").toMap().value("two_value").toString().isEmpty())
                {
                    m_languagePack.insert(value.toMap().value("key").toString().append("_two"),
                                          value.toMap().value("value").toMap().value("two_value").toString());
                }
                if (!value.toMap().value("value").toMap().value("few_value").toString().isEmpty())
                {
                    m_languagePack.insert(value.toMap().value("key").toString().append("_few"),
                                          value.toMap().value("value").toMap().value("few_value").toString());
                }
                if (!value.toMap().value("value").toMap().value("many_value").toString().isEmpty())
                {
                    m_languagePack.insert(value.toMap().value("key").toString().append("_many"),
                                          value.toMap().value("value").toMap().value("many_value").toString());
                }
                if (!value.toMap().value("value").toMap().value("other_value").toString().isEmpty())
                {
                    m_languagePack.insert(value.toMap().value("key").toString().append("_other"),
                                          value.toMap().value("value").toMap().value("other_value").toString());
                }
                break;
            }
            case fnv::hash("languagePackStringValueDeleted"): {
                break;
            }
        }
    }

    updatePluralRules();
}

void Localization::updatePluralRules()
{
    if (auto langId = Settings::getInstance().language(); m_allRules.contains(langId))
        m_currentPluralRules = m_allRules.value(langId);
    else
        m_currentPluralRules = m_allRules.value("en");

    emit languageChanged();
}
