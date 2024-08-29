#include "SvgIconItem.hpp"

#include <QDebug>
#include <QFile>
#include <QPainter>

SvgIconItem::SvgIconItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_color(Qt::blue)
    , m_renderer(std::make_unique<QSvgRenderer>(this))
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QUrl SvgIconItem::source() const
{
    return m_source;
}

void SvgIconItem::setSource(const QUrl &source)
{
    if (m_source != source)
    {
        m_source = source;
        loadSvg();
        emit sourceChanged();
    }
}

QColor SvgIconItem::color() const
{
    return m_color;
}

void SvgIconItem::setColor(const QColor &color)
{
    if (m_color != color)
    {
        m_color = color;
        loadSvg();
        emit colorChanged();
    }
}

QString SvgIconItem::urlToLocalFileOrQrc(const QUrl &url)
{
    if (url.scheme().compare(QLatin1String("qrc"), Qt::CaseInsensitive) == 0)
    {
        return url.authority().isEmpty() ? QLatin1Char(':') + url.path() : QString();
    }
    return url.toLocalFile();
}

void SvgIconItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if (m_renderer && m_renderer->isValid())
    {
        m_renderer->render(painter, boundingRect());
    }
}

void SvgIconItem::loadSvg()
{
    const QString filePath = urlToLocalFileOrQrc(m_source);
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open SVG file:" << filePath;
        return;
    }

    const QString svgContent = file.readAll();
    file.close();

    QDomDocument doc;
    if (!doc.setContent(svgContent))
    {
        qDebug() << "Failed to parse SVG content.";
        return;
    }

    updateSvgColors(doc);
    renderSvg(doc);
}

void SvgIconItem::updateSvgColors(QDomDocument &doc)
{
    QDomNodeList paths = doc.elementsByTagName("path");
    for (int i = 0; i < paths.count(); ++i)
    {
        QDomElement pathElement = paths.at(i).toElement();
        pathElement.setAttribute("fill", m_color.name());
    }
}

void SvgIconItem::renderSvg(const QDomDocument &doc)
{
    const QByteArray svgData = doc.toString().toUtf8();
    if (!m_renderer->load(svgData))
    {
        qDebug() << "Failed to load SVG data into renderer.";
    }
    update();
}
