#include "QrCodeItem.hpp"

#include <qrcodegen.hpp>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include <vector>

namespace Qr {

// Note: The following code is adapted from the QR code generation implementation found in
// https://github.com/desktop-app/lib_qr/blob/master/qr/qr_generate.cpp
// This includes QR code encoding, image generation functions, and related utilities.

enum class Redundancy {
    Low,
    Medium,
    Quartile,
    High,

    Default = Medium
};

struct Data
{
    int size = 0;
    Redundancy redundancy = Redundancy::Default;
    std::vector<bool> values;  // size x size
};

using namespace qrcodegen;

namespace {

    [[nodiscard]] int ReplaceElements(const Data &data)
    {
        const auto elements = [&] {
            switch (data.redundancy)
            {
                case Redundancy::Low:
                    return data.size / 5;
                case Redundancy::Medium:
                    return data.size / 4;
                case Redundancy::Quartile:
                    return data.size / 3;
                case Redundancy::High:
                    return data.size / 2;
                default:
                    throw std::invalid_argument("Invalid Redundancy value");
            }
        }();

        const auto close = (data.redundancy != Redundancy::Quartile);
        const auto shift = (data.size - elements) % 2;
        return elements + (close ? -1 : 1) * shift;
    }

    [[nodiscard]] QrCode::Ecc RedundancyToEcc(Redundancy redundancy)
    {
        switch (redundancy)
        {
            case Redundancy::Low:
                return QrCode::Ecc::LOW;
            case Redundancy::Medium:
                return QrCode::Ecc::MEDIUM;
            case Redundancy::Quartile:
                return QrCode::Ecc::QUARTILE;
            case Redundancy::High:
                return QrCode::Ecc::HIGH;
            default:
                throw std::invalid_argument("Invalid Redundancy value");
        }
    }

}  // namespace

[[nodiscard]] Data Encode(const QString &text, Redundancy redundancy)
{
    const std::string utf8 = text.toStdString();
    const auto qr = QrCode::encodeText(utf8.c_str(), RedundancyToEcc(redundancy));
    const int size = qr.getSize();

    Data result{.size = size, .redundancy = redundancy, .values = std::vector<bool>(size * size)};

    for (int row = 0; row < size; ++row)
    {
        for (int column = 0; column < size; ++column)
        {
            result.values[row * size + column] = qr.getModule(row, column);
        }
    }

    return result;
}

void PrepareForRound(QPainter &p)
{
    p.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);
    p.setPen(Qt::NoPen);
}

[[nodiscard]] QImage GenerateSingle(int size, QColor bg, QColor color)
{
    QImage result(size, size, QImage::Format_ARGB32_Premultiplied);
    result.fill(bg.rgba());

    QPainter p(&result);
    PrepareForRound(p);  // Prepare painter settings
    p.setCompositionMode(QPainter::CompositionMode_Source);

    const int radius = size / 2;

    p.setBrush(color);
    p.drawRoundedRect(0, 0, size, size, radius, radius);

    return result;
}

[[nodiscard]] int ReplaceSize(const Data &data, int pixel)
{
    return ReplaceElements(data) * pixel;
}

