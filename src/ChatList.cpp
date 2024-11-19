#include "ChatList.hpp"

ChatList::ChatList(QObject *parent)
    : QObject(parent)
{
}

ChatList::ChatList(td::td_api::object_ptr<td::td_api::ChatList> list, QObject *parent)
    : QObject(parent)
    , m_type(Type::None)
    , m_folderId(0)
{
    switch (list->get_id())
    {
        case td::td_api::chatListMain::ID:
            m_type = Type::Main;
            break;
        case td::td_api::chatListArchive::ID:
            m_type = Type::Archive;
            break;
        case td::td_api::chatListFolder::ID:
            m_type = Type::Folder;
            if (const auto folder = static_cast<const td::td_api::chatListFolder *>(list.get()))
            {
                m_folderId = folder->chat_folder_id_;
            }
            break;
        default:
            // m_type and m_folderId remain as initialized (None and 0).
            break;
    }
}

bool ChatList::operator==(const ChatList &other) const noexcept
{
    return this->type() == other.type() && this->folderId() == other.folderId();
}

ChatList::Type ChatList::type() const
{
    return m_type;
}

int ChatList::folderId() const
{
    return m_folderId;
}

void ChatList::setType(Type type)
{
    if (m_type != type)
    {
        m_type = type;
        emit listChanged();
    }
}

void ChatList::setFolderId(int folderId)
{
    if (m_folderId != folderId)
    {
        m_folderId = folderId;
        emit listChanged();
    }
}
