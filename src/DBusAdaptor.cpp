#include "DBusAdaptor.hpp"

#include <QApplication>
#include <QDBusConnection>
#include <QDeclarativeView>

DBusAdaptor::DBusAdaptor(QApplication *parent, QDeclarativeView *view)
    : QDBusAbstractAdaptor(parent)
    , m_view(view)
{
    QDBusConnection::sessionBus().registerService("com.meegram");
    QDBusConnection::sessionBus().registerObject("/", parent);
}

DBusAdaptor::~DBusAdaptor()
{
}

void DBusAdaptor::openChat(const QString &chatId)
{
}

