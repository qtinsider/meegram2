#pragma once

#include <QTranslator>
#include <QVariant>

class Localization : public QTranslator
{
    Q_OBJECT

public:
    Localization(QObject *parent = nullptr);
    ~Localization();

    QString translate(const char *context, const char *sourceText, const char *disambiguation = 0) const;

private slots:
    void handleLanguagePackStrings(const QString &localizationTarget, const QString &languagePackId, const QVariantList &strings);
    void handleLocalizationTargetInfo(const QVariantMap &data);

private:
};
