#pragma once

#include "Client.hpp"
#include "Common.hpp"

#include <QObject>
#include <QVariant>

#include <atomic>
#include <coroutine>
#include <stdexcept>

class Locale;
class StorageManager;

class ResponseAwaiter
{
public:
    explicit ResponseAwaiter(Client *client, const QVariantMap &request) noexcept
        : client_(client)
        , request_(request)
        , ready_(false)
    {
    }

    bool await_ready() const noexcept
    {
        return ready_.load(std::memory_order_acquire);
    }

    void await_suspend(std::coroutine_handle<> handle)
    {
        client_->send(request_, [this, handle](const QVariantMap &res) {
            response_ = res;
            ready_.store(true, std::memory_order_release);
            handle.resume();
        });
    }

    QVariantMap await_resume()
    {
        if (!ready_.load(std::memory_order_acquire))
        {
            throw std::runtime_error("Response not ready");
        }
        return std::move(response_);
    }

private:
    Client *client_;
    QVariantMap request_;
    QVariantMap response_;
    std::atomic_bool ready_;
};

class ResponseAwaitable
{
public:
    ResponseAwaitable(Client *client, const QVariantMap &request) noexcept
        : client_(client)
        , request_(request)
    {
    }

    ResponseAwaiter operator co_await() const
    {
        return ResponseAwaiter{client_, request_};
    }

private:
    Client *client_;
    QVariantMap request_;
};

class TaskPromise
{
public:
    struct promise_type
    {
        promise_type() = default;

        std::suspend_never initial_suspend() const noexcept
        {
            return {};
        }
        std::suspend_never final_suspend() const noexcept
        {
            return {};
        }
        TaskPromise get_return_object()
        {
            return TaskPromise();
        }
        void unhandled_exception()
        {
            std::terminate();
        }
        void return_void() const noexcept
        {
        }

        template <typename... Args>
        auto await_transform(Args &&...args)
        {
            return ResponseAwaitable{std::forward<Args>(args)...};
        }
    };
};

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
