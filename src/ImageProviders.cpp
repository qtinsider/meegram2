#include "ImageProviders.hpp"

#include "TdApi.hpp"

#include <QImageReader>

ChatPhotoProvider::ChatPhotoProvider()
    : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage ChatPhotoProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QString path(":/images/avatar-placeholder.png");

    auto chat = TdApi::getInstance().chatStore->get(id.toLongLong());

    auto chatPhoto = chat.value("photo").toMap();
    if (chatPhoto.value("small").toMap().value("local").toMap().value("is_downloading_completed").toBool())
    {
        path = chatPhoto.value("small").toMap().value("local").toMap().value("path").toString();
    }

    QImageReader reader(path);
    QImage result = reader.read();

    if (!result.isNull() && size)
        *size = result.size();

    return result;
}
