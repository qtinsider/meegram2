#include "Localization.hpp"

#include "Common.hpp"
#include "Serialize.hpp"
#include "TdApi.hpp"

#include <QDebug>
#include <QDir>

Localization::Localization(QObject *parent)
    : QTranslator(parent)
{
    connect(&TdApi::getInstance(), SIGNAL(updateLanguagePackStrings(const QString &, const QString &, const QVariantList &)),
            SLOT(handleLanguagePackStrings(const QString &, const QString &, const QVariantList &)));

    connect(&TdApi::getInstance(), SIGNAL(localizationTargetInfo(const QVariantMap &)),
            SLOT(handleLocalizationTargetInfo(const QVariantMap &)));

    TdApi::getInstance().setOption("language_pack_database_path", QString(QDir::homePath() + DatabaseDirectory + "/langpack"));
    TdApi::getInstance().setOption("localization_target", "android");
    TdApi::getInstance().setOption("language_pack_id", DefaultLanguageCode);
}

Localization::~Localization()
{
}

QString Localization::translate(const char *context, const char *sourceText, const char *disambiguation) const
{
    return {};
}

void Localization::handleLanguagePackStrings(const QString &localizationTarget, const QString &languagePackId, const QVariantList &strings)
{
}

void Localization::handleLocalizationTargetInfo(const QVariantMap &data)
{
    nlohmann::json j(data);

    qDebug() << j.dump(2).c_str();
}
