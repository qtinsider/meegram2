#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractListModel>

#include <memory>

class Client;

class CountryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(int selectedIndex READ selectedIndex NOTIFY selectedIndexChanged)

    Q_PROPERTY(QString phoneNumberPrefix READ phoneNumberPrefix WRITE setPhoneNumberPrefix NOTIFY phoneNumberPrefixChanged)

    Q_PROPERTY(QString callingCode READ callingCode NOTIFY callingCodeChanged)
    Q_PROPERTY(QString formattedPhoneNumber READ formattedPhoneNumber NOTIFY formattedPhoneNumberChanged)

public:
    CountryModel(QObject *parent = nullptr);

    enum CountryRoles {
        NameRole = Qt::UserRole + 1,
        Iso2Role,
        CodeRole,
    };

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE QVariant get(int index) const noexcept;

    int count() const noexcept;

    int selectedIndex() const noexcept;
    void setSelectedIndex(int value) noexcept;

    QString phoneNumberPrefix() const noexcept;
    void setPhoneNumberPrefix(const QString &value) noexcept;

    QString callingCode() const noexcept;
    QString formattedPhoneNumber() const noexcept;

signals:
    void countChanged();

    void selectedIndexChanged();

    void phoneNumberPrefixChanged();

    void callingCodeChanged();
    void formattedPhoneNumberChanged();

private slots:
    void updatePhoneInfoFromPrefix();

private:
    void fetchAndLoadCountries() noexcept;

    struct CountryInfo
    {
        QString name;
        QString countryCode;
        QString callingCode;
    };

    int m_selectedIndex{-1};

    QString m_phoneNumberPrefix;
    QString m_callingCode, m_formattedPhoneNumber;

    std::shared_ptr<Client> m_client;

    std::vector<std::unique_ptr<CountryInfo>> m_countries;
};
