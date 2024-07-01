#pragma once

#include "Client.hpp"
#include "Common.hpp"
#include "TdApi.hpp"

#include <QObject>
#include <QVariant>

class Locale;
class StorageManager;

class TdManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(TdApi::AuthorizationState authorizationState READ getAuthorizationState NOTIFY authorizationStateChanged)

    Q_PROPERTY(Locale *locale READ locale WRITE setLocale)
    Q_PROPERTY(StorageManager *storageManager READ storageManager WRITE setStorageManager)
public:
    explicit TdManager(QObject *parent = nullptr);

    // TODO(strawberry): async function
    void sendRequest(const QVariantMap &request, std::function<void(const QVariantMap &)> callback = {});

    TdApi::AuthorizationState getAuthorizationState() const noexcept;

    StorageManager *storageManager() const;
    void setStorageManager(StorageManager *storageManager);

    Locale *locale() const;
    void setLocale(Locale *locale);

    Q_INVOKABLE void addFileToDownloads(int fileId, qint64 chatId, qint64 messageId, int priority = 30);
    Q_INVOKABLE void downloadFile(qint32 fileId, qint32 priority, qint32 offset, qint32 limit, bool synchronous);
    Q_INVOKABLE void setLogVerbosityLevel(qint32 newVerbosityLevel);

public slots:
    void close() noexcept;
    void setOption(const QString &name, const QVariant &value);

signals:
    void authorizationStateChanged(TdApi::AuthorizationState state);

private slots:
    void handleResult(const QVariantMap &object);

private:
    void initialLanguagePack();
    void initialParameters();
    void initialReady();

    void handleAuthorizationState(const QVariantMap &data);

    Client *m_client;
    Locale *m_locale;
    StorageManager *m_storageManager;

    TdApi::AuthorizationState m_state{TdApi::AuthorizationStateClosed};
};
