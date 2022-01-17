#pragma once

#include <QDBusAbstractAdaptor>
#include <QStringList>

class QApplication;
class QDeclarativeView;
class DBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.meegram")

public:
    DBusAdaptor(QApplication *parent, QDeclarativeView *view);
    ~DBusAdaptor();

public slots:
    Q_NOREPLY void openChat(const QStringList &ids);
    Q_NOREPLY void activateWindow(const QStringList &dummy = QStringList()); // parameter for .desktop activation

private:
    QDeclarativeView *m_view;
};
