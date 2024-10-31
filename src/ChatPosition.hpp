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
    Type m_type{None};
    int m_folderId{};
};

class ChatPosition : public QObject
{
    Q_OBJECT

    Q_PROPERTY(ChatList *list READ list CONSTANT)
    Q_PROPERTY(qlonglong order READ order CONSTANT)
    Q_PROPERTY(bool isPinned READ isPinned CONSTANT)

public:
    explicit ChatPosition(td::td_api::object_ptr<td::td_api::chatPosition> position, QObject *parent = nullptr);

    ChatList *list() const;
    qlonglong order() const;
    bool isPinned() const;

private:
    qlonglong m_order{};
    bool m_isPinned{};

    std::unique_ptr<ChatList> m_list;
};

class ChatFolderInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int id READ id CONSTANT)
    Q_PROPERTY(QString title READ title CONSTANT)
    Q_PROPERTY(int colorId READ colorId CONSTANT)
    Q_PROPERTY(bool isShareable READ isShareable CONSTANT)
    Q_PROPERTY(bool hasMyInviteLinks READ hasMyInviteLinks CONSTANT)

public:
    explicit ChatFolderInfo(td::td_api::object_ptr<td::td_api::chatFolderInfo> info, QObject *parent = nullptr);

    int id() const;
    QString title() const;
    int colorId() const;
    bool isShareable() const;
    bool hasMyInviteLinks() const;

private:
    int m_id;
    QString m_title;
    int m_colorId;
    bool m_isShareable;
    bool m_hasMyInviteLinks;
};
