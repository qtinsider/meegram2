#pragma once

#include <QObject>
#include <QVariant>

class File : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id NOTIFY fileChanged)
    Q_PROPERTY(QString remoteId READ remoteId NOTIFY fileChanged)
    Q_PROPERTY(QString remoteUniqueId READ remoteUniqueId NOTIFY fileChanged)
    Q_PROPERTY(QString localPath READ localPath NOTIFY fileChanged)
    Q_PROPERTY(int expectedSize READ expectedSize NOTIFY fileChanged)
    Q_PROPERTY(int downloadedSize READ downloadedSize NOTIFY fileChanged)
    Q_PROPERTY(int uploadedSize READ uploadedSize NOTIFY fileChanged)

    Q_PROPERTY(bool hasDownloaded READ hasDownloaded NOTIFY fileChanged)
    Q_PROPERTY(bool isDownloading READ isDownloading NOTIFY fileChanged)
    Q_PROPERTY(bool hasUploaded READ hasUploaded NOTIFY fileChanged)
    Q_PROPERTY(bool isUploading READ isUploading NOTIFY fileChanged)

public:
    explicit File(QObject *parent = nullptr);

    int id() const;
    QString remoteId() const;
    QString remoteUniqueId() const;
    QString localPath() const;
    int expectedSize() const;
    int downloadedSize() const;
    int uploadedSize() const;

    bool hasDownloaded() const;
    bool isDownloading() const;
    bool hasUploaded() const;
    bool isUploading() const;

    Q_INVOKABLE void startDownload();
    Q_INVOKABLE void stopDownload();
    Q_INVOKABLE void stopUpload();

    void setFromVariantMap(const QVariantMap &map);

signals:
    void fileChanged();

private:
    QString m_remoteId;
    QString m_remoteUniqueId;
    QString m_localPath;

    int m_id;
    int m_expectedSize;
    int m_downloadedSize;
    int m_uploadedSize;

    bool m_hasDownloaded;
    bool m_isDownloading;
    bool m_hasUploaded;
    bool m_isUploading;
};
