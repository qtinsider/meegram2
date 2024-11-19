#pragma once

#include "ChatFolderInfo.hpp"

#include <QAbstractListModel>

#include <memory>
#include <vector>

class ChatFolderModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    ChatFolderModel(QObject *parent = nullptr);

    enum ChatFolderRole {
        IdRole = Qt::UserRole + 1,
        TitleRole
    };

    int rowCount(const QModelIndex &index = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Q_INVOKABLE QVariant get(int index) const noexcept;

    int count() const noexcept;

signals:
    void countChanged();

private:
    std::vector<std::weak_ptr<ChatFolderInfo>> m_chatFolders;
};
