#pragma once

#include "TdApi.hpp"

#include <td/telegram/td_api.h>

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
    explicit Application(QObject *parent = nullptr);

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
    void handleResult(td::td_api::Object *object);

    void initializeLanguagePack() noexcept;

private:
    void initializeParameters() noexcept;
    void initializeCountries() noexcept;
    void initializeLanguagePackInfo();

    void checkInitializationStatus() noexcept;

    void handleAuthorizationState(const td::td_api::AuthorizationState &authorizationState);
    void handleConnectionState(const td::td_api::ConnectionState &connectionState);

    Client *m_client{};
    Locale *m_locale{};
    Settings *m_settings{};
    StorageManager *m_storageManager{};

    bool m_isAuthorized = false;

    QString m_connectionStateString;

    std::array<bool, 4> m_initializationStatus{false, false, false, false};
};
