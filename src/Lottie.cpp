#include "Lottie.hpp"

#include <zlib.h>

#include <QFile>
#include <QPainter>
#include <QTimer>

#include <sstream>

namespace {

bool loadFileContent(const QString &path, std::string &out)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    // Handle gzipped files as well (also known as TGS format, for Telegram stickers)
    char buf[BUFSIZ];
    std::stringstream ss;
    gzFile gzf = gzdopen(file.handle(), "r");

    while (true)
    {
        int len = gzread(gzf, buf, sizeof(buf));

        if (len < 0)
            return false;

        ss.write(buf, len);

        if (static_cast<size_t>(len) < sizeof(buf))
            break;
    }

    out = ss.str();

    return true;
}

}  // namespace

Lottie::Lottie(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_frameTimer(new QTimer(this))
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    m_frameTimer->setSingleShot(false);
    connect(m_frameTimer, SIGNAL(timeout()), SLOT(renderNextFrame()));
}

Lottie::~Lottie()
{
}

Lottie::Status Lottie::status() const
{
    return m_status;
}

QUrl Lottie::source() const
{
    return m_source;
}

void Lottie::setSource(const QUrl &source)
{
    if (source == m_source)
        return;

    m_source = source;
    emit sourceChanged();

    if (isComponentComplete())
        load();
}

void Lottie::play()
{
    m_currentFrame = 0;
    m_frameTimer->start();
}

void Lottie::stop()
{
    m_frameTimer->stop();
}

void Lottie::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (m_animation == nullptr)
        return;

    QImage image(QSize(width(), height()), QImage::Format_ARGB32_Premultiplied);

    auto surface = rlottie::Surface(reinterpret_cast<uint32_t *>(image.bits()), image.width(), image.height(), image.bytesPerLine());
    m_animation->renderSync(m_currentFrame, std::move(surface));

    m_currentFrame += 1;

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter->drawImage(boundingRect(), image, QRect(0, 0, width(), height()));
}

void Lottie::renderNextFrame()
{
    if (m_currentFrame >= 0 && m_currentFrame < m_frameCount)
    {
        update();
    }
    else if (m_currentFrame == m_frameCount)
    {
        m_frameTimer->stop();

        m_currentFrame = 0;
        update();
        emit finished();
    }
}

void Lottie::setStatus(Status status)
{
    if (status == m_status)
        return;

    m_status = status;
    emit statusChanged();
}

void Lottie::load()
{
    std::string result;

    setStatus(Loading);

    if (!loadFileContent(m_source.toLocalFile(), result))
    {
        setStatus(Error);
        return;
    }

    m_animation = rlottie::Animation::loadFromData(result, std::string(), std::string(), false);

    if (m_animation == nullptr)
    {
        setStatus(Error);
        return;
    }

    auto width = size_t(0);
    auto height = size_t(0);
    m_animation->size(width, height);

    setImplicitWidth(width);
    setImplicitHeight(height);

    m_frameCount = m_animation->totalFrame();
    m_frameRate = m_animation->frameRate();

    m_frameTimer->setInterval(1000 / m_frameRate);

    setStatus(Ready);
}

void Lottie::componentComplete()
{
    QDeclarativeItem::componentComplete();

    if (m_source.isValid())
        load();
}
