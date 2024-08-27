#pragma once

#include <QColor>
#include <QDeclarativeItem>
#include <QImage>

class QrCodeItem : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(int size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(QColor foreground READ foreground WRITE setForeground NOTIFY foregroundChanged)
    Q_PROPERTY(QColor background READ background WRITE setBackground NOTIFY backgroundChanged)

public:
    QrCodeItem(QDeclarativeItem *parent = nullptr);

    QString text() const;
    void setText(const QString &text);

    int size() const;
    void setSize(int size);

    QColor foreground() const;
    void setForeground(const QColor &color);

    QColor background() const;
    void setBackground(const QColor &color);

signals:
    void textChanged();
    void sizeChanged();
    void foregroundChanged();
    void backgroundChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    void updateQrCode();

    QString m_text;
    int m_size = 100;
    QColor m_foreground = Qt::black;
    QColor m_background = Qt::white;

    QImage m_qrCodeImage;
};
