#pragma once

#include "TdApi.hpp"

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

    Q_PROPERTY(QString emptyString READ emptyString NOTIFY languageChanged)
    Q_PROPERTY(QString connectionStateString READ connectionStateString NOTIFY connectionStateChanged)

public:
    explicit Application(Settings *settings, StorageManager *storageManager, QObject *parent = nullptr);

    bool isAuthorized() const noexcept;

    QString emptyString() const noexcept;

    const QString &connectionStateString() const noexcept;

    Q_INVOKABLE QString getString(const QString &key) const noexcept;

signals:
    void authorizedChanged();

    void languageChanged();
    void connectionStateChanged();

    void appInitialized();

public slots:
    void close() noexcept;
    void setOption(const QString &name, const QVariant &value);
    void initialize() noexcept;

private slots:
    void handleResult(const QVariantMap &object);

    void initializeLanguagePack() noexcept;

private:
    void initializeParameters() noexcept;
    void initializeCountries() noexcept;
    void initializeLanguagePackInfo();

    void checkInitializationStatus() noexcept;

    void handleAuthorizationState(const QVariantMap &authorizationState);
    void handleConnectionState(const QVariantMap &connectionState);

    Client *m_client{};
    Locale *m_locale{};
    Settings *m_settings{};
    StorageManager *m_storageManager{};

    bool m_isAuthorized = false;

    QString m_connectionStateString;

    std::array<bool, 4> m_initializationStatus{false, false, false, false};
};
