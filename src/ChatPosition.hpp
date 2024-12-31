#pragma once

#include <td/telegram/td_api.h>

#include <QObject>

#include <memory>

class ChatList
{
    Q_GADGET

    Q_PROPERTY(Type type READ type CONSTANT)
    Q_PROPERTY(int folderId READ folderId CONSTANT)

    Q_ENUMS(Type)
public:
    enum Type { None, Main, Archive, Folder };

    ChatList(td::td_api::object_ptr<td::td_api::ChatList> list);
    ChatList(Type type, int folderId = 0);

    bool operator==(const ChatList &other) const noexcept;

    Type type() const;
    int folderId() const;

private:
    Type m_type;
    int m_folderId;
};

class ChatPosition
{
    Q_GADGET

    Q_PROPERTY(ChatList *list READ list CONSTANT)
    Q_PROPERTY(qlonglong order READ order CONSTANT)
    Q_PROPERTY(bool isPinned READ isPinned CONSTANT)

public:
    ChatPosition(td::td_api::object_ptr<td::td_api::chatPosition> position);

    ChatList *list() const;
    qlonglong order() const;
    bool isPinned() const;

private:
    qlonglong m_order{};
    bool m_isPinned{};

    std::unique_ptr<ChatList> m_list;
};
