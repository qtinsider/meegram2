#pragma once

#include <QDeclarativeImageProvider>


class ChatPhotoProvider : public QDeclarativeImageProvider
{
public:
    ChatPhotoProvider()
        : QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
    {
    }

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};
