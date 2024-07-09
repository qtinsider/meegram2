#include "LottieAnimation.hpp"

#include <QFile>
#include <QPainter>
#include <QTimer>

#include <zlib.h>

#include <string>
#include <vector>

namespace {

bool loadFileContent(const QString &path, std::string &out)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
        return false;

    // Use a larger buffer size for potentially better performance
    constexpr size_t bufferSize = 16 * 1024;
    std::vector<char> buffer(bufferSize);

    gzFile gzf = gzdopen(file.handle(), "r");
    if (!gzf)
        return false;

    std::string result;
    while (true)
    {
        int len = gzread(gzf, buffer.data(), buffer.size());

        if (len < 0)
            return false;

        result.append(buffer.data(), len);

        if (static_cast<size_t>(len) < buffer.size())
            break;
    }

    out = std::move(result);

    return true;
}

}  // namespace

LottieAnimation::LottieAnimation(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
    , m_frameTimer(new QTimer(this))
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    m_frameTimer->setSingleShot(false);
    connect(m_frameTimer, SIGNAL(timeout()), SLOT(renderNextFrame()));
}

LottieAnimation::~LottieAnimation()
{
    delete m_frameTimer;
}

LottieAnimation::Status LottieAnimation::status() const
{
    return m_status;
}

QUrl LottieAnimation::source() const
{
    return m_source;
}

void LottieAnimation::setSource(const QUrl &source)
{
    if (source == m_source)
        return;

    m_source = source;
    emit sourceChanged();

    if (isComponentComplete())
        load();
}

void LottieAnimation::play()
{
    m_currentFrame = 0;
    m_frameTimer->start();
}

void LottieAnimation::stop()
{
    m_frameTimer->stop();
}

void LottieAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (m_animation == nullptr)
        return;

    QImage image(QSize(width(), height()), QImage::Format_ARGB32_Premultiplied);

    auto surface = rlottie::Surface(reinterpret_cast<uint32_t *>(image.bits()), image.width(), image.height(), image.bytesPerLine());
    m_animation->renderSync(m_currentFrame, std::move(surface));

    painter->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter->drawImage(QPoint(0, 0), image);
}

void LottieAnimation::renderNextFrame()
{
    if (++m_currentFrame < m_frameCount)
    {
        update();
    }
    else if (m_currentFrame == m_frameCount)
    {
        m_frameTimer->stop();
        emit finished();
    }
}

void LottieAnimation::setStatus(Status status)
{
    if (status == m_status)
        return;

    m_status = status;
    emit statusChanged();
}

void LottieAnimation::load()
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

    size_t width = 0;
    size_t height = 0;
    m_animation->size(width, height);

    setImplicitWidth(width);
    setImplicitHeight(height);

    m_frameCount = m_animation->totalFrame();
    m_frameRate = m_animation->frameRate();

    m_frameTimer->setInterval(1000 / m_frameRate);

    setStatus(Ready);
}

void LottieAnimation::componentComplete()
{
    if (m_source.isValid())
        load();

    QDeclarativeItem::componentComplete();
}
