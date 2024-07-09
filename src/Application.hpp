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

    Q_PROPERTY(Client *client READ client NOTIFY clientChanged)
    Q_PROPERTY(Locale *locale READ locale NOTIFY localeChanged)
    Q_PROPERTY(Settings *settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(StorageManager *storageManager READ storageManager NOTIFY storageManagerChanged)

    Q_PROPERTY(QVariantList countries READ countries NOTIFY countriesChanged)
public:
    explicit Application(QObject *parent = nullptr);

    bool isAuthorized() const noexcept;

    Client *client() const noexcept;
    Locale *locale() const noexcept;
    Settings *settings() const noexcept;
    StorageManager *storageManager() const noexcept;

    const QVariantList &countries() const noexcept;

signals:
    void authorizedChanged();

    void clientChanged();
    void localeChanged();
    void settingsChanged();
    void storageManagerChanged();
    void countriesChanged();

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

    void handleAuthorizationState(const QVariantMap &data);

    Client *m_client;
    Locale *m_locale;
    Settings *m_settings;
    StorageManager *m_storageManager;

    QVariantList m_countries;

    bool m_isAuthorized{false};

    std::array<bool, 3> m_initializationStatus{false, false, false};
};
