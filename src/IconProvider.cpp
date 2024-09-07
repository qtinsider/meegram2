#include "IconProvider.hpp"

#include <QDebug>
#include <QFile>
#include <QPainter>

IconProvider::IconProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    , m_renderer(std::make_unique<QSvgRenderer>())
{
}

QImage IconProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    const auto args = id.split('?');
    const QString &filePath = args[0];
    QColor color = Qt::black;  // Default color

    if (args.size() > 1)
    {
        const QString &colorName = args[1];
        QColor parsedColor(colorName);
        if (parsedColor.isValid())
        {
            color = parsedColor;
        }
    }

    // Determine the image size
    QSize imageSize = requestedSize.isValid() ? requestedSize : QSize(40, 40);

    // Load and render the SVG
    if (loadSvg(filePath, color))
    {
        QImage image(imageSize, QImage::Format_ARGB32);
        image.fill(Qt::transparent);

        QPainter painter(&image);
        m_renderer->render(&painter);

        if (size)
        {
            *size = imageSize;
        }

        return image;
    }

    // If loading fails, return an empty image and set size to empty
    if (size)
    {
        *size = QSize();
    }

    return QImage();
}

bool IconProvider::loadSvg(const QString &filePath, const QColor &color)
{
    QString resourcePath = ":/" + filePath;

    QFile file(resourcePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open SVG file:" << resourcePath;
        return false;
    }

    QDomDocument doc;
    if (!doc.setContent(file.readAll()))
    {
        qDebug() << "Failed to parse SVG content.";
        return false;
    }

    updateSvgColors(doc, color);
    return m_renderer->load(doc.toByteArray());
}

void IconProvider::updateSvgColors(QDomDocument &doc, const QColor &color)
{
    QDomNodeList paths = doc.elementsByTagName("path");
    for (int i = 0; i < paths.count(); ++i)
    {
        QDomElement pathElement = paths.at(i).toElement();
        pathElement.setAttribute("fill", color.name());
    }
}
