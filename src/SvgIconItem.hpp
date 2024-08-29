#pragma once

#include <QDeclarativeItem>
#include <QDomDocument>
#include <QSvgRenderer>

#include <memory>

class SvgIconItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    explicit SvgIconItem(QDeclarativeItem *parent = nullptr);

    QUrl source() const;
    void setSource(const QUrl &source);

    QColor color() const;
    void setColor(const QColor &color);

    static QString urlToLocalFileOrQrc(const QUrl &url);

signals:
    void colorChanged();
    void sourceChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void loadSvg();

    void updateSvgColors(QDomDocument &doc);
    void renderSvg(const QDomDocument &doc);

    QColor m_color;
    QUrl m_source;
    std::unique_ptr<QSvgRenderer> m_renderer;
};
