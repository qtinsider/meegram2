#pragma once

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>

#include <QObject>

#include <functional>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <unordered_map>

class Client : public QObject
{
    Q_OBJECT

public:
    explicit Client(QObject *parent = nullptr);

    int clientId() const noexcept;

    void send(td::td_api::object_ptr<td::td_api::Function> request, std::function<void(td::td_api::object_ptr<td::td_api::Object>)> callback);

signals:
    void result(td::td_api::Object *object);

private:
    void initialize();

    int m_clientId;

    std::unique_ptr<td::ClientManager> m_clientManager;

    std::jthread m_worker;
    std::shared_mutex m_handlerMutex;
    std::atomic<std::uint64_t> m_requestId{0};
    std::unordered_map<std::uint64_t, std::function<void(td::td_api::object_ptr<td::td_api::Object>)>> m_handlers;
};
