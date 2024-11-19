#pragma once

#include "ChatList.hpp"

#include <td/telegram/td_api.h>

#include <QObject>

#include <memory>

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
