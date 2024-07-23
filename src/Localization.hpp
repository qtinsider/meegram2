#pragma once

#include "Common.hpp"

#include <QObject>
#include <QVariant>

#include <memory>
#include <unordered_map>

class PluralRules
{
public:
    virtual ~PluralRules() = default;

    enum Quantity {
        QuantityOther = 0x0000,
        QuantityZero = 0x0001,
        QuantityOne = 0x0002,
        QuantityTwo = 0x0004,
        QuantityFew = 0x0008,
        QuantityMany = 0x0010,
    };

    virtual Quantity quantityForNumber(int count) const = 0;
    virtual std::unique_ptr<PluralRules> clone() const = 0;
};

class Locale : public QObject
{
    Q_OBJECT
public:
    explicit Locale(QObject *parent = nullptr);

    QString getString(const QString &key) const;
    QString formatPluralString(const QString &key, int plural) const;
    QString formatCallDuration(int duration) const;
    QString formatTtl(int ttl) const;

    QString languagePlural() const;
    void setLanguagePlural(const QString &value);

    void processStrings(const QVariantMap &languagePackStrings);

private:
    QString stringForQuantity(PluralRules::Quantity quantity) const;

    void addRules(const QStringList &languages, std::unique_ptr<PluralRules> rules);
    void updatePluralRules();

    QString m_languagePlural;
    std::unordered_map<QString, QString> m_languagePack;
    std::unordered_map<QString, std::unique_ptr<PluralRules>> m_allRules;
    std::unique_ptr<PluralRules> m_currentPluralRules;
};
