#include "File.hpp"

File::File(td::td_api::object_ptr<td::td_api::file> file, QObject *parent)
    : QObject(parent)
    , m_file(std::move(file))
{
    updateFileProperties();
}

int File::id() const
{
    return m_id;
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

    if (m_file->local_)
    {
        m_localPath = QString::fromStdString(m_file->local_->path_);
        m_canBeDownloaded = m_file->local_->can_be_downloaded_;
        m_isDownloadingActive = m_file->local_->is_downloading_active_;
        m_isDownloadingCompleted = m_file->local_->is_downloading_completed_;
    }
}
