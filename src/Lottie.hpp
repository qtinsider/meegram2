#pragma once

#include <rlottie.h>

#include <QDeclarativeItem>
#include <QUrl>

#include <memory>

class Lottie : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
public:
    explicit Lottie(QDeclarativeItem *parent = nullptr);

    QUrl source() const;
    void setSource(const QUrl &source);

    void paint(QPainter *p, const QStyleOptionGraphicsItem *, QWidget *) override;

    void componentComplete() override;

    void load();

signals:
    void sourceChanged();

private:
    void renderRequest(int frame);

    QSize size;
    QImage image;
    QUrl m_source;

    std::unique_ptr<rlottie::Animation> animation;
};
