#pragma once

#include <QObject>
#include <QString>
#include <functional>

class SignalConnector : public QObject
{
    Q_OBJECT

public:
    SignalConnector(QObject *parent, const char *signal, std::function<void()> f, Qt::ConnectionType type = Qt::AutoConnection);

    bool disconnect();
    bool isConnected() const noexcept;

public slots:
    void trigger();

private:
    QString m_signal;
    bool m_result;
    std::function<void()> m_lambda;
};

SignalConnector *connecl(QObject *sender, const char *signal, std::function<void()> lambda, Qt::ConnectionType type = Qt::AutoConnection);
