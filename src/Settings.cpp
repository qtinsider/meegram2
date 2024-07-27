#include "Settings.hpp"

#include "Common.hpp"

#include <QCoreApplication>
#include <QSettings>

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), QCoreApplication::applicationName(), parent))
{
    m_languagePackId = m_settings->value("languagePackId", DefaultLanguageCode).toString();
    m_languagePluralId = m_settings->value("languagePluralId", DefaultLanguageCode).toString();
}

QString Settings::languagePackId() const
{
    return m_languagePackId;
}

void Settings::setLanguagePackId(const QString &value)
{
    if (m_languagePackId != value)
    {
        m_languagePackId = value;
        m_settings->setValue("languagePackId", m_languagePackId);
        emit languagePackIdChanged();
    }
}

QString Settings::languagePluralId() const
{
    return m_languagePluralId;
}

void Settings::setLanguagePluralId(const QString &value)
{
    if (m_languagePluralId != value)
    {
        m_languagePluralId = value;
        m_settings->setValue("languagePluralId", m_languagePluralId);
        emit languagePluralIdChanged();
    }
}
