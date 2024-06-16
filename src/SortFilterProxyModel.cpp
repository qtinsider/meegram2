#include "SortFilterProxyModel.hpp"

#include <algorithm>

SortFilterProxyModel::SortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    ,
{
    connect(this, &QAbstractItemModel::modelReset, this, &SortFilterProxyModel::updateRoles);
    connect(this, &QAbstractItemModel::rowsInserted, this, &SortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::rowsRemoved, this, &SortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::modelReset, this, &SortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::layoutChanged, this, &SortFilterProxyModel::countChanged);
    connect(this, &QAbstractItemModel::dataChanged, this, &SortFilterProxyModel::onDataChanged);
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
    Q_EMIT filterRoleNameChanged();
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
    Q_EMIT filterPatternChanged();
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
    Q_EMIT filterPatternSyntaxChanged();
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
    Q_EMIT filterValueChanged();
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
    Q_EMIT sortRoleNameChanged();
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
    Q_EMIT ascendingSortOrderChanged();
    queueInvalidate();
}

void SortFilterProxyModel::classBegin()
{
}

void SortFilterProxyModel::componentComplete()
{
    m_completed = true;

    for (const auto &filter : m_filters)
        filter->proxyModelCompleted(*this);
    for (const auto &sorter : m_sorters)
        sorter->proxyModelCompleted(*this);
    for (const auto &proxyRole : m_proxyRoles)
        proxyRole->proxyModelCompleted(*this);

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
    QPair<ProxyRole *, QString> proxyRolePair = m_proxyRoleMap[role];
    if (ProxyRole *proxyRole = proxyRolePair.first)
        return proxyRole->roleData(sourceIndex, *this, proxyRolePair.second);
    else
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
    QModelIndex modelIndex = index(row, 0);
    QHash<int, QByteArray> roles = roleNames();
    for (QHash<int, QByteArray>::const_iterator it = roles.begin(); it != roles.end(); ++it)
        map.insert(it.value(), data(modelIndex, it.key()));
    return map;
}

QVariant SortFilterProxyModel::get(int row, const QString &roleName) const
{
    return data(index(row, 0), roleForName(roleName));
}

QModelIndex SortFilterProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    return QSortFilterProxyModel::mapToSource(proxyIndex);
}

int SortFilterProxyModel::mapToSource(int proxyRow) const
{
    QModelIndex proxyIndex = index(proxyRow, 0);
    QModelIndex sourceIndex = mapToSource(proxyIndex);
    return sourceIndex.isValid() ? sourceIndex.row() : -1;
}

QModelIndex SortFilterProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    return QSortFilterProxyModel::mapFromSource(sourceIndex);
}

int SortFilterProxyModel::mapFromSource(int sourceRow) const
{
    QModelIndex proxyIndex;
    if (QAbstractItemModel *source = sourceModel())
    {
        QModelIndex sourceIndex = source->index(sourceRow, 0);
        proxyIndex = mapFromSource(sourceIndex);
    }
    return proxyIndex.isValid() ? proxyIndex.row() : -1;
}

bool SortFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    if (!m_completed)
        return true;
    QModelIndex sourceIndex = sourceModel()->index(source_row, 0, source_parent);
    bool valueAccepted = !m_filterValue.isValid() || (m_filterValue == sourceModel()->data(sourceIndex, filterRole()));
    bool baseAcceptsRow = valueAccepted && QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    baseAcceptsRow = baseAcceptsRow && std::all_of(m_filters.begin(), m_filters.end(), [=, &source_parent](Filter *filter) {
                         return filter->filterAcceptsRow(sourceIndex, *this);
                     });
    return baseAcceptsRow;
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
        auto sortedSorters = m_sorters;
        std::stable_sort(sortedSorters.begin(), sortedSorters.end(), [](Sorter *a, Sorter *b) { return a->priority() > b->priority(); });
        for (auto sorter : sortedSorters)
        {
            if (sorter->enabled())
            {
                int comparison = sorter->compareRows(source_left, source_right, *this);
                if (comparison != 0)
                    return comparison < 0;
            }
        }
    }
    return source_left.row() < source_right.row();
}

