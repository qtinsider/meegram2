#pragma once

#include <td/telegram/td_api.h>

#include <QObject>

#include <memory>

class ChatFolderInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)

public:
    explicit ChatFolderInfo(td::td_api::object_ptr<td::td_api::chatFolderInfo> info, QObject *parent = nullptr);

    int id() const;
    QString title() const;

private:
    int m_id;
    QString m_title;
};
