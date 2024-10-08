#include "Canvas.hpp"
#include "CanvasTimer.hpp"
#include "Context2D.hpp"

#include <QPainter>

Canvas::Canvas(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_context(new Context2D(this))
    , m_canvasWidth(0)
    , m_canvasHeight(0)
    , m_fillMode(Canvas::Stretch)
    , m_color(Qt::white)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
    setCacheMode(QGraphicsItem::DeviceCoordinateCache);
}

void Canvas::componentComplete()
{
    if (m_canvasWidth == 0 && m_canvasHeight == 0)
        m_context->setSize(width(), height());
    else
        m_context->setSize(m_canvasWidth, m_canvasHeight);

    connect(m_context, SIGNAL(changed()), this, SLOT(requestPaint()));
    emit init();
    QDeclarativeItem::componentComplete();
}

void Canvas::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    m_context->setInPaint(true);
    emit paint();

    //    bool oldAA = painter->testRenderHint(QPainter::Antialiasing);
    //    bool oldSmooth = painter->testRenderHint(QPainter::SmoothPixmapTransform);
    //    if (smooth())
    //        painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform, smooth());

    if (m_context->pixmap().isNull())
    {
        painter->fillRect(0, 0, width(), height(), m_color);
    }
    else if (width() != m_context->pixmap().width() || height() != m_context->pixmap().height())
    {
        if (m_fillMode >= Tile)
        {
            if (m_fillMode == Tile)
            {
                painter->drawTiledPixmap(QRectF(0, 0, width(), height()), m_context->pixmap());
            }
            else
            {
                qreal widthScale = width() / qreal(m_context->pixmap().width());
                qreal heightScale = height() / qreal(m_context->pixmap().height());

                QTransform scale;
                if (m_fillMode == TileVertically)
                {
                    scale.scale(widthScale, 1.0);
                    QTransform old = painter->transform();
                    painter->setWorldTransform(scale * old);
                    painter->drawTiledPixmap(QRectF(0, 0, m_context->pixmap().width(), height()), m_context->pixmap());
                    painter->setWorldTransform(old);
                }
                else
                {
                    scale.scale(1.0, heightScale);
                    QTransform old = painter->transform();
                    painter->setWorldTransform(scale * old);
                    painter->drawTiledPixmap(QRectF(0, 0, width(), m_context->pixmap().height()), m_context->pixmap());
                    painter->setWorldTransform(old);
                }
            }
        }
        else
        {
            qreal widthScale = width() / qreal(m_context->pixmap().width());
            qreal heightScale = height() / qreal(m_context->pixmap().height());

            QTransform scale;

            if (m_fillMode == PreserveAspectFit)
            {
                if (widthScale <= heightScale)
                {
                    heightScale = widthScale;
                    scale.translate(0, (height() - heightScale * m_context->pixmap().height()) / 2);
                }
                else if (heightScale < widthScale)
                {
                    widthScale = heightScale;
                    scale.translate((width() - widthScale * m_context->pixmap().width()) / 2, 0);
                }
            }
            else if (m_fillMode == PreserveAspectCrop)
            {
                if (widthScale < heightScale)
                {
                    widthScale = heightScale;
                    scale.translate((width() - widthScale * m_context->pixmap().width()) / 2, 0);
                }
                else if (heightScale < widthScale)
                {
                    heightScale = widthScale;
                    scale.translate(0, (height() - heightScale * m_context->pixmap().height()) / 2);
                }
            }
            if (clip())
            {
                painter->save();
                painter->setClipRect(boundingRect(), Qt::IntersectClip);
            }
            scale.scale(widthScale, heightScale);
            QTransform old = painter->transform();
            painter->setWorldTransform(scale * old);
            painter->drawPixmap(0, 0, m_context->pixmap());
            painter->setWorldTransform(old);
            if (clip())
            {
                painter->restore();
            }
        }
    }
    else
    {
        painter->drawPixmap(0, 0, m_context->pixmap());
    }

    //    if (smooth()) {
    //        painter->setRenderHint(QPainter::Antialiasing, oldAA);
    //        painter->setRenderHint(QPainter::SmoothPixmapTransform, oldSmooth);
    //    }
    m_context->setInPaint(false);
}

Context2D *Canvas::getContext(const QString &contextId)
{
    if (contextId == QLatin1String("2d"))
        return m_context;
    qDebug("Canvas:requesting unsupported context");
    return 0;
}

void Canvas::requestPaint()
{
    update();
}

void Canvas::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (m_canvasWidth == 0 && m_canvasHeight == 0 && newGeometry.width() > 0 && newGeometry.height() > 0)
    {
        m_context->setSize(width(), height());
    }
    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

int Canvas::canvasWidth()
{
    return m_canvasWidth;
}

void Canvas::setCanvasWidth(int newWidth)
{
    if (m_canvasWidth != newWidth)
    {
        m_canvasWidth = newWidth;
        m_context->setSize(m_canvasWidth, m_canvasHeight);
        emit canvasWidthChanged();
    }
}

int Canvas::canvasHeight()
{
    return m_canvasHeight;
}

void Canvas::setCanvasHeight(int newHeight)
{
    if (m_canvasHeight != newHeight)
    {
        m_canvasHeight = newHeight;
        m_context->setSize(m_canvasWidth, m_canvasHeight);
        emit canvasHeightChanged();
    }
}

void Canvas::setFillMode(FillMode mode)
{
    if (m_fillMode == mode)
        return;

    m_fillMode = mode;
    update();
    emit fillModeChanged();
}

QColor Canvas::color()
{
    return m_color;
}

void Canvas::setColor(const QColor &color)
{
    if (m_color != color)
    {
        m_color = color;
        colorChanged();
    }
}

Canvas::FillMode Canvas::fillMode() const
{
    return m_fillMode;
}

bool Canvas::save(const QString &filename) const
{
    return m_context->pixmap().save(filename);
}

CanvasImage *Canvas::toImage() const
{
    return new CanvasImage(m_context->pixmap());
}

void Canvas::setTimeout(const QScriptValue &handler, long timeout)
{
    if (handler.isFunction())
        CanvasTimer::createTimer(this, handler, timeout, true);
}

void Canvas::setInterval(const QScriptValue &handler, long interval)
{
    if (handler.isFunction())
        CanvasTimer::createTimer(this, handler, interval, false);
}

void Canvas::clearTimeout(const QScriptValue &handler)
{
    CanvasTimer::removeTimer(handler);
}

void Canvas::clearInterval(const QScriptValue &handler)
{
    CanvasTimer::removeTimer(handler);
}
