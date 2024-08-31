#include "SortFilterProxyModel.hpp"

#include <algorithm>

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    connect(this, SIGNAL(modelReset()), this, SLOT(updateRoles()));
    connect(this, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(rowsRemoved(QModelIndex, int, int)), this, SIGNAL(countChanged()));
    connect(this, SIGNAL(modelReset()), this, SLOT(countChanged()));
    connect(this, SIGNAL(layoutChanged()), this, SLOT(countChanged()));
    connect(this, SIGNAL(dataChanged(QModelIndex, QModelIndex)), this, SLOT(onDataChanged(QModelIndex, QModelIndex)));
    setDynamicSortFilter(true);
}

int SortFilterProxyModel::count() const
{
    return rowCount();
}

const QString &SortFilterProxyModel::filterRoleName() const
{
    return m_filterRoleName;
}

void SortFilterProxyModel::setFilterRoleName(const QString &filterRoleName)
{
    if (m_filterRoleName == filterRoleName)
        return;

    m_filterRoleName = filterRoleName;
    updateFilterRole();
    emit filterRoleChanged();
}

QString SortFilterProxyModel::filterPattern() const
{
    return filterRegExp().pattern();
}

void SortFilterProxyModel::setFilterPattern(const QString &filterPattern)
{
    QRegExp regExp = filterRegExp();
    if (regExp.pattern() == filterPattern)
        return;

    regExp.setPattern(filterPattern);
    QSortFilterProxyModel::setFilterRegExp(regExp);
    emit filterPatternChanged();
}

SortFilterProxyModel::PatternSyntax SortFilterProxyModel::filterPatternSyntax() const
{
    return static_cast<PatternSyntax>(filterRegExp().patternSyntax());
}

void SortFilterProxyModel::setFilterPatternSyntax(SortFilterProxyModel::PatternSyntax patternSyntax)
{
    QRegExp regExp = filterRegExp();
    QRegExp::PatternSyntax patternSyntaxTmp = static_cast<QRegExp::PatternSyntax>(patternSyntax);
    if (regExp.patternSyntax() == patternSyntaxTmp)
        return;

    regExp.setPatternSyntax(patternSyntaxTmp);
    QSortFilterProxyModel::setFilterRegExp(regExp);
    emit filterPatternSyntaxChanged();
}

const QVariant &SortFilterProxyModel::filterValue() const
{
    return m_filterValue;
}

void SortFilterProxyModel::setFilterValue(const QVariant &filterValue)
{
    if (m_filterValue == filterValue)
        return;

    m_filterValue = filterValue;
    queueInvalidateFilter();
    emit filterValueChanged();
}

const QString &SortFilterProxyModel::sortRoleName() const
{
    return m_sortRoleName;
}

void SortFilterProxyModel::setSortRoleName(const QString &sortRoleName)
{
    if (m_sortRoleName == sortRoleName)
        return;

    m_sortRoleName = sortRoleName;
    updateSortRole();
    emit sortRoleChanged();
}

bool SortFilterProxyModel::ascendingSortOrder() const
{
    return m_ascendingSortOrder;
}

void SortFilterProxyModel::setAscendingSortOrder(bool ascendingSortOrder)
{
    if (m_ascendingSortOrder == ascendingSortOrder)
        return;

    m_ascendingSortOrder = ascendingSortOrder;
    emit ascendingSortOrderChanged();
    queueInvalidate();
}

void SortFilterProxyModel::classBegin()
{
}

void SortFilterProxyModel::componentComplete()
{
    m_completed = true;
    invalidate();
    sort(0);
}

QVariant SortFilterProxyModel::sourceData(const QModelIndex &sourceIndex, const QString &roleName) const
{
    int role = roleNames().key(roleName.toUtf8());
    return sourceData(sourceIndex, role);
}

QVariant SortFilterProxyModel::sourceData(const QModelIndex &sourceIndex, int role) const
{
    return sourceModel()->data(sourceIndex, role);
}

QVariant SortFilterProxyModel::data(const QModelIndex &index, int role) const
{
    return sourceData(mapToSource(index), role);
}

QHash<int, QByteArray> SortFilterProxyModel::roleNames() const
{
    return m_roleNames.isEmpty() && sourceModel() ? sourceModel()->roleNames() : m_roleNames;
}

int SortFilterProxyModel::roleForName(const QString &roleName) const
{
    return m_roleNames.key(roleName.toUtf8(), -1);
}

