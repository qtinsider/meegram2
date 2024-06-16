#pragma once

#include <QObject>
#include <QStringList>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString languagePackId READ languagePackId WRITE setLanguagePackId NOTIFY languagePackIdChanged)
    Q_PROPERTY(QString languagePluralId READ languagePluralId WRITE setLanguagePluralId NOTIFY languagePluralIdChanged)
    Q_PROPERTY(QString languageShownId READ languageShownId WRITE setLanguageShownId NOTIFY languageShownIdChanged)

public:
    explicit Settings(QObject *parent = nullptr);

    QString languagePackId() const;
    void setLanguagePackId(const QString &value);

    QString languagePluralId() const;
    void setLanguagePluralId(const QString &value);

    QString languageShownId() const;
    void setLanguageShownId(const QString &value);

signals:
    void languagePackIdChanged();
    void languagePluralIdChanged();
    void languageShownIdChanged();

private:
    QSettings *m_settings;

    QString m_languagePackId;
    QString m_languagePluralId;
    QString m_languageShownId;
};
