#include "File.hpp"

File::File(QObject *parent)
    : QObject(parent)
    , m_remoteId("")
    , m_remoteUniqueId("")
    , m_localPath("")
    , m_id(0)
    , m_expectedSize(0)
    , m_downloadedSize(0)
    , m_uploadedSize(0)
    , m_hasDownloaded(false)
    , m_isDownloading(false)
    , m_hasUploaded(false)
    , m_isUploading(false)
{
}

QString File::remoteId() const
{
    return m_remoteId;
}

QString File::remoteUniqueId() const
{
    return m_remoteUniqueId;
}

QString File::localPath() const
{
    return m_localPath;
}

int File::id() const
{
    return m_id;
}

int File::expectedSize() const
{
    return m_expectedSize;
}

int File::downloadedSize() const
{
    return m_downloadedSize;
}

int File::uploadedSize() const
{
    return m_uploadedSize;
}

bool File::hasDownloaded() const
{
    return m_hasDownloaded;
}

bool File::isDownloading() const
{
    return m_isDownloading;
}

bool File::hasUploaded() const
{
    return m_hasUploaded;
}

bool File::isUploading() const
{
    return m_isUploading;
}

void File::startDownload()
{

}

void File::stopDownload()
{

}

void File::stopUpload()
{

}

void File::setFromVariantMap(const QVariantMap &map)
{

}
