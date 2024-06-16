#pragma once

#include <QVariant>

class PluralRules;

class Locale : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString languageCode READ languageCode WRITE setLanguageCode NOTIFY languageChanged)
    Q_PROPERTY(QString languagePlural READ languagePlural WRITE setLanguagePlural NOTIFY languageChanged)
    Q_PROPERTY(QString emptyString READ getEmptyString NOTIFY languageChanged)
public:
    explicit Locale(QObject *parent = nullptr);

    enum Quantity {
        QuantityOther = 0x0000,
        QuantityZero = 0x0001,
        QuantityOne = 0x0002,
        QuantityTwo = 0x0004,
        QuantityFew = 0x0008,
        QuantityMany = 0x0010,
    };

    QString getEmptyString() const;

    Q_INVOKABLE QString getString(const QString &key) const;

    QString formatPluralString(const QString &key, int plural) const;
    QString formatCallDuration(int duration) const;
    QString formatTtl(int ttl) const;

    QString languageCode() const;
    void setLanguageCode(const QString &value);

    QString languagePlural() const;
    void setLanguagePlural(const QString &value);

signals:
    void languageChanged();

public slots:
    void processStrings(const QVariantMap &languagePackStrings);

private:
    QString stringForQuantity(Quantity quantity) const;

    void addRules(const QStringList &languages, PluralRules *rules);

    void updatePluralRules();

    QString m_languageCode;
    QString m_languagePlural;

    QMap<QString, QString> m_languagePack;
    QMap<QString, PluralRules *> m_allRules;
    PluralRules *m_currentPluralRules;
};
