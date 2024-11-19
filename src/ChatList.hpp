#pragma once

#include <td/telegram/td_api.h>

#include <QObject>

#include <memory>

class ChatList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(Type type READ type WRITE setType NOTIFY listChanged)
    Q_PROPERTY(int folderId READ folderId WRITE setFolderId NOTIFY listChanged)

    Q_ENUMS(Type)
public:
    explicit ChatList(QObject *parent = nullptr);
    explicit ChatList(td::td_api::object_ptr<td::td_api::ChatList> list, QObject *parent = nullptr);

    enum Type { None, Main, Archive, Folder };

    bool operator==(const ChatList &other) const noexcept;

    Type type() const;
    int folderId() const;

    void setType(Type type);
    void setFolderId(int folderId);

signals:
    void listChanged();

private:
    Type m_type;
    int m_folderId;
};
