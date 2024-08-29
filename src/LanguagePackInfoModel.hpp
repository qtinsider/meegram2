#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractListModel>

class Client;

class LanguagePackInfoModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    LanguagePackInfoModel(QObject *parent = nullptr);

    enum LanguagePackInfoRoles {
        IdRole = Qt::UserRole + 1,
        BaseIdRole,
        NameRole,
        NativeNameRole,
        PluralCodeRole,
    };

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariant get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();

private:
    void loadData() noexcept;

    Client *m_client;

    std::vector<td::td_api::object_ptr<td::td_api::languagePackInfo>> m_languagePackInfo;
};
