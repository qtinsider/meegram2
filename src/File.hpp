#pragma once

#include <td/telegram/td_api.h>

#include <QVariant>

#include <memory>

class Client;

class File : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id NOTIFY fileChanged)

    Q_PROPERTY(int size READ size NOTIFY fileChanged)
    Q_PROPERTY(int expectedSize READ expectedSize NOTIFY fileChanged)

    Q_PROPERTY(QString localPath READ localPath NOTIFY fileChanged)

    Q_PROPERTY(bool canBeDownloaded READ canBeDownloaded NOTIFY fileChanged)
    Q_PROPERTY(bool isDownloadingActive READ isDownloadingActive NOTIFY fileChanged)
    Q_PROPERTY(bool isDownloadingCompleted READ isDownloadingCompleted NOTIFY fileChanged)

    Q_PROPERTY(QString remoteId READ remoteId NOTIFY fileChanged)
    Q_PROPERTY(QString remoteUniqueId READ remoteUniqueId NOTIFY fileChanged)

    Q_PROPERTY(qlonglong uploadedSize READ uploadedSize NOTIFY fileChanged)
    Q_PROPERTY(bool isUploadingActive READ isUploadingActive NOTIFY fileChanged)
    Q_PROPERTY(bool isUploadingCompleted READ isUploadingCompleted NOTIFY fileChanged)

public:
    explicit File(td::td_api::object_ptr<td::td_api::file> file, QObject *parent = nullptr);

    int id() const;

    int size() const;
    int expectedSize() const;

    QString localPath() const;

    bool canBeDownloaded() const;
    bool isDownloadingActive() const;
    bool isDownloadingCompleted() const;

    QString remoteId() const;
    QString remoteUniqueId() const;

    qlonglong uploadedSize() const;
    bool isUploadingActive() const;
    bool isUploadingCompleted() const;

    Q_INVOKABLE void downloadFile();
    Q_INVOKABLE void cancelDownloadFile();
    Q_INVOKABLE void cancelUploadFile();

    void setFile(td::td_api::object_ptr<td::td_api::file> file);

signals:
    void fileChanged();

private:
    void updateFileProperties();

    int m_id;

    int m_size;
    int m_expectedSize;

    QString m_localPath;

    bool m_canBeDownloaded;
    bool m_isDownloadingActive;
    bool m_isDownloadingCompleted;

    QString m_remoteId;
    QString m_remoteUniqueId;

    qlonglong m_uploadedSize;
    bool m_isUploadingActive;
    bool m_isUploadingCompleted;

    std::shared_ptr<Client> m_client;

    td::td_api::object_ptr<td::td_api::file> m_file;
};

// This allows us to store File * as a QVariant
Q_DECLARE_METATYPE(File *);
