#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractListModel>

#include <memory>

class Client;

class CountryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int defaultIndex READ getDefaultIndex NOTIFY countChanged)

public:
    CountryModel(QObject *parent = nullptr);

    enum CountryRoles {
        NameRole = Qt::UserRole + 1,
        Iso2Role,
        CodeRole,
    };

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariant get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();

private:
    struct CountryInfo
    {
        QString name;
        QString countryCode;
        QString callingCode;
    };

    void loadData() noexcept;

    int getDefaultIndex() const noexcept;

    Client *m_client{};

    std::vector<std::unique_ptr<CountryInfo>> m_countries;
};
