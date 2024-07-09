#pragma once

#include <QAbstractListModel>

class CountryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QVariantList countries READ countries WRITE setCountries NOTIFY countriesChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int defaultIndex READ getDefaultIndex NOTIFY countChanged)

public:
    CountryModel(QObject *parent = nullptr);

    enum CountryRoles {
        Iso2Role = Qt::UserRole + 1,
        CodeRole,
    };

    const QVariantList &countries() const noexcept;
    void setCountries(QVariantList countries);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariantMap get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countriesChanged();
    void countChanged();

private:
    int getDefaultIndex() const noexcept;

    QVariantList m_countries;
};

class ChatFolderModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QVariantList chatFolders READ getChatFolders WRITE setChatFolders NOTIFY chatFoldersChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    ChatFolderModel(QObject *parent = nullptr);

    enum ChatFolderRole {
        IdRole = Qt::UserRole + 1,
        IconNameRole,
    };

    QVariantList getChatFolders() const noexcept;
    void setChatFolders(QVariantList value) noexcept;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariantMap get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();
    void chatFoldersChanged();

private:
    QVariantList m_chatFolders;
};
