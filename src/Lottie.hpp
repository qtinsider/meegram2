#pragma once

#include <rlottie.h>

#include <QDeclarativeItem>

class Lottie : public QDeclarativeItem
{
    Q_OBJECT
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status WRITE setStatus NOTIFY statusChanged)

    Q_ENUMS(Status)
public:
    explicit Lottie(QDeclarativeItem *parent = nullptr);
    ~Lottie() override;

    enum Status { Null, Loading, Ready, Error };

    Status status() const;

    QUrl source() const;
    void setSource(const QUrl &source);

    Q_INVOKABLE void play();
    Q_INVOKABLE void stop();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

signals:
    void sourceChanged();
    void statusChanged();
    void finished();

protected:
    void componentComplete() override;

private slots:
    void renderNextFrame();

private:
    void load();
    void setStatus(Status status);

    int m_currentFrame{};
    int m_frameCount{};
    int m_frameRate{30};

    QUrl m_source;
    QTimer *m_frameTimer;

    Status m_status = Status::Null;

    std::unique_ptr<rlottie::Animation> m_animation;
};
