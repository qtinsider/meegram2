#include "Localization.hpp"

#include "Common.hpp"
#include "Serialize.hpp"
#include "TdApi.hpp"

#include <QDebug>
#include <QDir>

Localization::Localization(QObject *parent)
    : QObject(parent)
{
    connect(&TdApi::getInstance(), SIGNAL(updateLanguagePackStrings(const QString &, const QString &, const QVariantList &)),
            SLOT(handleLanguagePackStrings(const QString &, const QString &, const QVariantList &)));

    TdApi::getInstance().setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    TdApi::getInstance().setOption("localization_target", "android");
    TdApi::getInstance().setOption("language_pack_id", DefaultLanguageCode);
}

Localization::~Localization()
{
}

void Localization::handleLanguagePackStrings(const QString &localizationTarget, const QString &languagePackId, const QVariantList &strings)
{
}

