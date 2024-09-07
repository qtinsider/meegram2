#include "ChatPhotoProvider.hpp"

QImage ChatPhotoProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage source(id);

    if (size)
        *size = QSize(source.width(), source.height());

    return source;
}
