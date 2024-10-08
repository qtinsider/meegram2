#pragma once

#include <td/telegram/td_api.h>

#include <QVariant>

class Client;
class StorageManager;

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
    explicit File(QObject *parent = nullptr);
    explicit File(td::td_api::file *file, QObject *parent = nullptr);

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

    void setFile(td::td_api::file *file);

signals:
    void fileChanged();

private slots:
    void onItemChanged(int fileId, td::td_api::Object *object);

private:
    Client *m_client{};
    StorageManager *m_storageManager{};

    td::td_api::file *m_file{};
};

// This allows us to store File * as a QVariant
Q_DECLARE_METATYPE(File *);
