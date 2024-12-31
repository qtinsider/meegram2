#include "LanguagePackInfoModel.hpp"

#include "StorageManager.hpp"

#include <QDebug>
#include <QStringList>
#include <algorithm>

LanguagePackInfoModel::LanguagePackInfoModel(std::shared_ptr<Client> client, QObject *parent)
    : QAbstractListModel(parent)
    , m_client(std::move(client))
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(BaseIdRole, "baseId");
    roles.insert(NameRole, "name");
    roles.insert(NativeNameRole, "nativeName");
    roles.insert(PluralCodeRole, "pluralCode");

    setRoleNames(roles);

    loadLocalizationData();
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

void LanguagePackInfoModel::loadLocalizationData() noexcept
{
    auto request = td::td_api::make_object<td::td_api::getLocalizationTargetInfo>();
    request->only_local_ = true;

    qDebug() << "Loading localization target info...";

    m_client->send(std::move(request), [this](auto &&response) {
        if (response->get_id() == td::td_api::localizationTargetInfo::ID)
        {
            auto localizationInfo = td::td_api::move_object_as<td::td_api::localizationTargetInfo>(response);
            if (!localizationInfo)
            {
                qDebug() << "Failed to retrieve localization info";
                return;
            }

            auto languagePacks = std::move(localizationInfo->language_packs_);

            if (languagePacks.empty())
            {
                qDebug() << "No language packs found";
                return;
            }

            beginResetModel();
            m_languagePackInfo = std::move(languagePacks);
            endResetModel();

            emit countChanged();
        }
    });
}
