#include "DBusAdaptor.hpp"

#include <QApplication>
#include <QDBusConnection>
#include <QDeclarativeItem>
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

void DBusAdaptor::openChat(const QStringList &ids)
{
    m_view->activateWindow();

    if (ids.count() == 1)
    {
        QMetaObject::invokeMethod(m_view->rootObject(), "openChat", Q_ARG(QVariant, ids.at(0)));
    }
    else
    {
        activateWindow(ids);
    }
}

void DBusAdaptor::activateWindow(const QStringList &dummy)
{
    Q_UNUSED(dummy)

    m_view->activateWindow();

    QMetaObject::invokeMethod(m_view->rootObject(), "activate");
}
