#pragma once

#include <rlottie.h>

#include <QDeclarativeItem>
#include <QTimer>

class LottieAnimation : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(qreal loop READ loop WRITE setLoop NOTIFY loopChanged)

    Q_ENUMS(Status)
public:
    explicit LottieAnimation(QDeclarativeItem *parent = nullptr);

    enum Status { Null, Loading, Ready, Error };

    Status status() const noexcept;
    void setStatus(Status status);

    QUrl source() const noexcept;
    void setSource(const QUrl &source);

    qreal loop() const noexcept;
    void setLoop(qreal loop);

public slots:
    void play();
    void stop();

signals:
    void sourceChanged();
    void statusChanged();
    void finished();
    void loopChanged();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    void componentComplete();

private slots:
    void updateFrame();

private:
    void loadContent();
    void initializeAnimation();

    static QString urlToLocalFileOrQrc(const QUrl &url);

    int m_frameCount{0};
    int m_currentFrame{0};
    int m_frameRate{30};  // 30 FPS

    qreal m_loop{1};
    qreal m_loopIteration{0};

    Status m_status{Status::Null};

    QUrl m_source;
    QTimer m_frameTimer;
    QPixmap m_cachedPixmap;

    std::unique_ptr<rlottie::Animation> m_animation;
};
