#include "ChatPosition.hpp"

ChatPosition::ChatPosition(td::td_api::object_ptr<td::td_api::chatPosition> position, QObject *parent)
    : QObject(parent)
    , m_order(position->order_)
    , m_isPinned(position->is_pinned_)
    , m_list(std::make_unique<ChatList>(std::move(position->list_), this))
{
}

ChatList *ChatPosition::list() const
{
    return m_list.get();
}

qlonglong ChatPosition::order() const
{
    return m_order;
}

bool ChatPosition::isPinned() const
{
    return m_isPinned;
}
