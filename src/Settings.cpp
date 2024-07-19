#include "Settings.hpp"

#include "Common.hpp"

#include <QSettings>

Settings::Settings(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings("/home/insider/settings.ini", QSettings::IniFormat))
{
    m_languagePluralId = m_settings->value("languagePluralId", DefaultLanguageCode).toString();
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

