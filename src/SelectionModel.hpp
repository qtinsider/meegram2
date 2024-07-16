#pragma once

#include <QAbstractListModel>

class FlexibleListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int defaultIndex READ defaultIndex NOTIFY defaultIndexChanged)
    Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY valuesChanged)

public:
    explicit FlexibleListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int count() const noexcept;

    int defaultIndex() const noexcept;

    QVariantList values() const noexcept;
    void setValues(const QVariantList &values) noexcept;

    Q_INVOKABLE void append(const QVariant &value) noexcept;
    Q_INVOKABLE void remove(int index) noexcept;
    Q_INVOKABLE QVariant get(int index) const noexcept;
    Q_INVOKABLE void clear() noexcept;
    Q_INVOKABLE void replace(const QString &key, const QVariant &value) noexcept;
    Q_INVOKABLE void insert(int index, const QVariant &value) noexcept;

    Q_INVOKABLE void sort(const QString &criteria, bool ascending);

    Q_INVOKABLE void setDefaultIndex(const QVariant &criteria) noexcept;

signals:
    void countChanged();
    void defaultIndexChanged();
    void valuesChanged();

private:
    int calculateDefaultIndex(const QVariantMap &criteria) const noexcept;

    int m_defaultIndex;

    QVariantList m_values;
    QHash<int, QByteArray> m_roleNames;
};
