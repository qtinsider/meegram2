#include "File.hpp"

File::File(QObject *parent)
    : QObject(parent)
    , m_id(0)
    , m_size(0)
    , m_expectedSize(0)
{
}

qint32 File::id() const
{
    return m_id;
}

void File::setId(qint32 id)
{
    if (m_id != id)
    {
        m_id = id;
        emit idChanged();
    }
}

qint64 File::size() const
{
    return m_size;
}

void File::setSize(qint64 size)
{
    if (m_size != size)
    {
        m_size = size;
        emit sizeChanged();
    }
}

qint64 File::expectedSize() const
{
    return m_expectedSize;
}

void File::setExpectedSize(qint64 expectedSize)
{
    if (m_expectedSize != expectedSize)
    {
        m_expectedSize = expectedSize;
        emit expectedSizeChanged();
    }
}

QVariantMap File::local() const
{
    return m_local;
}

void File::setLocal(const QVariantMap &local)
{
    if (m_local != local)
    {
        m_local = local;
        emit localChanged();
    }
}

QVariantMap File::remote() const
{
    return m_remote;
}

void File::setRemote(const QVariantMap &remote)
{
    if (m_remote != remote)
    {
        m_remote = remote;
        emit remoteChanged();
    }
}

void File::setFromVariantMap(const QVariantMap &map)
{
    setId(map.value("id").toInt());
    setSize(map.value("size").toLongLong());
    setExpectedSize(map.value("expected_size").toLongLong());
    setLocal(map.value("local").toMap());
    setRemote(map.value("remote").toMap());
}
