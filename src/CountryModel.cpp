#include "CountryModel.hpp"

#include "Settings.hpp"
#include "StorageManager.hpp"

#include <QDebug>

#include <algorithm>

CountryModel::CountryModel(QObject *parent)
    : QAbstractListModel(parent)
    , m_client(StorageManager::instance().client())
{
    setRoleNames(roleNames());

    connect(this, SIGNAL(phoneNumberPrefixChanged()), this, SLOT(updatePhoneInfoFromPrefix()));

    fetchAndLoadCountries();
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
            return countryInfo->name;
        case Iso2Role:
            return countryInfo->countryCode;
        case CodeRole:
            return countryInfo->callingCode;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> CountryModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles.insert(NameRole, "name");
    roles.insert(Iso2Role, "iso2");
    roles.insert(CodeRole, "code");

    return roles;
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

int CountryModel::selectedIndex() const noexcept
{
    return m_selectedIndex;
}

void CountryModel::setSelectedIndex(int value) noexcept
{
    if (m_selectedIndex != value)
    {
        m_selectedIndex = value;
        emit selectedIndexChanged();
    }
}

QString CountryModel::phoneNumberPrefix() const noexcept
{
    return m_phoneNumberPrefix;
}

void CountryModel::setPhoneNumberPrefix(const QString &value) noexcept
{
    if (m_phoneNumberPrefix != value)
    {
        m_phoneNumberPrefix = value;
        emit phoneNumberPrefixChanged();
    }
}

QString CountryModel::callingCode() const noexcept
{
    return m_callingCode;
}

QString CountryModel::formattedPhoneNumber() const noexcept
{
    return m_formattedPhoneNumber;
}

void CountryModel::updatePhoneInfoFromPrefix()
{
    m_phoneNumberPrefix.remove(QRegExp("\\D"));  // Strip all non-digit characters

    const auto languageCode = Settings::instance().languagePackId().toStdString();
    const auto phoneNumberPrefix = m_phoneNumberPrefix.toStdString();

    auto info = td::ClientManager::execute(td::td_api::make_object<td::td_api::getPhoneNumberInfoSync>(languageCode, phoneNumberPrefix));

    if (info->get_id() == td::td_api::error::ID)
    {
        qDebug() << "Error:" << QString::fromStdString(td::td_api::move_object_as<td::td_api::error>(info)->message_);
        return;  // Early exit on error
    }

    QString countryCode;

    const auto phoneInfo = td::td_api::move_object_as<td::td_api::phoneNumberInfo>(info);

    const auto countryCallingCode = QString::fromStdString(phoneInfo->country_calling_code_);
    const auto formattedPhoneNumber = QString::fromStdString(phoneInfo->formatted_phone_number_);

    if (phoneInfo->country_)
    {
        const auto &country = phoneInfo->country_;
        countryCode = QString::fromStdString(country->country_code_);  // Direct assignment from country code
    }

    if (auto it = std::ranges::find(m_countries, countryCode, &CountryInfo::countryCode); it != m_countries.end())
    {
        setSelectedIndex(static_cast<int>(std::distance(m_countries.begin(), it)));
    }
    else
    {
        setSelectedIndex(-1);
    }

    if (m_callingCode != countryCallingCode)
    {
        m_callingCode = countryCallingCode;
        emit callingCodeChanged();
    }

    if (m_formattedPhoneNumber != formattedPhoneNumber)
    {
        m_formattedPhoneNumber = formattedPhoneNumber;
        emit formattedPhoneNumberChanged();
    }
}

void CountryModel::fetchAndLoadCountries() noexcept
{
    m_client->send(td::td_api::make_object<td::td_api::getCountries>(), [this](auto response) {
        if (response->get_id() == td::td_api::countries::ID)
        {
            beginResetModel();

            auto countries = std::move(td::td_api::move_object_as<td::td_api::countries>(response)->countries_);

            // Clear existing countries before loading new data
            m_countries.clear();
            m_countries.reserve(countries.size() * 2);  // Estimate number of entries

            std::ranges::sort(countries, std::ranges::less{}, &td::td_api::countryInfo::name_);

            for (const auto &country : countries)
            {
                const auto name = QString::fromStdString(country->name_);
                const auto countryCode = QString::fromStdString(country->country_code_);

                for (const auto &code : country->calling_codes_)
                {
                    m_countries.emplace_back(std::make_unique<CountryInfo>(name, countryCode, QString::fromStdString(code)));
                }
            }

            endResetModel();

            emit countChanged();
        }
    });
}
