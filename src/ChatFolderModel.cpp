#include "ChatFolderModel.hpp"

#include "StorageManager.hpp"

#include <ranges>

ChatFolderModel::ChatFolderModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(TitleRole, "name");

    setRoleNames(roles);

    auto newFolders = StorageManager::instance().chatFolders();

    m_chatFolders =
        newFolders | std::ranges::views::transform([](const auto &folder) { return std::weak_ptr<ChatFolderInfo>(folder); }) | std::ranges::to<std::vector>();
}

int ChatFolderModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_chatFolders.size();
}

QVariant ChatFolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= static_cast<int>(m_chatFolders.size()))
        return QVariant();

    auto folderPtr = m_chatFolders.at(index.row()).lock();

    if (!folderPtr)
    {
        return QVariant();
    }

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
    QModelIndex modelIndex = createIndex(index, 0);
    QVariantMap result;
    result.insert("id", data(modelIndex, IdRole));
    result.insert("name", data(modelIndex, TitleRole));  // title
    return result;
}

int ChatFolderModel::count() const noexcept
{
    return m_chatFolders.size();
}
