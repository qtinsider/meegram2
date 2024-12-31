#include "EmojiModel.hpp"

EmojiModel::EmojiModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_emojis(Emoji::emojis().begin(), Emoji::emojis().end())
{
    QHash<int, QByteArray> roles;
    roles.insert(UnicodeRole, "unicode");
    roles.insert(FileNameRole, "filename");
    roles.insert(DescriptionRole, "description");
    roles.insert(CategoryRole, "category");

    setRoleNames(roles);
}

int EmojiModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return static_cast<int>(m_emojis.size());
}

QVariant EmojiModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= rowCount())
        return {};

    const auto &emoji = m_emojis[index.row()];

    switch (role)
    {
        case UnicodeRole:
            return emoji.unicode();
        case FileNameRole:
            return emoji.filename();
        case DescriptionRole:
            return emoji.description();
        case CategoryRole:
            return emoji.category();
        default:
            return {};
    }
}
