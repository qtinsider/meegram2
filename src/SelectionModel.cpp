#include "SelectionModel.hpp"

#include <QStringList>

#include <algorithm>

CountryModel::CountryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "name");
    roles.insert(Iso2Role, "iso2");
    roles.insert(CodeRole, "code");

    setRoleNames(roles);
}

const QVariantList &CountryModel::countries() const noexcept
{
    return m_countries;
}

void CountryModel::setCountries(QVariantList countries)
{
    if (m_countries != countries)
    {
        beginResetModel();
        m_countries = std::move(countries);
        endResetModel();

        emit countChanged();
        emit countriesChanged();
    }
}

int CountryModel::rowCount(const QModelIndex &) const
{
    return m_countries.count();
}

QVariant CountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_countries.isEmpty())
        return QVariant();

    const auto &countryInfo = m_countries.at(index.row()).toMap();
    switch (role)
    {
        case Qt::DisplayRole:
            return countryInfo.value("name").toString();
        case Iso2Role:
            return countryInfo.value("country_code").toString();
        case CodeRole:
            return countryInfo.value("calling_codes").toStringList();
    }

    return QVariant();
}

QVariantMap CountryModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);

    QVariantMap result;
    result.insert("name", data(modelIndex, Qt::DisplayRole));
    result.insert("iso2", data(modelIndex, Iso2Role));
    result.insert("code", data(modelIndex, CodeRole));

    return result;
}

int CountryModel::count() const noexcept
{
    return m_countries.count();
}

int CountryModel::getDefaultIndex() const noexcept
{
    // TODO(strawberry): refactor
    auto it = std::ranges::find_if(m_countries, [](const auto &value) {
        return value.toMap().value("country_code").toString().compare("NG", Qt::CaseInsensitive) == 0;
    });

    if (it != m_countries.end())
    {
        return int(std::distance(m_countries.begin(), it));
    }

    return {};
}

ChatFolderModel::ChatFolderModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(Qt::DisplayRole, "name");
    roles.insert(IconNameRole, "iconName");

    setRoleNames(roles);
}

QVariantList ChatFolderModel::getChatFolders() const noexcept
{
    return m_chatFolders;
}

void ChatFolderModel::setChatFolders(QVariantList value) noexcept
{
    if (m_chatFolders != value)
    {
        beginResetModel();
        m_chatFolders = std::move(value);
        endResetModel();

        emit chatFoldersChanged();
    }
}

int ChatFolderModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_chatFolders.count();
}

QVariant ChatFolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto chatFolder = m_chatFolders.value(index.row()).toMap();
    switch (role)
    {
        case IdRole:
            return chatFolder.value("id").toInt();
        case Qt::DisplayRole:
            return chatFolder.value("title").toString();
        case IconNameRole:
            return chatFolder.value("icon_name").toString();
        default:
            return QVariant();
    }
}

QVariantMap ChatFolderModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);
    QVariantMap result;
    result.insert("id", data(modelIndex, IdRole));
    result.insert("name", data(modelIndex, Qt::DisplayRole));  // title
    result.insert("iconName", data(modelIndex, IconNameRole));
    return result;
}

int ChatFolderModel::count() const noexcept
{
    return m_chatFolders.count();
}
