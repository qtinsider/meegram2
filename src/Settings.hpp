#pragma once

#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
public:
    Settings(const Settings &) = delete;
    Settings &operator=(const Settings &) = delete;

    static Settings &getInstance();

    void save();
    void load();

    QString language() const;
    void setLanguage(QString language);

signals:
    void languageChanged();

private:
    Settings();

    QString m_language;

    QSettings m_settings;
};