void SortFilterProxyModel::resetInternalData()
{
    QSortFilterProxyModel::resetInternalData();
    updateRoleNames();
}

void SortFilterProxyModel::setSourceModel(QAbstractItemModel *sourceModel)
{
    if (sourceModel && sourceModel->roleNames().isEmpty())
    {  // workaround for when a model has no roles and roles are added when the model is populated (ListModel)
        // QTBUG-57971

        connect(sourceModel, SIGNAL(rowsInserted(QModelIndex, int, int)), this, SIGNAL(initRoles()));
    }
    QSortFilterProxyModel::setSourceModel(sourceModel);
}

void SortFilterProxyModel::queueInvalidateFilter()
{
    if (0)
    {
        if (!m_invalidateFilterQueued && !m_invalidateQueued)
        {
            m_invalidateFilterQueued = true;
            QMetaObject::invokeMethod(this, "invalidateFilter", Qt::QueuedConnection);
        }
    }
    else
    {
        invalidateFilter();
    }
}

void SortFilterProxyModel::invalidateFilter()
{
    m_invalidateFilterQueued = false;
    if (m_completed && !m_invalidateQueued)
        QSortFilterProxyModel::invalidateFilter();
}

void SortFilterProxyModel::queueInvalidate()
{
    if (m_delayed)
    {
        if (!m_invalidateQueued)
        {
            m_invalidateQueued = true;
            QMetaObject::invokeMethod(this, "invalidate", Qt::QueuedConnection);
        }
    }
    else
    {
        invalidate();
    }
}

void SortFilterProxyModel::invalidate()
{
    m_invalidateQueued = false;
    if (m_completed)
        QSortFilterProxyModel::invalidate();
}

void SortFilterProxyModel::updateRoleNames()
{
    if (!sourceModel())
        return;
    m_roleNames = sourceModel()->roleNames();

    auto roles = m_roleNames.keys();
    auto maxIt = std::max_element(roles.begin(), roles.end());
    int maxRole = maxIt != roles.end() ? *maxIt : -1;
}

void SortFilterProxyModel::updateFilterRole()
{
    QList<int> filterRoles = roleNames().keys(m_filterRoleName.toUtf8());
    if (!filterRoles.empty())
    {
        setFilterRole(filterRoles.first());
    }
}

void SortFilterProxyModel::updateSortRole()
{
    QList<int> sortRoles = roleNames().keys(m_sortRoleName.toUtf8());
    if (!sortRoles.empty())
    {
        setSortRole(sortRoles.first());
        queueInvalidate();
    }
}

void SortFilterProxyModel::updateRoles()
{
    updateFilterRole();
    updateSortRole();
}

void SortFilterProxyModel::initRoles()
{
    disconnect(sourceModel(), &QAbstractItemModel::rowsInserted, this, &SortFilterProxyModel::initRoles);
    resetInternalData();
    updateRoles();
}

void SortFilterProxyModel::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight)
{
    Q_EMIT dataChanged(topLeft, bottomRight);
}

void SortFilterProxyModel::queueInvalidateProxyRoles()
{
    queueInvalidate();
    if (m_delayed)
    {
        if (!m_invalidateProxyRolesQueued)
        {
            m_invalidateProxyRolesQueued = true;
            QMetaObject::invokeMethod(this, "invalidateProxyRoles", Qt::QueuedConnection);
        }
    }
    else
    {
        invalidateProxyRoles();
    }
}

void SortFilterProxyModel::invalidateProxyRoles()
{
    m_invalidateProxyRolesQueued = false;
    if (m_completed)
        Q_EMIT dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
}

QVariantMap SortFilterProxyModel::modelDataMap(const QModelIndex &modelIndex) const
{
    QVariantMap map;
    QHash<int, QByteArray> roles = roleNames();
    for (QHash<int, QByteArray>::const_iterator it = roles.begin(); it != roles.end(); ++it)
        map.insert(it.value(), sourceModel()->data(modelIndex, it.key()));
    return map;
}
