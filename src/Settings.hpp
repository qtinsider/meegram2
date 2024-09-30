#pragma once

#include <QObject>
#include <QStringList>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool invertedTheme READ invertedTheme WRITE setInvertedTheme NOTIFY invertedThemeChanged)

    Q_PROPERTY(QString languagePackId READ languagePackId WRITE setLanguagePackId NOTIFY languagePackIdChanged)
    Q_PROPERTY(QString languagePluralId READ languagePluralId WRITE setLanguagePluralId NOTIFY languagePluralIdChanged)

public:
    static Settings &instance();

    Settings(const Settings &) = delete;
    Settings &operator=(const Settings &) = delete;

    bool invertedTheme() const;
    void setInvertedTheme(bool value);

    QString languagePackId() const;
    void setLanguagePackId(const QString &value);

    QString languagePluralId() const;
    void setLanguagePluralId(const QString &value);

signals:
    void invertedThemeChanged();

    void languagePackIdChanged();
    void languagePluralIdChanged();

private:
    Settings();

    QSettings *m_settings{};

    bool m_invertedTheme;

    QString m_languagePackId;
    QString m_languagePluralId;
};
