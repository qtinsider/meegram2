#pragma once

#include <QAbstractListModel>
#include <QDeclarativeParserStatus>

class Client;
class Locale;
class StorageManager;
class TdManager;

class CountryModel : public QAbstractListModel, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(TdManager *manager READ manager WRITE setManager)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int defaultIndex READ getDefaultIndex NOTIFY countChanged)

public:
    CountryModel(QObject *parent = nullptr);

    enum CountryRoles {
        Iso2Role = Qt::UserRole + 1,
        CodeRole,
    };

    TdManager *manager() const;
    void setManager(TdManager *manager);

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariantMap get(int index) const noexcept;

    int count() const noexcept;

protected:
    void classBegin() override;
    void componentComplete() override;

signals:
    void countChanged();

private:
    int getDefaultIndex() const noexcept;

    TdManager *m_manager;

    QVariantList m_countries;
};

class ChatFolderModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(TdManager *manager READ manager WRITE setManager)
    Q_PROPERTY(TdManager *manager READ manager WRITE setManager)
    Q_PROPERTY(QVariantList chatFolders READ getChatFolders WRITE setChatFolders NOTIFY chatFoldersChanged)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    ChatFolderModel(QObject *parent = nullptr);

    enum ChatFolderRole {
        IdRole = Qt::UserRole + 1,
        IconNameRole,
    };

    TdManager *manager() const;
    void setManager(TdManager *manager);

    QVariantList getChatFolders() const noexcept;
    void setChatFolders(const QVariantList &value) noexcept;

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariantMap get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();
    void chatFoldersChanged();

private:
    Locale *m_locale{nullptr};
    TdManager *m_manager{nullptr};

    QVariantList m_chatFolders;
};
