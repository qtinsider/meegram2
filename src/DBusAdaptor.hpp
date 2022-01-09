#pragma once

#include <QDBusAbstractAdaptor>

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
    void openChat(const QString &chatId);

private:
    QDeclarativeView *m_view;
};
