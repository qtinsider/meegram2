#pragma once

#include "TdApi.hpp"

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
    Q_PROPERTY(QVariantList languagePackInfo READ languagePackInfo NOTIFY languagePackInfoChanged)
    Q_PROPERTY(QString connectionStateString READ connectionStateString NOTIFY connectionStateChanged)

    Q_PROPERTY(QString emptyString READ getEmptyString NOTIFY languageChanged)

public:
    explicit Application(QObject *parent = nullptr);

    bool isAuthorized() const noexcept;

    QString getEmptyString() const;

    QObject *client() const noexcept;
    QObject *locale() const noexcept;
    QObject *settings() const noexcept;
    QObject *storageManager() const noexcept;

    const QVariantList &countries() const noexcept;
    const QVariantList &languagePackInfo() const noexcept;
    const QString &connectionStateString() const noexcept;

    Q_INVOKABLE QString getString(const QString &key) const noexcept;
    Q_INVOKABLE QString getFormattedText(const QVariantMap &formattedText) const noexcept;

signals:
    void authorizedChanged();

    void clientChanged();
    void localeChanged();
    void settingsChanged();
    void storageManagerChanged();
    void countriesChanged();
    void languagePackInfoChanged();
    void connectionStateChanged();
    void languageChanged();

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

    QVariantList m_countries;
    QVariantList m_languagePackInfo;

    QString m_connectionStateString;

    bool m_isAuthorized{false};

    std::array<bool, 4> m_initializationStatus{false, false, false, false};
};
