#pragma once

#include <QScriptValue>
#include <QTimer>

class CanvasTimer : public QTimer
{
    Q_OBJECT
public:
    CanvasTimer(QObject *parent, const QScriptValue &data);

    static void createTimer(QObject *parent, const QScriptValue &value, long timeout, bool singleShot);
    static void removeTimer(CanvasTimer *timer);
    static void removeTimer(const QScriptValue &);

public slots:
    void handleTimeout();
    bool equals(const QScriptValue &value);

private:
    QScriptValue m_value;
};
