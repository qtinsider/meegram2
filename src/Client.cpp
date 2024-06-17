#include "Client.hpp"

#include "Serialize.hpp"

#include <td/telegram/td_json_client.h>

#include <mutex>

Client::Client(QObject *parent)
    : QObject(parent)
    , m_clientId(td_create_client_id())  // Ensure client ID is initialized properly
{
    // disable TDLib logging
    td_execute(R"({"@type":"setLogVerbosityLevel", "new_verbosity_level":0})");

    initialize();
}

int Client::clientId() const noexcept
{
    return m_clientId;
}

void Client::send(const QVariantMap &request, std::function<void(const QVariantMap &)> callback)
{
    auto id = m_requestId.fetch_add(1, std::memory_order_relaxed);
    nlohmann::json json = nlohmann::json(request);
    json["@extra"] = id;
    {
        std::unique_lock lock(m_handlerMutex);
        m_handlers[id] = std::move(callback);  // Use move to avoid unnecessary copy
    }

    td_send(m_clientId, json.dump().c_str());
}

QVariantMap Client::execute(const QVariantMap &request)
{
    auto result = td_execute(nlohmann::json(request).dump().c_str());
    return nlohmann::json::parse(result).get<QVariantMap>();  // Ensure correct JSON to QVariantMap conversion
}

void Client::initialize()
{
    td_send(m_clientId, R"({"@type":"getOption", "name":"version"})");

    // A worker thread using std::jthread for automatic joining
    m_worker = std::jthread([this](std::stop_token token) {
        while (!token.stop_requested())
        {
            if (const char *value = td_receive(30.0))
            {
                nlohmann::json json = nlohmann::json::parse(value);
                if (json.contains("@extra"))
                {
                    auto id = json["@extra"].get<std::uint64_t>();
                    std::function<void(const QVariantMap &)> handler;
                    {
                        std::shared_lock lock(m_handlerMutex);
                        if (auto it = m_handlers.find(id); it != m_handlers.end())
                        {
                            handler = std::move(it->second);  // Use move to avoid unnecessary copy
                        }
                    }
                    if (handler)
                    {
                        handler(json.get<QVariantMap>());
                        {
                            std::unique_lock lock(m_handlerMutex);
                            m_handlers.erase(id);
                        }
                    }
                }
                else
                    emit result(json.get<QVariantMap>());
            }
        }
    });
}
