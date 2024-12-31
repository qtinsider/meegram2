#pragma once

#include "Emoji.hpp"

#include <QAbstractListModel>

#include <vector>

class EmojiModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit EmojiModel(QObject *parent = nullptr);

    enum Roles { UnicodeRole = Qt::UserRole + 1, FileNameRole, DescriptionRole, CategoryRole };

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    std::vector<Emoji> m_emojis;
};
