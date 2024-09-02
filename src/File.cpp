#include "File.hpp"

#include "StorageManager.hpp"

File::File(QObject *parent)
    : QObject(parent)
    , m_client(StorageManager::instance().client())
    , m_storageManager(&StorageManager::instance())
{
    connect(m_storageManager, SIGNAL(dataChanged(td::td_api::Object *)), this, SLOT(onDataChanged(td::td_api::Object *)));
}

int File::id() const
{
    return m_file ? m_file->id_ : 0;
}

int File::size() const
{
    return m_file ? m_file->size_ : 0;
}

int File::expectedSize() const
{
    return m_file ? m_file->expected_size_ : 0;
}

QString File::localPath() const
{
    return m_file && m_file->local_ ? QString::fromStdString(m_file->local_->path_) : QString();
}

bool File::isDownloadingActive() const
{
    return m_file && m_file->local_ && m_file->local_->is_downloading_active_;
}

bool File::isDownloadingCompleted() const
{
    return m_file && m_file->local_ && m_file->local_->is_downloading_completed_;
}

QString File::remoteId() const
{
    return m_file && m_file->remote_ ? QString::fromStdString(m_file->remote_->id_) : QString();
}

QString File::remoteUniqueId() const
{
    return m_file && m_file->remote_ ? QString::fromStdString(m_file->remote_->unique_id_) : QString();
}

qint64 File::uploadedSize() const
{
    return m_file && m_file->remote_ ? m_file->remote_->uploaded_size_ : 0;
}

bool File::isUploadingActive() const
{
    return m_file && m_file->remote_ && m_file->remote_->is_uploading_active_;
}

bool File::isUploadingCompleted() const
{
    return m_file && m_file->remote_ && m_file->remote_->is_uploading_completed_;
}

void File::downloadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::downloadFile>(), {});
}

void File::cancelDownloadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::cancelDownloadFile>(), {});
}

void File::cancelUploadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::cancelPreliminaryUploadFile>(), {});
}

void File::setFile(td::td_api::file *file)
{
    m_file = file;

    emit fileChanged(file->id_);
}

void File::onDataChanged(td::td_api::Object *object)
{
    if (object->get_id() == td::td_api::updateFile::ID)
    {
        auto file = static_cast<td::td_api::updateFile *>(object);
        if (file->file_->id_ == m_file->id_)
        {
            m_file = m_storageManager->getFile(m_file->id_);
            emit fileChanged(m_file->id_);
        }
    }
}
