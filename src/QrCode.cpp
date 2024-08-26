#include "QrCode.hpp"

#include <qrcodegen.hpp>

#include <QPainter>
#include <QStyleOptionGraphicsItem>

QrCode::QrCode(QDeclarativeItem *parent)
    : QDeclarativeItem(parent)
{
    setFlag(QGraphicsItem::ItemHasNoContents, false);
}

QString QrCode::text() const
{
    return m_text;
}

void QrCode::setText(const QString &text)
{
    if (m_text != text)
    {
        m_text = text;
        emit textChanged();
        updateQrCode();
        update();
    }
}

int QrCode::size() const
{
    return m_size;
}

void QrCode::setSize(int size)
{
    if (m_size != size)
    {
        m_size = size;
        emit sizeChanged();
        updateQrCode();
        update();
    }
}

QColor QrCode::foreground() const
{
    return m_foreground;
}

void QrCode::setForeground(const QColor &color)
{
    if (m_foreground != color)
    {
        m_foreground = color;
        emit foregroundChanged();
        updateQrCode();
        update();
    }
}

QColor QrCode::background() const
{
    return m_background;
}

void QrCode::setBackground(const QColor &color)
{
    if (m_background != color)
    {
        m_background = color;
        emit backgroundChanged();
        updateQrCode();
        update();
    }
}

void QrCode::updateQrCode()
{
    if (m_text.isEmpty())
    {
        m_qrCodeImage = QImage();
        return;
    }

    // Update the QR code image based on the current text and settings.
    // 1. Encode the text into QR code data using a specific redundancy level.
    // 2. Generate the QR code image with the specified pixel size and foreground color.
    // 3. If a background color is provided (non-transparent), replace the center of the QR code image with a background color.
    //
    // Note: The actual QR code generation and manipulation code is currently commented out.
}

void QrCode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
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
