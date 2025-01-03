#include "ChatFolderModel.hpp"

ChatFolderInfo::ChatFolderInfo(td::td_api::object_ptr<td::td_api::chatFolderInfo> info)
    : m_id(info->id_)
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

ChatFolderModel::ChatFolderModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(TitleRole, "name");

    setRoleNames(roles);
}

void ChatFolderModel::setItems(std::vector<std::shared_ptr<ChatFolderInfo>> chatFolders)
{
    beginResetModel();
    m_chatFolders = std::move(chatFolders);
    endResetModel();

    emit countChanged();
}

int ChatFolderModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return static_cast<int>(m_chatFolders.size());
}

QVariant ChatFolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_chatFolders.size()))
        return QVariant();

    auto folderPtr = m_chatFolders.at(index.row());

    if (!folderPtr)
        return QVariant();

    switch (role)
    {
        case IdRole:
            return folderPtr->id();
        case TitleRole:
            return folderPtr->title();
        default:
            return QVariant();
    }
}

QVariant ChatFolderModel::get(int index) const noexcept
{
    if (index < 0 || index >= static_cast<int>(m_chatFolders.size()))
    {
        return QVariant();
    }

    return QVariant::fromValue(m_chatFolders.at(index).get());
}

int ChatFolderModel::count() const noexcept
{
    return static_cast<int>(m_chatFolders.size());
}
