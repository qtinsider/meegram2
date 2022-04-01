#pragma once

#include <QVariant>

enum class Quantity {
    QuantityOther = 0x0000,
    QuantityZero = 0x0001,
    QuantityOne = 0x0002,
    QuantityTwo = 0x0004,
    QuantityFew = 0x0008,
    QuantityMany = 0x0010,
};

class PluralRules;

class Localization : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString emptyString READ getEmptyString NOTIFY languageChanged)
public:
    Localization(const Localization &) = delete;
    Localization &operator=(const Localization &) = delete;

    static Localization &getInstance();

    QString getEmptyString() const;

    Q_INVOKABLE QString getString(const QString &key) const;

    QString formatPluralString(const QString &key, int plural) const;
    QString formatCallDuration(int duration) const;
    QString formatTtl(int ttl) const;

signals:
    void languageChanged();

private slots:
    void loadLanguage();

    void handleLanguagePackStrings(const QString &localizationTarget, const QString &languagePackId, const QVariantList &strings);

private:
    Localization();

    QString stringForQuantity(Quantity quantity) const;

    void addRules(const QStringList &languages, PluralRules *rules);

    void processStrings(const QVariantMap &languagePackStrings);
    void updatePluralRules();

    QMap<QString, QString> m_languagePack;
    QMap<QString, PluralRules *> m_allRules;
    PluralRules *m_currentPluralRules;
};
