#pragma once

#include <QDeclarativeImageProvider>

class TdImageProvider : public QDeclarativeImageProvider
{
public:
    TdImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;
};
