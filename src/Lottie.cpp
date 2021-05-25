#include "Lottie.hpp"

#include <QByteArray>
#include <QPainter>

#include <zstd.h>

namespace {

inline constexpr auto MaxFileSize = 2 * 1024 * 1024;

std::string UnpackGzip(const QByteArray &data)
{
    std::string out;

    const size_t dSize = ZSTD_decompress(out.data(), out.size(), data.constData(), data.size());
    if (ZSTD_isError(dSize) != 0u)
    {
        qDebug() << "Error decoding:" << ZSTD_getErrorName(dSize);
        return {};
    }
    out.resize(dSize);
    return out;
}
}  // namespace

Lottie::Lottie(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QUrl Lottie::source() const
{
    return m_source;
}

void Lottie::setSource(const QUrl &source)
{
    if (m_source == source)
        return;

    m_source = source;
    emit sourceChanged();

    if (isComponentComplete())
        load();
}

void Lottie::paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (image.isNull())
        return;

    p->setRenderHint(QPainter::Antialiasing, true);
    p->setRenderHint(QPainter::SmoothPixmapTransform, true);
    p->drawImage(QRect(QPoint(), size), image);
}

void Lottie::componentComplete()
{
    QDeclarativeItem::componentComplete();

    if (m_source.isValid())
        load();
}

void Lottie::load()
{
    if (animation)
        return;

    const auto string = UnpackGzip(m_source.path().toUtf8());

    animation = rlottie::Animation::loadFromData(string, std::string(), std::string(), false);

    size.setHeight(height());
    size.setWidth(width());
}

void Lottie::renderRequest(int frame)
{
    image = QImage(size, QImage::Format_ARGB32_Premultiplied);

    auto surface = rlottie::Surface(reinterpret_cast<uint32_t *>(image.bits()), image.width(), image.height(), image.bytesPerLine());
    animation->renderSync(frame, surface);
}
