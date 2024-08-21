#include "Client.hpp"

#include <mutex>

Client::Client(QObject *parent)
    : QObject(parent)
    , m_clientManager(std::make_unique<td::ClientManager>())
{
    // disable TDLib logging
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));

    m_clientId = m_clientManager->create_client_id();

    initialize();
}

int Client::clientId() const noexcept
{
    return m_clientId;
}

void Client::send(td::td_api::object_ptr<td::td_api::Function> request, std::function<void(td::td_api::object_ptr<td::td_api::Object>)> callback)
{
    auto id = m_requestId.fetch_add(1, std::memory_order_relaxed);
    if (callback)
    {
        std::unique_lock lock(m_handlerMutex);

        m_handlers.emplace(id, std::move(callback));
    }
    m_clientManager->send(m_clientId, id, std::move(request));
}

void Client::initialize()
{
    // A worker thread using std::jthread for automatic joining
    m_worker = std::jthread([this](std::stop_token token) {
        while (!token.stop_requested())
        {
            auto response = m_clientManager->receive(30.0);
            if (!response.object)
            {
                continue;
            }

            if (response.request_id != 0)
            {
                std::function<void(td::td_api::object_ptr<td::td_api::Object>)> handler;
                {
                    std::shared_lock lock(m_handlerMutex);
                    auto it = m_handlers.find(response.request_id);
                    if (it != m_handlers.end())
                    {
                        handler = std::move(it->second);
                    }
                }

                if (handler)
                {
                    handler(std::move(response.object));
                    {
                        std::unique_lock lock(m_handlerMutex);
                        m_handlers.erase(response.request_id);
                    }
                }
            }
            else
            {
                emit result(response.object.release());
            }
        }
    });
}
