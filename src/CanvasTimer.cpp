#include "CanvasTimer.hpp"

#include <QList>

Q_GLOBAL_STATIC(QList<CanvasTimer *>, activeTimers)

CanvasTimer::CanvasTimer(QObject *parent, const QScriptValue &data)
    : QTimer(parent)
    , m_value(data)
{
}

void CanvasTimer::handleTimeout()
{
    Q_ASSERT(m_value.isFunction());
    m_value.call();
    if (isSingleShot())
    {
        removeTimer(this);
    }
}

bool CanvasTimer::equals(const QScriptValue &value)
{
    return m_value.equals(value);
}

void CanvasTimer::createTimer(QObject *parent, const QScriptValue &value, long timeout, bool singleShot)
{
    CanvasTimer *timer = new CanvasTimer(parent, value);
    timer->setInterval(timeout);
    timer->setSingleShot(singleShot);
    connect(timer, SIGNAL(timeout()), timer, SLOT(handleTimeout()));
    activeTimers()->append(timer);
    timer->start();
}

void CanvasTimer::removeTimer(CanvasTimer *timer)
{
    activeTimers()->removeAll(timer);
    timer->deleteLater();
}

void CanvasTimer::removeTimer(const QScriptValue &val)
{
    if (!val.isFunction())
        return;

    for (int i = 0; i < activeTimers()->count(); ++i)
    {
        CanvasTimer *timer = activeTimers()->at(i);
        if (timer->equals(val))
        {
            removeTimer(timer);
            return;
        }
    }
}
