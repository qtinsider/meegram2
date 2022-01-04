#pragma once

#include <QDeclarativeImageProvider>

class ChatPhotoProvider : public QDeclarativeImageProvider
{
public:
    ChatPhotoProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};
