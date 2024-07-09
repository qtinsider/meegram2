#pragma once

#include <QObject>
#include <QVariant>

#include <functional>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

class Client : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int clientId READ clientId CONSTANT)
public:
    explicit Client(QObject *parent = nullptr);

    int clientId() const noexcept;

    void send(const QVariantMap &request, std::function<void(const QVariantMap &)> callback = {});

    Q_INVOKABLE static QVariantMap execute(const QVariantMap &request);

signals:
    void result(const QVariantMap &object);

private:
    void initialize();

    int m_clientId;
    std::atomic_uint64_t m_requestId;
    std::jthread m_worker;
    mutable std::shared_mutex m_handlerMutex;
    std::unordered_map<std::uint64_t, std::function<void(const QVariantMap &)>> m_handlers;
};
