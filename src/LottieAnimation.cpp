#include "LottieAnimation.hpp"

#include "SvgIconItem.hpp"

#include <QFile>
#include <QPainter>
#include <QTimer>

#include <zlib.h>

#include <optional>
#include <string>
#include <vector>

namespace {

std::optional<std::string> loadFileContent(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        return std::nullopt;
    }

    // Determine if the file is compressed or not
    bool isCompressed = false;

    // Read a small part of the file to check its compression status
    constexpr size_t checkSize = 512;
    std::vector<char> checkBuffer(checkSize);
    qint64 bytesRead = file.read(checkBuffer.data(), checkSize);

    if (bytesRead > 0)
    {
        // Simple heuristic: check for gzip magic number (0x1F 0x8B)
        if (checkBuffer[0] == 0x1F && checkBuffer[1] == 0x8B)
        {
            isCompressed = true;
        }
    }

    file.seek(0);  // Reset file position to start

    if (isCompressed)
    {
        gzFile gzf = gzdopen(file.handle(), "r");
        if (!gzf)
        {
            return std::nullopt;
        }

        std::string result;
        result.reserve(64 * 1024);  // Reserve initial space

        constexpr size_t bufferSize = 16 * 1024;
        std::vector<char> buffer(bufferSize);

        while (true)
        {
            int len = gzread(gzf, buffer.data(), buffer.size());

            if (len < 0)
            {
                gzclose(gzf);
                return std::nullopt;
            }

            result.append(buffer.data(), static_cast<std::size_t>(len));

            if (static_cast<std::size_t>(len) < buffer.size())
            {
                break;
            }
        }

        gzclose(gzf);
        return result;
    }
    else
    {
        QByteArray byteArray = file.readAll();
        if (byteArray.isEmpty())
        {
            return std::nullopt;
        }
        return std::string(byteArray.data(), byteArray.size());
    }
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
    setStatus(Loading);

    const auto filePath = SvgIconItem::urlToLocalFileOrQrc(m_source);
    const auto result = loadFileContent(filePath);

    if (!result)
    {
        setStatus(Error);
        return;
    }

    m_animation = rlottie::Animation::loadFromData(*result, std::string(), std::string(), false);

    if (!m_animation)
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
    QDeclarativeItem::componentComplete();

    if (m_source.isValid())
        load();
}
