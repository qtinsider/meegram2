#include "SignalConnector.hpp"

SignalConnector::SignalConnector(QObject *parent, const char *signal, std::function<void()> f, Qt::ConnectionType type)
    : QObject(parent)
    , m_signal(QString::fromLatin1(signal))
    , m_result(QObject::connect(parent, signal, this, SLOT(trigger()), type))
    , m_lambda(std::move(f))
{
}

void SignalConnector::trigger()
{
    if (m_lambda)
    {
        m_lambda();
    }
}

bool SignalConnector::isConnected() const noexcept
{
    return m_result;
}

bool SignalConnector::disconnect()
{
    if (m_result)
    {
        m_result = QObject::disconnect(parent(), m_signal.toLatin1().constData(), this, SLOT(trigger()));
    }
    return m_result;
}

SignalConnector *connecl(QObject *sender, const char *signal, std::function<void()> lambda, Qt::ConnectionType type)
{
    return new SignalConnector(sender, signal, std::move(lambda), type);
}
