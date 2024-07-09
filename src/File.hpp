#pragma once

#include <QObject>
#include <QVariant>

class File : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint32 id READ id NOTIFY idChanged)
    Q_PROPERTY(qint64 size READ size NOTIFY sizeChanged)
    Q_PROPERTY(qint64 expectedSize READ expectedSize NOTIFY expectedSizeChanged)
    Q_PROPERTY(QVariantMap local READ local NOTIFY localChanged)
    Q_PROPERTY(QVariantMap remote READ remote NOTIFY remoteChanged)

public:
    explicit File(QObject *parent = nullptr);

    qint32 id() const;
    qint64 size() const;
    qint64 expectedSize() const;
    QVariantMap local() const;
    QVariantMap remote() const;

    void setId(qint32 id);
    void setSize(qint64 size);
    void setExpectedSize(qint64 expectedSize);
    void setLocal(const QVariantMap &local);
    void setRemote(const QVariantMap &remote);

    void setFromVariantMap(const QVariantMap &map);

signals:
    void idChanged();
    void sizeChanged();
    void expectedSizeChanged();
    void localChanged();
    void remoteChanged();

private:
    qint32 m_id;
    qint64 m_size;
    qint64 m_expectedSize;
    QVariantMap m_local;
    QVariantMap m_remote;
};
