#pragma once

#include <QVariant>

class Localization : public QObject
{
    Q_OBJECT

public:
    Localization(QObject *parent = nullptr);
    ~Localization();

private slots:
    void handleLanguagePackStrings(const QString &localizationTarget, const QString &languagePackId, const QVariantList &strings);

private:
};
