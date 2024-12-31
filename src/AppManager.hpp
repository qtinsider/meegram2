#pragma once

#include "LanguagePackInfoModel.hpp"

#include <td/telegram/td_api.h>

#include <QObject>

#include <array>
#include <memory>

class Authorization;
class ChatManager;
class Client;
class Locale;
class StorageManager;
class Settings;

class AppManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool authorized READ isAuthorized NOTIFY authorizedChanged)
    Q_PROPERTY(QString connectionStateString READ connectionStateString NOTIFY connectionStateChanged)

    Q_PROPERTY(Client *client READ client NOTIFY clientChanged)
    Q_PROPERTY(Authorization *authorization READ authorization NOTIFY authorizationChanged)
    Q_PROPERTY(Locale *locale READ locale NOTIFY localeChanged)
    Q_PROPERTY(Settings *settings READ settings NOTIFY settingsChanged)
    Q_PROPERTY(StorageManager *storageManager READ storageManager NOTIFY storageManagerChanged)

    Q_PROPERTY(ChatManager *chatManager READ chatManager NOTIFY chatManagerChanged)

    Q_PROPERTY(LanguagePackInfoModel *languagePackInfoModel READ languagePackInfoModel NOTIFY languagePackInfoModelChanged)
public:
    explicit AppManager(QObject *parent = nullptr);

    bool isAuthorized() const noexcept;

    const QString &connectionStateString() const noexcept;

    Client *client() const noexcept;
    Authorization *authorization() const noexcept;
    Locale *locale() const noexcept;
    Settings *settings() const noexcept;
    StorageManager *storageManager() const noexcept;

    ChatManager *chatManager() const noexcept;

    LanguagePackInfoModel *languagePackInfoModel() const noexcept;

signals:
    void clientChanged();
    void authorizationChanged();
    void localeChanged();
    void settingsChanged();
    void storageManagerChanged();
    void chatManagerChanged();
    void languagePackInfoModelChanged();

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

private:
    void setParameters() noexcept;

    void checkInitializationStatus() noexcept;

    void handleAuthorizationState(const td::td_api::AuthorizationState &authorizationState);
    void handleConnectionState(const td::td_api::ConnectionState &connectionState);

    bool m_isAuthorized{false};

    QString m_connectionStateString;

    std::shared_ptr<Client> m_client;
    std::shared_ptr<Authorization> m_authorization;
    std::shared_ptr<Locale> m_locale;
    std::shared_ptr<Settings> m_settings;

    std::shared_ptr<ChatManager> m_chatManager;
    std::shared_ptr<StorageManager> m_storageManager;

    std::unique_ptr<LanguagePackInfoModel> m_languagePackInfoModel;

    std::array<bool, 2> m_initializationStatus{false, false};
};
