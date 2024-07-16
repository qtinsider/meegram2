#include "SelectionModel.hpp"

#include "Serialize.hpp"

#include <QDebug>

#include <algorithm>

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

int FlexibleListModel::defaultIndex() const noexcept
{
    return m_defaultIndex;
}

void FlexibleListModel::setDefaultIndex(const QVariant &criteria) noexcept
{
    int index = calculateDefaultIndex(criteria.toMap());
    if (index != m_defaultIndex)
    {
        m_defaultIndex = index;
        emit defaultIndexChanged();
    }
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

int FlexibleListModel::calculateDefaultIndex(const QVariantMap &criteria) const noexcept
{
    auto it = std::ranges::find_if(m_values, [&criteria](const auto &value) {
        for (auto it = criteria.constBegin(); it != criteria.constEnd(); ++it)
        {
            if (value.toMap().value(it.key()).toString().compare(it.value().toString(), Qt::CaseInsensitive) != 0)
            {
                return false;
            }
        }
        return true;
    });

    if (it != m_values.end())
    {
        return int(std::distance(m_values.begin(), it));
    }

    return -1;
}