[[nodiscard]] QImage Generate(const Data &data, int pixel, QColor fg)
{
    const auto bg = Qt::transparent;
    const auto replaceElements = ReplaceElements(data);
    const auto replaceFrom = (data.size - replaceElements) / 2;
    const auto replaceTill = (data.size - replaceFrom);
    const auto black = GenerateSingle(pixel, bg, fg);
    const auto white = GenerateSingle(pixel, fg, bg);
    const auto value = [&](int row, int column) {
        return (row >= 0) && (row < data.size) && (column >= 0) && (column < data.size) &&
               (row < replaceFrom || row >= replaceTill || column < replaceFrom || column >= replaceTill) && data.values[row * data.size + column];
    };
    const auto blackFull = [&](int row, int column) {
        return (value(row - 1, column) && value(row + 1, column)) || (value(row, column - 1) && value(row, column + 1));
    };
    const auto whiteCorner = [&](int row, int column, int dx, int dy) {
        return !value(row + dy, column) || !value(row, column + dx) || !value(row + dy, column + dx);
    };
    const auto whiteFull = [&](int row, int column) {
        return whiteCorner(row, column, -1, -1) && whiteCorner(row, column, 1, -1) && whiteCorner(row, column, 1, 1) && whiteCorner(row, column, -1, 1);
    };
    auto result = QImage(data.size * pixel, data.size * pixel, QImage::Format_ARGB32_Premultiplied);
    result.fill(bg);
    {
        auto p = QPainter(&result);
        p.setCompositionMode(QPainter::CompositionMode_Source);
        const auto skip = pixel - pixel / 2;
        const auto brect = [&](int x, int y, int width, int height) { p.fillRect(x, y, width, height, fg); };
        const auto wrect = [&](int x, int y, int width, int height) { p.fillRect(x, y, width, height, bg); };
        const auto large = [&](int x, int y) {
            p.setBrush(fg);
            p.drawRoundedRect(QRect{x, y, pixel * 7, pixel * 7}, pixel * 2., pixel * 2.);
            p.setBrush(bg);
            p.drawRoundedRect(QRect{x + pixel, y + pixel, pixel * 5, pixel * 5}, pixel * 1.5, pixel * 1.5);
            p.setBrush(fg);
            p.drawRoundedRect(QRect{x + pixel * 2, y + pixel * 2, pixel * 3, pixel * 3}, pixel, pixel);
        };
        for (auto row = 0; row != data.size; ++row)
        {
            for (auto column = 0; column != data.size; ++column)
            {
                if ((row < 7 && (column < 7 || column >= data.size - 7)) || (column < 7 && (row < 7 || row >= data.size - 7)))
                {
                    continue;
                }
                const auto x = column * pixel;
                const auto y = row * pixel;
                if (value(row, column))
                {
                    if (blackFull(row, column))
                    {
                        brect(x, y, pixel, pixel);
                    }
                    else
                    {
                        p.drawImage(x, y, black);
                        if (value(row - 1, column))
                        {
                            brect(x, y, pixel, pixel / 2);
                        }
                        else if (value(row + 1, column))
                        {
                            brect(x, y + skip, pixel, pixel / 2);
                        }
                        if (value(row, column - 1))
                        {
                            brect(x, y, pixel / 2, pixel);
                        }
                        else if (value(row, column + 1))
                        {
                            brect(x + skip, y, pixel / 2, pixel);
                        }
                    }
                }
                else if (whiteFull(row, column))
                {
                    wrect(x, y, pixel, pixel);
                }
                else
                {
                    p.drawImage(x, y, white);
                    if (whiteCorner(row, column, -1, -1) && whiteCorner(row, column, 1, -1))
                    {
                        wrect(x, y, pixel, pixel / 2);
                    }
                    else if (whiteCorner(row, column, -1, 1) && whiteCorner(row, column, 1, 1))
                    {
                        wrect(x, y + skip, pixel, pixel / 2);
                    }
                    if (whiteCorner(row, column, -1, -1) && whiteCorner(row, column, -1, 1))
                    {
                        wrect(x, y, pixel / 2, pixel);
                    }
                    else if (whiteCorner(row, column, 1, -1) && whiteCorner(row, column, 1, 1))
                    {
                        wrect(x + skip, y, pixel / 2, pixel);
                    }
                    if (whiteCorner(row, column, -1, -1))
                    {
                        wrect(x, y, pixel / 2, pixel / 2);
                    }
                    if (whiteCorner(row, column, 1, -1))
                    {
                        wrect(x + skip, y, pixel / 2, pixel / 2);
                    }
                    if (whiteCorner(row, column, 1, 1))
                    {
                        wrect(x + skip, y + skip, pixel / 2, pixel / 2);
                    }
                    if (whiteCorner(row, column, -1, 1))
                    {
                        wrect(x, y + skip, pixel / 2, pixel / 2);
                    }
                }
            }
        }

        PrepareForRound(p);
        large(0, 0);
        large((data.size - 7) * pixel, 0);
        large(0, (data.size - 7) * pixel);
    }
    return result;
}

QImage ReplaceCenter(QImage qr, const QImage &center)
{
    QImage copy = qr.copy();  // Make a copy of the original QR image
    {
        QPainter p(&copy);
        const auto x = (copy.width() - center.width()) / 2;
        const auto y = (copy.height() - center.height()) / 2;
        p.drawImage(x, y, center);
    }
    return copy;
}

}  // namespace Qr

QrCodeItem::QrCodeItem(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QString QrCodeItem::text() const
{
    return m_text;
}

void QrCodeItem::setText(const QString &text)
{
    if (m_text != text)
    {
        m_text = text;
        emit textChanged();
        updateQrCode();
    }
}

int QrCodeItem::size() const
{
    return m_size;
}

void QrCodeItem::setSize(int size)
{
    if (m_size != size)
    {
        m_size = size;
        emit sizeChanged();
        updateQrCode();
    }
}

QColor QrCodeItem::foreground() const
{
    return m_foreground;
}

void QrCodeItem::setForeground(const QColor &color)
{
    if (m_foreground != color)
    {
        m_foreground = color;
        emit foregroundChanged();
        updateQrCode();
    }
}

QColor QrCodeItem::background() const
{
    return m_background;
}

void QrCodeItem::setBackground(const QColor &color)
{
    if (m_background != color)
    {
        m_background = color;
        emit backgroundChanged();
        updateQrCode();
    }
}

void QrCodeItem::updateQrCode()
{
    if (m_text.isEmpty())
    {
        m_qrCodeImage = QImage();
        update();

        return;
    }

    // Generate the QR code data
    const auto qrData = Qr::Encode(m_text, Qr::Redundancy::Default);

    // Determine the pixel size for the QR code image
    const int pixelSize = m_size / qrData.size;

    // Generate the QR code image
    m_qrCodeImage = Qr::Generate(qrData, pixelSize, m_foreground);

    // Optionally, you can replace the center of the QR code with another image if needed
    // Uncomment and replace 'centerImage' with the desired image
    // QImage centerImage = ...;
    // m_qrCodeImage = Qr::ReplaceCenter(m_qrCodeImage, centerImage);

    update();
}

void QrCodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(widget);

    if (m_qrCodeImage.isNull())
    {
        return;
    }

    const QRectF boundingRect = option->rect.adjusted(0, 0, -1, -1);
    const QSizeF imageSize = m_qrCodeImage.size();
    const QSizeF boundingSize = boundingRect.size();

    // Calculate scale factor to preserve aspect ratio
    const qreal scaleFactor = qMin(boundingSize.width() / imageSize.width(), boundingSize.height() / imageSize.height());
    const QSizeF targetSize = imageSize * scaleFactor;
    const QRectF targetRect = QRectF(boundingRect.topLeft(), targetSize)
                                  .translated((boundingRect.width() - targetSize.width()) / 2.0, (boundingRect.height() - targetSize.height()) / 2.0);

    painter->drawImage(targetRect, m_qrCodeImage);
}
