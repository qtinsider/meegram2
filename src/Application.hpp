#pragma once

#include <td/telegram/td_api.h>

#include <QObject>

#include <array>
#include <memory>

class Chat;
class Client;
class Locale;
class Settings;
class StorageManager;

class Application : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool authorized READ isAuthorized NOTIFY authorizedChanged)

    Q_PROPERTY(QString connectionStateString READ connectionStateString NOTIFY connectionStateChanged)

public:
    explicit Application(QObject *parent = nullptr);

    bool isAuthorized() const noexcept;

    const QString &connectionStateString() const noexcept;

    Q_INVOKABLE Chat *getChat(qlonglong chatId) const noexcept;

    Q_INVOKABLE QString translate(const char *key, int plural = -1) const;

signals:
    void authorizedChanged();
    void connectionStateChanged();

    void appInitialized();

public slots:
    void close() noexcept;
    void setOption(const QString &name, const QVariant &value);

    void initialize() noexcept;

private slots:
    void handleResult(td::td_api::Object *object);

    void loadLanguagePack() noexcept;

    void retranslateUi() noexcept;

private:
    void setParameters() noexcept;

    void checkInitializationStatus() noexcept;

    void handleAuthorizationState(const td::td_api::AuthorizationState &authorizationState);
    void handleConnectionState(const td::td_api::ConnectionState &connectionState);

    std::shared_ptr<Client> m_client;

    Locale *m_locale{};
    Settings *m_settings{};
    StorageManager *m_storageManager{};

    bool m_isAuthorized{false};

    QString m_connectionStateString;

    std::array<bool, 2> m_initializationStatus{false, false};
};
