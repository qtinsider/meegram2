#pragma once

#include <td/telegram/td_api.h>

#include <QAbstractListModel>

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

private slots:
    void retranslateUi();

private:
    QString m_localeString;

    std::vector<const td::td_api::chatFolderInfo *> m_chatFolders;
};
