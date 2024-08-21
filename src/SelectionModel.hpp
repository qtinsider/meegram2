#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractListModel>

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
    int getDefaultIndex() const noexcept;

    std::vector<const td::td_api::countryInfo *> m_countries;
};

class ChatFolderModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    Q_PROPERTY(QString localeString READ localeString WRITE setLocaleString NOTIFY localeStringChanged)

public:
    ChatFolderModel(QObject *parent = nullptr);

    enum ChatFolderRole {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        IconNameRole,
    };

    const QString &localeString() const;
    void setLocaleString(const QString &value);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariant get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();
    void localeStringChanged();

private:
    QString m_localeString;

    std::vector<const td::td_api::chatFolderInfo *> m_chatFolders;
};

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

    void setLanguagePackInfo(td::td_api::object_ptr<td::td_api::localizationTargetInfo> &&value);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariant get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();

private:
    std::vector<const td::td_api::languagePackInfo *> m_languagePackInfo;
};
