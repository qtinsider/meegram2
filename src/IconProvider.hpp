#pragma once

#include <QDeclarativeImageProvider>
#include <QDomDocument>
#include <QSvgRenderer>

#include <memory>

class IconProvider : public QDeclarativeImageProvider
{
public:
    IconProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize) override;

private:
    bool loadSvg(const QString &filePath, const QColor &color);
    void updateSvgColors(QDomDocument &doc, const QColor &color);

    std::unique_ptr<QSvgRenderer> m_renderer;
};
