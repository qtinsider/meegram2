#include "File.hpp"

#include "StorageManager.hpp"

#include <QDebug>

File::File(QObject *parent)
    : QObject(parent)
    , m_client(StorageManager::instance().client())
    , m_storageManager(&StorageManager::instance())
{
    connect(m_storageManager, SIGNAL(fileUpdated(int, td::td_api::Object *)), this, SLOT(onDataChanged(int, td::td_api::Object *)));
}

File::File(td::td_api::file *file, QObject *parent)
    : QObject(parent)
    , m_client(StorageManager::instance().client())
    , m_storageManager(&StorageManager::instance())
    , m_file(file)
{
    connect(m_storageManager, SIGNAL(fileUpdated(int, td::td_api::Object *)), this, SLOT(onDataChanged(int, td::td_api::Object *)));
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

bool File::canBeDownloaded() const
{
    return m_file && m_file->local_ && m_file->local_->can_be_downloaded_;
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

qlonglong File::uploadedSize() const
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
    m_client->send(td::td_api::make_object<td::td_api::downloadFile>(m_file->id_, 1, 0, 0, false), {});
}

void File::cancelDownloadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::cancelDownloadFile>(m_file->id_, false), {});
}

void File::cancelUploadFile()
{
    m_client->send(td::td_api::make_object<td::td_api::cancelPreliminaryUploadFile>(m_file->id_), {});
}

void File::setFile(td::td_api::file *file)
{
    m_file = file;

    emit fileChanged();
}

void File::onDataChanged(int fileId, td::td_api::Object *object)
{
    if (object->get_id() != td::td_api::updateFile::ID)
        return;

    if (fileId != m_file->id_)
        return;

    if (auto file = m_storageManager->getFile(m_file->id_); file)
    {
        m_file = file;

        emit fileChanged();
    }
}
