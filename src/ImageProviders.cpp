#include "ImageProviders.hpp"

#include "TdApi.hpp"

#include <QImageReader>

TdImageProvider::TdImageProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage TdImageProvider::requestImage(const QString &id, QSize *size, const QSize &/*requestedSize*/)
{
    QImageReader reader(id);
    QImage result = reader.read();

    if (!result.isNull() && size)
        *size = result.size();

    return result;
}
