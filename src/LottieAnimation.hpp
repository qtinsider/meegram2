#pragma once

#include <rlottie.h>

#include <QDeclarativeItem>
#include <QTimer>

class LottieAnimation : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)
    Q_PROPERTY(int loop READ loop WRITE setLoop NOTIFY loopChanged)

    Q_ENUMS(Status)
public:
    explicit LottieAnimation(QDeclarativeItem *parent = nullptr);

    enum Status { Null, Loading, Ready, Error };

    Status status() const noexcept;
    void setStatus(Status status) noexcept;

    QUrl source() const noexcept;
    void setSource(const QUrl &source) noexcept;

    int loop() const noexcept;
    void setLoop(int loop) noexcept;

public slots:
    void play();
    void stop();

signals:
    void sourceChanged();
    void statusChanged();
    void loopChanged();

    void finished();

protected:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *) override;

    void componentComplete() override;

private slots:
    void updateFrame() noexcept;

private:
    void loadContent() noexcept;
    void initializeAnimation() noexcept;

    static QString urlToLocalFileOrQrc(const QUrl &url) noexcept;

    int m_frameCount{0};
    int m_currentFrame{0};
    int m_frameRate{30};  // 30 FPS

    int m_loop{1};
    int m_loopIteration{0};

    Status m_status{Status::Null};

    QUrl m_source;
    QTimer m_frameTimer;
    QPixmap m_cachedPixmap;

    std::unique_ptr<rlottie::Animation> m_animation;
};
