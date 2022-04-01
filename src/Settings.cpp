#include "Settings.hpp"

#include "Serialize.hpp"
#include "TdApi.hpp"

#include <QDebug>
#include <QLocale>

Settings::Settings()
{
    QVariantMap request;
    request.insert("@type", "getLocalizationTargetInfo");

    TdApi::getInstance().sendRequest(request, [](const auto &value) {
        if (value.value("@type").toByteArray() == "localizationTargetInfo")
        {
        }
    });
}

Settings &Settings::getInstance()
{
    static Settings staticObject;
    return staticObject;
}

void Settings::load()
{
    m_language = m_settings.value("language", QLocale::system().name().left(2)).toString();
}

void Settings::setLanguage(QString language)
{
    if (language == m_language)
        return;

    m_language = language;
    save();
    emit languageChanged();
}

QString Settings::language() const
{
    return !m_language.isEmpty() ? m_language : QLocale::system().name().left(2);
}

void Settings::save()
{
    m_settings.setValue("language", language());

    m_settings.sync();
}
