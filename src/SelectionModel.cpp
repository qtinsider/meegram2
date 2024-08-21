#include "SelectionModel.hpp"

#include "StorageManager.hpp"

#include <QDebug>
#include <QStringList>
#include <algorithm>

CountryModel::CountryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(Iso2Role, "iso2");
    roles.insert(CodeRole, "code");

    setRoleNames(roles);

    beginResetModel();
    m_countries = StorageManager::instance().countries();
    endResetModel();

    emit countChanged();
}

int CountryModel::rowCount(const QModelIndex &) const
{
    return m_countries.size();
}

QVariant CountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_countries.empty())
        return QVariant();

    const auto &countryInfo = m_countries.at(index.row());
    switch (role)
    {
        case NameRole:
            return QString::fromStdString(countryInfo->name_);
        case Iso2Role:
            return QString::fromStdString(countryInfo->country_code_);
        case CodeRole: {
            QStringList result;

            for (const auto &value : countryInfo->calling_codes_)
            {
                result.append(QString::fromStdString(value));
            }

            return result;
        }
    }

    return QVariant();
}

QVariant CountryModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);

    QVariantMap result;
    result.insert("name", data(modelIndex, NameRole));
    result.insert("iso2", data(modelIndex, Iso2Role));
    result.insert("code", data(modelIndex, CodeRole));

    return result;
}

int CountryModel::count() const noexcept
{
    return m_countries.size();
}

int CountryModel::getDefaultIndex() const noexcept
{
    // TODO(strawberry): refactor
    auto it = std::ranges::find_if(m_countries,
                                   [](const auto &value) { return QString::fromStdString(value->country_code_).compare("NG", Qt::CaseInsensitive) == 0; });

    if (it != m_countries.end())
    {
        return int(std::distance(m_countries.begin(), it));
    }

    return {};
}

ChatFolderModel::ChatFolderModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(TitleRole, "name");

    setRoleNames(roles);

    m_chatFolders = StorageManager::instance().chatFolders();
    setLocaleString(m_localeString);  // Assuming `m_localeString` is still valid here
}

const QString &ChatFolderModel::localeString() const
{
    return m_localeString;
}

void ChatFolderModel::setLocaleString(const QString &value)
{
    auto folder = td::td_api::make_object<td::td_api::chatFolderInfo>();
    folder->id_ = 0;
    folder->title_ = value.toStdString();

    auto it = std::ranges::find_if(m_chatFolders, [](const auto &folder) { return folder->id_ == 0; });

    if (it != m_chatFolders.end())
    {
        // Delete the old pointer to prevent memory leak
        delete *it;
        // Replace the existing folder with id 0
        *it = folder.release();
    }
    else
    {
        // Insert the new folder at the beginning
        m_chatFolders.emplace(m_chatFolders.begin(), folder.release());
    }

    emit countChanged();
}

int ChatFolderModel::rowCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return m_chatFolders.size();
}

QVariant ChatFolderModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    const auto &chatFolder = m_chatFolders.at(index.row());
    switch (role)
    {
        case IdRole:
            return chatFolder->id_;
        case TitleRole:
            return QString::fromStdString(chatFolder->title_);
        default:
            return QVariant();
    }
}

QVariant ChatFolderModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);
    QVariantMap result;
    result.insert("id", data(modelIndex, IdRole));
    result.insert("name", data(modelIndex, TitleRole));  // title
    return result;
}

int ChatFolderModel::count() const noexcept
{
    return m_chatFolders.size();
}

LanguagePackInfoModel::LanguagePackInfoModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(BaseIdRole, "baseId");
    roles.insert(NameRole, "name");
    roles.insert(NativeNameRole, "nativeName");
    roles.insert(PluralCodeRole, "pluralCode");

    setRoleNames(roles);

    beginResetModel();
    m_languagePackInfo = StorageManager::instance().languagePackInfo();
    endResetModel();
}

int LanguagePackInfoModel::rowCount(const QModelIndex &) const
{
    return m_languagePackInfo.size();
}

QVariant LanguagePackInfoModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_languagePackInfo.empty())
        return QVariant();

    const auto &value = m_languagePackInfo.at(index.row());
    switch (role)
    {
        case IdRole:
            return QString::fromStdString(value->id_);
        case BaseIdRole:
            return QString::fromStdString(value->base_language_pack_id_);
        case NameRole:
            return QString::fromStdString(value->name_);
        case NativeNameRole:
            return QString::fromStdString(value->native_name_);
        case PluralCodeRole:
            return QString::fromStdString(value->plural_code_);
    }

    return QVariant();
}

QVariant LanguagePackInfoModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);

    QVariantMap result;
    result.insert("id", data(modelIndex, IdRole));
    result.insert("baseId", data(modelIndex, BaseIdRole));
    result.insert("name", data(modelIndex, NameRole));
    result.insert("nativeName", data(modelIndex, NativeNameRole));
    result.insert("pluralCode", data(modelIndex, PluralCodeRole));

    return result;
}

int LanguagePackInfoModel::count() const noexcept
{
    return m_languagePackInfo.size();
}
