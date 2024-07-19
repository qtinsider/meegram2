#pragma once

#include <QObject>

class QSettings;

class Settings : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString languagePluralId READ languagePluralId WRITE setLanguagePluralId NOTIFY languagePluralIdChanged)

public:
    explicit Settings(QObject *parent = nullptr);


    QString languagePluralId() const;
    void setLanguagePluralId(const QString &value);


signals:
    void languagePluralIdChanged();

private:
    QSettings *m_settings;

    QString m_languagePluralId;
};
