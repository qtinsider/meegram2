#include "SelectionModel.hpp"

#include "Localization.hpp"
#include "Serialize.hpp"

#include <QDebug>

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

QObject *ChatFolderModel::locale() const
{
    return m_locale;
}

void ChatFolderModel::setLocale(QObject *locale)
{
    m_locale = qobject_cast<Locale *>(locale);

    QVariantMap chatFolder;
    chatFolder.insert("id", 0);
    chatFolder.insert("title", m_locale->getString("FilterAllChats"));

    beginInsertRows(QModelIndex(), 0, 0);
    m_chatFolders.insert(0, chatFolder);
    endInsertRows();

    emit countChanged();
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
        m_chatFolders.append(std::move(value));
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

FlexibleListModel::FlexibleListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

int FlexibleListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_values.size();
}

QVariant FlexibleListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_values.size())
        return QVariant();

    if (const auto value = m_values.at(index.row()); value.canConvert<QVariantMap>())
    {
        const auto valueMap = value.toMap();

        if (auto roleName = m_roleNames.value(role); !roleName.isEmpty())
            return valueMap.value(QString::fromUtf8(roleName));

        return QVariant();
    }
    else
    {
        return value;
    }
}

int FlexibleListModel::count() const noexcept
{
    return m_values.size();
}

QVariantList FlexibleListModel::values() const noexcept
{
    return m_values;
}

void FlexibleListModel::setValues(const QVariantList &values) noexcept
{
    beginResetModel();
    m_values = values;

    // Update role names if values are QVariantMap
    if (!m_values.isEmpty() && m_values.first().canConvert<QVariantMap>())
    {
        const auto firstItem = m_values.first().toMap();
        m_roleNames.clear();
        int role = Qt::UserRole + 1;
        for (const auto &key : firstItem.keys())
        {
            m_roleNames[role++] = key.toUtf8();
        }

        setRoleNames(m_roleNames);
    }

    endResetModel();
    emit valuesChanged();
    emit countChanged();
}

void FlexibleListModel::append(const QVariant &value) noexcept
{
    beginInsertRows(QModelIndex(), m_values.size(), m_values.size());
    m_values.append(value);

    // Update role names if the first item is QVariantMap
    if (m_values.size() == 1 && value.canConvert<QVariantMap>())
    {
        const auto firstItem = value.toMap();
        m_roleNames.clear();
        int role = Qt::UserRole + 1;
        for (const auto &key : firstItem.keys())
        {
            m_roleNames[role++] = key.toUtf8();
        }

        setRoleNames(m_roleNames);
    }

    endInsertRows();
    emit valuesChanged();
    emit countChanged();
}

void FlexibleListModel::remove(int index) noexcept
{
    if (index < 0 || index >= m_values.size())
        return;

    beginRemoveRows(QModelIndex(), index, index);
    m_values.removeAt(index);
    endRemoveRows();
    emit valuesChanged();
    emit countChanged();
}

QVariant FlexibleListModel::get(int index) const noexcept
{
    if (index < 0 || index >= m_values.size())
        return QVariant();

    return m_values.at(index);
}

void FlexibleListModel::clear() noexcept
{
    beginResetModel();
    m_values.clear();
    m_roleNames.clear();
    endResetModel();
    emit valuesChanged();
    emit countChanged();
}

void FlexibleListModel::replace(const QString &key, const QVariant &value) noexcept
{
    if (!value.canConvert<QVariantMap>())
    {
        qWarning() << "Invalid value: not a QVariantMap";
        return;
    }

    for (int i = 0; i < m_values.size(); ++i)
    {
        if (m_values[i].toMap().value(key).toString() == value.toMap().value(key).toString())
        {
            m_values[i] = value.toMap();
            emit dataChanged(index(i), index(i));
            emit valuesChanged();
            emit countChanged();
            return;
        }
    }
    append(value);
}

void FlexibleListModel::insert(int index, const QVariant &value) noexcept
{
    if (index < 0 || index > m_values.size())
    {
        qWarning() << "Invalid index";
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_values.insert(index, value);

    // Update role names if the first insertion is QVariantMap
    if (m_values.size() == 1 && value.canConvert<QVariantMap>())
    {
        const auto firstItem = value.toMap();
        m_roleNames.clear();
        int role = Qt::UserRole + 1;
        for (const auto &key : firstItem.keys())
        {
            m_roleNames[role++] = key.toUtf8();
        }

        setRoleNames(m_roleNames);
    }

    endInsertRows();
    emit valuesChanged();
    emit countChanged();
}

void FlexibleListModel::sort(const QString &criteria, bool ascending)
{
    emit layoutAboutToBeChanged();

    std::sort(m_values.begin(), m_values.end(), [&criteria, ascending](const auto &a, const auto &b) {
        if (ascending)
        {
            return a.toMap().value(criteria).toString() < b.toMap().value(criteria).toString();
        }
        else
        {
            return a.toMap().value(criteria).toString() > b.toMap().value(criteria).toString();
        }
    });

    emit layoutChanged();
}
