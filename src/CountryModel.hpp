#pragma once

#include <QAbstractListModel>

class CountryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count CONSTANT)
    Q_PROPERTY(int defaultIndex READ getDefaultIndex CONSTANT);

public:
    CountryModel(QObject *parent = nullptr);

    enum CountryRoles {
        Iso2Role = Qt::UserRole + 1,
        CodeRole,
    };

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariantMap get(int index) const noexcept;

    [[nodiscard]] int count() const noexcept;

private:
    [[nodiscard]] int getDefaultIndex() const noexcept;
};
