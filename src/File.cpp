#include "File.hpp"

#include "StorageManager.hpp"

#include <QDebug>

File::File(td::td_api::object_ptr<td::td_api::file> file, QObject *parent)
    : QObject(parent)
    , m_client(StorageManager::instance().client())
    , m_file(std::move(file))
{
    updateFileProperties();
}

int File::id() const
{
    return m_id;
}

int File::size() const
{
    return m_size;
}

int File::expectedSize() const
{
    return m_expectedSize;
}

QString File::localPath() const
{
    return m_localPath;
}

bool File::canBeDownloaded() const
{
    return m_canBeDownloaded;
}

bool File::isDownloadingActive() const
{
    return m_isDownloadingActive;
}

bool File::isDownloadingCompleted() const
{
    return m_isDownloadingCompleted;
}

QString File::remoteId() const
{
    return m_remoteId;
}

QString File::remoteUniqueId() const
{
    return m_remoteUniqueId;
}

qlonglong File::uploadedSize() const
{
    return m_uploadedSize;
}

bool File::isUploadingActive() const
{
    return m_isUploadingActive;
}

bool File::isUploadingCompleted() const
{
    return m_isUploadingCompleted;
}

void File::downloadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::downloadFile>(m_id, 1, 0, 0, false), {});
}

void File::cancelDownloadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::cancelDownloadFile>(m_id, false), {});
}

void File::cancelUploadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::cancelPreliminaryUploadFile>(m_id), {});
}

void File::setFile(td::td_api::object_ptr<td::td_api::file> file)
{
    m_file = std::move(file);
    updateFileProperties();
    emit fileChanged();
}

void File::updateFileProperties()
{
    if (!m_file)
        return;

    m_id = m_file->id_;
    m_size = m_file->size_;
    m_expectedSize = m_file->expected_size_;

    if (m_file->local_)
    {
        m_localPath = QString::fromStdString(m_file->local_->path_);
        m_canBeDownloaded = m_file->local_->can_be_downloaded_;
        m_isDownloadingActive = m_file->local_->is_downloading_active_;
        m_isDownloadingCompleted = m_file->local_->is_downloading_completed_;
    }

    if (m_file->remote_)
    {
        m_remoteId = QString::fromStdString(m_file->remote_->id_);
        m_remoteUniqueId = QString::fromStdString(m_file->remote_->unique_id_);
        m_uploadedSize = m_file->remote_->uploaded_size_;
        m_isUploadingActive = m_file->remote_->is_uploading_active_;
        m_isUploadingCompleted = m_file->remote_->is_uploading_completed_;
    }
}
