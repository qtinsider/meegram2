#include "SelectionModel.hpp"

#include "Localization.hpp"
#include "Serialize.hpp"
#include "TdApi.hpp"

#include <QDebug>

#include <algorithm>

CountryModel::CountryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "name");
    roles.insert(Iso2Role, "iso2");
    roles.insert(CodeRole, "code");

    setRoleNames(roles);

    QVariantMap request;
    request.insert("@type", "getCountries");

    TdApi::getInstance().sendRequest(request, [this](const auto &value) {
        if (value.value("@type").toByteArray() == "countries")
        {
            beginInsertRows(QModelIndex(), rowCount(), value.count() - 1);
            std::ranges::copy(value.value("countries").toList(), std::back_inserter(m_countries));
            endInsertRows();

            emit countChanged();
        }
    });
}

int CountryModel::rowCount(const QModelIndex &) const
{
    return m_countries.count();
}

QVariant CountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_countries.isEmpty())
        return QVariant();

    switch (const auto &countryInfo = m_countries.at(index.row()).toMap(); role)
    {
        case Qt::DisplayRole:
            return countryInfo.value("name").toString();
        case Iso2Role:
            return countryInfo.value("country_code").toString();
        case CodeRole:
            return countryInfo.value("calling_codes").toStringList();
    }

    return QVariant();
}

QVariantMap CountryModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);

    QVariantMap result;
    result.insert("name", data(modelIndex, Qt::DisplayRole));
    result.insert("iso2", data(modelIndex, Iso2Role));
    result.insert("code", data(modelIndex, CodeRole));

    return result;
}

int CountryModel::count() const noexcept
{
    return m_countries.count();
}

int CountryModel::getDefaultIndex() const noexcept
{
    // TODO(strawberry): refactor
    auto it = std::ranges::find_if(m_countries, [](const auto &value) {
        return value.toMap().value("country_code").toString().compare("NG", Qt::CaseInsensitive) == 0;
    });

    if (it != m_countries.end())
    {
        return int(std::distance(m_countries.begin(), it));
    }

    return {};
}

ChatFilterModel::ChatFilterModel(QObject *parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(IdRole, "id");
    roles.insert(Qt::DisplayRole, "name");
    roles.insert(IconNameRole, "iconName");

    setRoleNames(roles);

    connect(&TdApi::getInstance(), SIGNAL(updateChatFilters(const QVariantList &)), SLOT(handleChatFilters(const QVariantList &)));
}

int ChatFilterModel::rowCount(const QModelIndex &index) const
{
    return m_chatFilters.count();
}

QVariant ChatFilterModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (auto chatFilter = m_chatFilters.value(index.row()).toMap(); role)
    {
        case IdRole:
            return chatFilter.value("id").toInt();
        case Qt::DisplayRole:
            return chatFilter.value("title").toString();
        case IconNameRole:
            return chatFilter.value("icon_name").toString();
    }

    return QVariant();
}

QVariantMap ChatFilterModel::get(int index) const noexcept
{
    QModelIndex modelIndex = createIndex(index, 0);

    QVariantMap result;
    result.insert("id", data(modelIndex, IdRole));
    result.insert("name", data(modelIndex, Qt::DisplayRole));  // title
    result.insert("iconName", data(modelIndex, IconNameRole));

    return result;
}

int ChatFilterModel::count() const noexcept
{
    return m_chatFilters.count();
}

void ChatFilterModel::handleChatFilters(const QVariantList &chatFilters)
{
    QVariantMap chatFilter;
    chatFilter.insert("id", 0);
    chatFilter.insert("title", Localization::getInstance().getString("FilterAllChats"));

    beginResetModel();
    m_chatFilters.clear();
    m_chatFilters.append(chatFilter);
    std::ranges::copy(chatFilters, std::back_inserter(m_chatFilters));
    endResetModel();

    emit countChanged();
}
