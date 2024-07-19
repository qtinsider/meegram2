#pragma once

#include <QObject>
#include <QStringList>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString languagePackId READ languagePackId WRITE setLanguagePackId NOTIFY languagePackIdChanged)
    Q_PROPERTY(QString languagePluralId READ languagePluralId WRITE setLanguagePluralId NOTIFY languagePluralIdChanged)

public:
    explicit Settings(QObject *parent = nullptr);

    QString languagePackId() const;
    void setLanguagePackId(const QString &value);

    QString languagePluralId() const;
    void setLanguagePluralId(const QString &value);


signals:
    void languagePackIdChanged();
    void languagePluralIdChanged();

private:
    QSettings *m_settings;

    QString m_languagePackId;
    QString m_languagePluralId;
};
