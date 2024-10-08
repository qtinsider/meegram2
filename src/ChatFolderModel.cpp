#include "ChatFolderModel.hpp"

#include "StorageManager.hpp"

#include <QDebug>
#include <QStringList>
#include <algorithm>

ChatFolderModel::ChatFolderModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(TitleRole, "name");

    setRoleNames(roles);

    m_chatFolders = StorageManager::instance().chatFolders();
    setLocaleString(m_localeString);  // Assuming `m_localeString` is still valid here
}

const QString &ChatFolderModel::localeString() const
{
    return m_localeString;
}

void ChatFolderModel::setLocaleString(const QString &value)
{
    auto folder = td::td_api::make_object<td::td_api::chatFolderInfo>();
    folder->id_ = 0;
    folder->title_ = value.toStdString();

    auto it = std::ranges::find_if(m_chatFolders, [](const auto &folder) { return folder->id_ == 0; });

    if (it != m_chatFolders.end())
    {
        // Delete the old pointer to prevent memory leak
        delete *it;
        // Replace the existing folder with id 0
        *it = folder.release();
    }
    else
    {
        // Insert the new folder at the beginning
        m_chatFolders.emplace(m_chatFolders.begin(), folder.release());
    }

    emit countChanged();
}

int ChatFolderModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_chatFolders.size();
}

QVariant ChatFolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto &chatFolder = m_chatFolders.at(index.row());
    switch (role)
    {
        case IdRole:
            return chatFolder->id_;
        case TitleRole:
            return QString::fromStdString(chatFolder->title_);
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

void ChatFolderModel::retranslateUi()
{

}
