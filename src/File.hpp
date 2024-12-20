#pragma once

#include <td/telegram/td_api.h>

#include <QVariant>

#include <memory>

class Client;

class File : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id NOTIFY fileChanged)

    Q_PROPERTY(QString localPath READ localPath NOTIFY fileChanged)

    Q_PROPERTY(bool canBeDownloaded READ canBeDownloaded NOTIFY fileChanged)
    Q_PROPERTY(bool isDownloadingActive READ isDownloadingActive NOTIFY fileChanged)
    Q_PROPERTY(bool isDownloadingCompleted READ isDownloadingCompleted NOTIFY fileChanged)

public:
    explicit File(td::td_api::object_ptr<td::td_api::file> file, QObject *parent = nullptr);

    int id() const;

    QString localPath() const;

    bool canBeDownloaded() const;
    bool isDownloadingActive() const;
    bool isDownloadingCompleted() const;

    Q_INVOKABLE void downloadFile();

    void setFile(td::td_api::object_ptr<td::td_api::file> file);

signals:
    void fileChanged();

private:
    void updateFileProperties();

    int m_id;

    QString m_localPath;

    bool m_canBeDownloaded;
    bool m_isDownloadingActive;
    bool m_isDownloadingCompleted;

    std::shared_ptr<Client> m_client;

    td::td_api::object_ptr<td::td_api::file> m_file;
};

// This allows us to store File * as a QVariant
Q_DECLARE_METATYPE(File *);
