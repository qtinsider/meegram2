#include "LottieAnimation.hpp"

#include <QDebug>
#include <QFile>
#include <QPainter>

#include <zlib.h>

#include <cstring>
#include <optional>
#include <string>
#include <vector>

namespace {
std::optional<std::string> loadFileContent(const QString &path)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file:" << path;
        return std::nullopt;
    }

    // Check if the file has a .tgs extension
    bool isTgsFile = path.endsWith(".tgs", Qt::CaseInsensitive);
    qDebug() << "File" << path << "is TGS format:" << isTgsFile;

    constexpr size_t checkSize = 512;
    std::vector<char> checkBuffer(checkSize);
    qint64 bytesRead = file.read(checkBuffer.data(), checkSize);

    if (bytesRead <= 0)
    {
        qDebug() << "Failed to read file content for:" << path;
        return std::nullopt;
    }

    // Reset file position to start
    file.seek(0);

    QByteArray byteArray = file.readAll();
    if (byteArray.isEmpty())
    {
        qDebug() << "File is empty or read failed for:" << path;
        return std::nullopt;
    }

    if (isTgsFile || (checkBuffer[0] == 0x1F && checkBuffer[1] == 0x8B))
    {
        qDebug() << "Decompressing file:" << path;

        std::string result;
        result.reserve(byteArray.size());  // Reserve initial space

        constexpr size_t bufferSize = 16 * 1024;
        std::vector<char> buffer(bufferSize);

        z_stream zs;
        std::memset(&zs, 0, sizeof(zs));

        if (inflateInit2(&zs, 15 + 16) != Z_OK)  // 15 + 16 for gzip
        {
            qDebug() << "Failed to initialize zlib for decompression.";
            return std::nullopt;
        }

        zs.next_in = reinterpret_cast<Bytef *>(byteArray.data());
        zs.avail_in = byteArray.size();

        int ret;
        do
        {
            zs.next_out = reinterpret_cast<Bytef *>(buffer.data());
            zs.avail_out = buffer.size();

            ret = inflate(&zs, Z_NO_FLUSH);

            if (ret == Z_STREAM_ERROR || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR)
            {
                inflateEnd(&zs);
                qDebug() << "Decompression error occurred.";
                return std::nullopt;
            }

            size_t have = buffer.size() - zs.avail_out;
            result.append(buffer.data(), have);

        } while (ret != Z_STREAM_END);

        inflateEnd(&zs);
        qDebug() << "Decompression completed successfully.";
        return result;
    }
    else
    {
        qDebug() << "File is not compressed or not a TGS format, returning content as-is.";
        return std::string(byteArray.data(), byteArray.size());
    }
}

}  // namespace

LottieAnimation::LottieAnimation(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);

    m_frameTimer.setSingleShot(false);
    connect(&m_frameTimer, SIGNAL(timeout()), this, SLOT(renderNextFrame()));
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
        loadContent();
}

void LottieAnimation::play()
{
    m_currentFrame = 0;
    m_frameTimer.start();
}

void LottieAnimation::stop()
{
    m_frameTimer.stop();
}

void LottieAnimation::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    if (!m_animation)
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
        m_frameTimer.stop();
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

void LottieAnimation::loadContent()
{
    setStatus(Loading);

    const auto filePath = urlToLocalFileOrQrc(m_source);
    qDebug() << "Loading content from:" << filePath;

    if (const auto result = loadFileContent(filePath); result)
    {
        m_animation = rlottie::Animation::loadFromData(*result, std::string(), std::string(), false);
        if (m_animation)
        {
            initializeAnimation();
            setStatus(Ready);
            update();  // Ensure the item is updated after loading content
            return;
        }
    }

    qDebug() << "Failed to load animation from:" << filePath;
    setStatus(Error);
}

void LottieAnimation::initializeAnimation()
{
    size_t width = 0;
    size_t height = 0;
    m_animation->size(width, height);

    setImplicitWidth(width);
    setImplicitHeight(height);

    m_frameCount = m_animation->totalFrame();
    m_frameRate = m_animation->frameRate();

    m_frameTimer.setInterval(1000 / m_frameRate);
}

QString LottieAnimation::urlToLocalFileOrQrc(const QUrl &url)
{
    if (url.scheme().compare(QLatin1String("qrc"), Qt::CaseInsensitive) == 0)
    {
        return url.authority().isEmpty() ? QLatin1Char(':') + url.path() : QString();
    }

    return url.toLocalFile();
}

void LottieAnimation::componentComplete()
{
    QDeclarativeItem::componentComplete();

    if (m_source.isValid())
        loadContent();
}
