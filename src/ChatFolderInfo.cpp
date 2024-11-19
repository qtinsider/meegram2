#include "ChatFolderInfo.hpp"

ChatFolderInfo::ChatFolderInfo(td::td_api::object_ptr<td::td_api::chatFolderInfo> info, QObject *parent)
    : QObject(parent)
    , m_id(info->id_)
    , m_title(QString::fromStdString(info->title_))
{
}

int ChatFolderInfo::id() const
{
    return m_id;
}

QString ChatFolderInfo::title() const
{
    return m_title;
}
