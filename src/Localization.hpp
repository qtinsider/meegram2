#pragma once

#include "Common.hpp"
#include "PluralRules.hpp"

#include <td/telegram/td_api.h>

#include <QHash>
#include <QTranslator>

#include <memory>
#include <unordered_map>

class Locale : public QTranslator
{
    Q_OBJECT
public:
    explicit Locale(QObject *parent = nullptr);

    QString translate(const char *key, int plural = -1) const;
    QString translate(const char *context, const char *sourceText, const char *disambiguation) const override;

    QString getString(const char *key) const;
    QString formatPluralString(const char *key, int plural) const;
    QString formatCallDuration(int duration) const;
    QString formatTtl(int ttl, bool shorter = false) const;

    void setLanguagePlural(const QString &value);
    void setLanguagePackStrings(td::td_api::object_ptr<td::td_api::languagePackStrings> value);

private:
    QString stringForQuantity(PluralRules::Quantity quantity) const;

    void addRules(const QStringList &languages, std::unique_ptr<PluralRules> rules);
    void updatePluralRules();

    QString m_languagePlural;
    QHash<QString, QString> m_languagePack;

    std::unique_ptr<PluralRules> m_currentPluralRules;
    std::unordered_map<QString, std::unique_ptr<PluralRules>> m_allRules;
};
