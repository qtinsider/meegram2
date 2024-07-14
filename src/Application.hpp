#pragma once

#include "TdApi.hpp"

#include <QApplication>
#include <QObject>
#include <QVariant>

#include <array>

class Client;
class Locale;
class Settings;
class StorageManager;

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool authorized READ isAuthorized NOTIFY authorizedChanged)

    Q_PROPERTY(QObject *client READ client NOTIFY clientChanged)
    Q_PROPERTY(QObject *locale READ locale NOTIFY localeChanged)
    Q_PROPERTY(QObject *settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(QObject *storageManager READ storageManager NOTIFY storageManagerChanged)

    Q_PROPERTY(QVariantList countries READ countries NOTIFY countriesChanged)
    Q_PROPERTY(QString connectionStateString READ connectionStateString NOTIFY connectionStateChanged)
public:
    explicit Application(QObject *parent = nullptr);

    bool isAuthorized() const noexcept;

    QObject *client() const noexcept;
    QObject *locale() const noexcept;
    QObject *settings() const noexcept;
    QObject *storageManager() const noexcept;

    const QVariantList &countries() const noexcept;
    const QString &connectionStateString() const noexcept;

    Q_INVOKABLE void initialize();
    Q_INVOKABLE QString getFormattedText(const QVariantMap &formattedText) const noexcept;

signals:
    void authorizedChanged();

    void clientChanged();
    void localeChanged();
    void settingsChanged();
    void storageManagerChanged();
    void countriesChanged();
    void connectionStateChanged();

    void appInitialized();

public slots:
    void close() noexcept;
    void setOption(const QString &name, const QVariant &value);

private slots:
    void handleResult(const QVariantMap &object);

private:
    void initializeParameters() noexcept;
    void initializeLanguagePack() noexcept;
    void initializeCountries() noexcept;
    void checkInitializationStatus() noexcept;

    void handleAuthorizationState(const QVariantMap &authorizationState);
    void handleConnectionState(const QVariantMap &connectionState);

    Client *m_client;
    Locale *m_locale;
    Settings *m_settings;
    StorageManager *m_storageManager;

    QVariantList m_countries;
    QString m_connectionStateString;


    bool m_isAuthorized{false};

    std::array<bool, 3> m_initializationStatus{false, false, false};
};
