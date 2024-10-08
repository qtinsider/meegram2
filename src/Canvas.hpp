#pragma once

#include "CanvasTimer.hpp"

#include <QDeclarativeItem>

class Context2D;
class CanvasImage;

class Canvas : public QDeclarativeItem
{
    Q_OBJECT

    Q_ENUMS(FillMode)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(int canvasWidth READ canvasWidth WRITE setCanvasWidth NOTIFY canvasWidthChanged)
    Q_PROPERTY(int canvasHeight READ canvasHeight WRITE setCanvasHeight NOTIFY canvasHeightChanged)
    Q_PROPERTY(FillMode fillMode READ fillMode WRITE setFillMode NOTIFY fillModeChanged)

public:
    Canvas(QDeclarativeItem *parent = 0);

    enum FillMode { Stretch, PreserveAspectFit, PreserveAspectCrop, Tile, TileVertically, TileHorizontally };

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

    int canvasWidth();
    void setCanvasWidth(int newWidth);

    int canvasHeight();
    void setCanvasHeight(int canvasHeight);

    void componentComplete();

public slots:
    Context2D *getContext(const QString & = QString("2d"));
    void requestPaint();

    FillMode fillMode() const;
    void setFillMode(FillMode);

    QColor color();
    void setColor(const QColor &);

    // Save current canvas to disk
    bool save(const QString &filename) const;

    // Timers
    void setInterval(const QScriptValue &handler, long timeout);
    void setTimeout(const QScriptValue &handler, long timeout);
    void clearInterval(const QScriptValue &handler);
    void clearTimeout(const QScriptValue &handler);

Q_SIGNALS:
    void fillModeChanged();
    void canvasWidthChanged();
    void canvasHeightChanged();
    void colorChanged();
    void init();
    void paint();

private:
    // Return canvas contents as a drawable image
    CanvasImage *toImage() const;
    Context2D *m_context;
    int m_canvasWidth;
    int m_canvasHeight;
    FillMode m_fillMode;
    QColor m_color;

    friend class Context2D;
};
