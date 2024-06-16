#pragma once

#include <QDeclarativeParserStatus>
#include <QSortFilterProxyModel>

class SortFilterProxyModel : public QSortFilterProxyModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(QString filterRole READ filterRoleName WRITE setFilterRoleName NOTIFY filterRoleChanged)
    Q_PROPERTY(QString filterFixedString READ filterFixedString WRITE setFilterFixedString)
    Q_PROPERTY(QString filterWildcard READ filterWildcard WRITE setFilterWildcard)
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)
    Q_PROPERTY(QString sortRole READ sortRoleName WRITE setSortRoleName NOTIFY sortRoleChanged)

    Q_PROPERTY(QVariant sourceModel READ sourceModelVariant WRITE setSourceModelVariant NOTIFY sourceModelChanged)

public:
    SortFilterProxyModel(QObject *parent = nullptr);

    int count() const;

    const QString &sortRoleName() const;
    void setSortRoleName(const QString &sortRoleName);

    bool ascendingSortOrder() const;
    void setAscendingSortOrder(bool ascendingSortOrder);

    void classBegin() override;
    void componentComplete() override;

    QVariant sourceData(const QModelIndex &sourceIndex, const QString &roleName) const;
    QVariant sourceData(const QModelIndex &sourceIndex, int role) const;

    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE int roleForName(const QString &roleName) const;

    Q_INVOKABLE QVariantMap get(int row) const;
    Q_INVOKABLE QVariant get(int row, const QString &roleName) const;

    Q_INVOKABLE QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;
    Q_INVOKABLE int mapToSource(int proxyRow) const;
    Q_INVOKABLE QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    Q_INVOKABLE int mapFromSource(int sourceRow) const;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

Q_SIGNALS:
    void countChanged();

    void sortRoleNameChanged();
    void ascendingSortOrderChanged();

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

protected Q_SLOTS:
    void resetInternalData();

private Q_SLOTS:
    void queueInvalidateFilter();
    void invalidateFilter();
    void queueInvalidate();
    void invalidate();
    void updateRoleNames();
    void updateFilterRole();
    void updateSortRole();
    void updateRoles();
    void initRoles();
    void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
    void queueInvalidateProxyRoles();
    void invalidateProxyRoles();

private:
    QVariantMap modelDataMap(const QModelIndex &modelIndex) const;

    QString m_sortRoleName;
    bool m_ascendingSortOrder = true;
    bool m_completed = false;
    QHash<int, QByteArray> m_roleNames;

    bool m_invalidateFilterQueued = false;
    bool m_invalidateQueued = false;
    bool m_invalidateProxyRolesQueued = false;
};