QVariantMap SortFilterProxyModel::get(int row) const
{
    QVariantMap map;
    if (row < 0 || row >= rowCount())
        return map;

    QModelIndex modelIndex = index(row, 0);
    for (auto it = roleNames().begin(); it != roleNames().end(); ++it)
        map.insert(it.value(), data(modelIndex, it.key()));

    return map;
}

QVariant SortFilterProxyModel::get(int row, const QString &roleName) const
{
    if (row < 0 || row >= rowCount())
        return QVariant();

    int role = roleForName(roleName);
    if (role == -1)
        return QVariant();

    return data(index(row, 0), role);
}

QModelIndex SortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return QSortFilterProxyModel::mapToSource(proxyIndex);
}

int SortFilterProxyModel::mapToSource(int proxyRow) const
{
    return QSortFilterProxyModel::mapToSource(index(proxyRow, 0)).row();
}

QModelIndex SortFilterProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

int SortFilterProxyModel::mapFromSource(int sourceRow) const
{
    return QSortFilterProxyModel::mapFromSource(sourceModel()->index(sourceRow, 0)).row();
}

void SortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    QSortFilterProxyModel::setSourceModel(sourceModel);
    initRoles();
}

QVariant SortFilterProxyModel::sourceModelVariant() const
{
    return QVariant::fromValue<QObject *>(sourceModel());
}

void SortFilterProxyModel::setSourceModelVariant(const QVariant &sourceModel)
{
    setSourceModel(qobject_cast<QAbstractItemModel *>(sourceModel.value<QObject *>()));
}

bool SortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool SortFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    if (m_completed)
    {
        if (!m_sortRoleName.isEmpty())
        {
            if (QSortFilterProxyModel::lessThan(source_left, source_right))
                return m_ascendingSortOrder;
            if (QSortFilterProxyModel::lessThan(source_right, source_left))
                return !m_ascendingSortOrder;
        }
    }

    return source_left.row() < source_right.row();
}

void SortFilterProxyModel::resetInternalData()
{
    beginResetModel();
    endResetModel();
}

void SortFilterProxyModel::queueInvalidateFilter()
{
    if (!m_invalidateFilterQueued)
    {
        m_invalidateFilterQueued = true;
        QMetaObject::invokeMethod(this, "invalidateFilter", Qt::QueuedConnection);
    }
}

void SortFilterProxyModel::invalidateFilter()
{
    if (!m_invalidateFilterQueued)
        return;

    m_invalidateFilterQueued = false;
    QSortFilterProxyModel::invalidateFilter();
}

void SortFilterProxyModel::queueInvalidate()
{
    if (!m_invalidateQueued)
    {
        m_invalidateQueued = true;
        QMetaObject::invokeMethod(this, "invalidate", Qt::QueuedConnection);
    }
}

void SortFilterProxyModel::invalidate()
{
    if (!m_invalidateQueued)
        return;

    m_invalidateQueued = false;
    QSortFilterProxyModel::invalidate();
}

void SortFilterProxyModel::updateRoleNames()
{
    if (!sourceModel())
        return;

    QHash<int, QByteArray> roleNames = sourceModel()->roleNames();
    if (roleNames.isEmpty())
        return;

    m_roleNames = roleNames;
    emit roleNamesChanged();
}

void SortFilterProxyModel::updateFilterRole()
{
    setFilterRole(roleForName(m_filterRoleName));
    queueInvalidateFilter();
}

void SortFilterProxyModel::updateSortRole()
{
    setSortRole(roleForName(m_sortRoleName));
    queueInvalidate();
}

void SortFilterProxyModel::updateRoles()
{
    updateFilterRole();
    updateSortRole();
}

void SortFilterProxyModel::initRoles()
{
    m_roleNames.clear();
    updateRoleNames();
    updateRoles();
}

void SortFilterProxyModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_UNUSED(topLeft)
    Q_UNUSED(bottomRight)
    invalidate();
}

void SortFilterProxyModel::queueInvalidateProxyRoles()
{
    if (!m_invalidateProxyRolesQueued)
    {
        m_invalidateProxyRolesQueued = true;
        QMetaObject::invokeMethod(this, "invalidateProxyRoles", Qt::QueuedConnection);
    }
}

void SortFilterProxyModel::invalidateProxyRoles()
{
    if (!m_invalidateProxyRolesQueued)
        return;

    m_invalidateProxyRolesQueued = false;
    updateRoleNames();
    invalidate();
}
