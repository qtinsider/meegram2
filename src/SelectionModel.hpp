#pragma once

#include <QAbstractListModel>

class CountryModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int defaultIndex READ getDefaultIndex NOTIFY countChanged);

public:
    CountryModel(QObject *parent = nullptr);

    enum CountryRoles {
        Iso2Role = Qt::UserRole + 1,
        CodeRole,
    };

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariantMap get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();

private:
    int getDefaultIndex() const noexcept;

    QVariantList m_countries;
};

class ChatFilterModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    ChatFilterModel(QObject *parent = nullptr);

    enum ChatFilterRole {
        IdRole = Qt::UserRole + 1,
        IconNameRole,
    };

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariantMap get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();

private slots:
    void handleChatFilters(const QVariantList &chatFilters);

private:
    Q_DISABLE_COPY(ChatFilterModel)

    QVariantList m_chatFilters;
};
