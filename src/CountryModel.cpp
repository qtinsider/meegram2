#include "CountryModel.hpp"

#include "StorageManager.hpp"

#include <QStringList>
#include <algorithm>

CountryModel::CountryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_client(StorageManager::instance().client())
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(Iso2Role, "iso2");
    roles.insert(CodeRole, "code");

    setRoleNames(roles);

    loadData();
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

void CountryModel::loadData() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::getCountries>(), [this](auto response) {
        if (response->get_id() == td::td_api::countries::ID)
        {
            beginResetModel();
            m_countries = std::move(td::move_tl_object_as<td::td_api::countries>(response)->countries_);
            endResetModel();

            emit countChanged();
        }
    });
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
