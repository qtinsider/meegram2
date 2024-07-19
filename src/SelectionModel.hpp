#pragma once

#include <QAbstractListModel>

class Locale;

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

    Q_PROPERTY(QObject *locale READ locale WRITE setLocale)

    Q_PROPERTY(QVariantList chatFolders READ getChatFolders WRITE setChatFolders NOTIFY chatFoldersChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    ChatFolderModel(QObject *parent = nullptr);

    enum ChatFolderRole {
        IdRole = Qt::UserRole + 1,
        IconNameRole,
    };

    QObject *locale() const;
    void setLocale(QObject *locale);

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
    Locale *m_locale;

    QVariantList m_chatFolders;
};

class FlexibleListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QVariantList values READ values WRITE setValues NOTIFY valuesChanged)

public:
    explicit FlexibleListModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    int count() const noexcept;

    QVariantList values() const noexcept;
    void setValues(const QVariantList &values) noexcept;

    Q_INVOKABLE void append(const QVariant &value) noexcept;
    Q_INVOKABLE void remove(int index) noexcept;
    Q_INVOKABLE QVariant get(int index) const noexcept;
    Q_INVOKABLE void clear() noexcept;
    Q_INVOKABLE void replace(const QString &key, const QVariant &value) noexcept;
    Q_INVOKABLE void insert(int index, const QVariant &value) noexcept;

    Q_INVOKABLE void sort(const QString &criteria, bool ascending);

signals:
    void countChanged();
    void valuesChanged();

private:
    QVariantList m_values;
    QHash<int, QByteArray> m_roleNames;
};
