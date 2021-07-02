#include "SelectionModel.hpp"

#include "TdApi.hpp"

#include <QSystemNetworkInfo>

#include <algorithm>
#include <array>

struct CountryInfo
{
    const char *name = nullptr;
    const char *iso2 = nullptr;
    const char *code = nullptr;
};

namespace {

inline constexpr std::array<CountryInfo, 231> countries = {{
    {"Afghanistan", "AF", "93"},
    {"Albania", "AL", "355"},
    {"Algeria", "DZ", "213"},
    {"American Samoa", "AS", "1684"},
    {"Andorra", "AD", "376"},
    {"Angola", "AO", "244"},
    {"Anguilla", "AI", "1264"},
    {"Antigua & Barbuda", "AG", "1268"},
    {"Argentina", "AR", "54"},
    {"Armenia", "AM", "374"},
    {"Aruba", "AW", "297"},
    {"Australia", "AU", "61"},
    {"Austria", "AT", "43"},
    {"Azerbaijan", "AZ", "994"},
    {"Bahamas", "BS", "1242"},
    {"Bahrain", "BH", "973"},
    {"Bangladesh", "BD", "880"},
    {"Barbados", "BB", "1246"},
    {"Belarus", "BY", "375"},
    {"Belgium", "BE", "32"},
    {"Belize", "BZ", "501"},
    {"Benin", "BJ", "229"},
    {"Bermuda", "BM", "1441"},
    {"Bhutan", "BT", "975"},
    {"Bolivia", "BO", "591"},
    {"Bonaire, Sint Eustatius & Saba", "BQ", "599"},
    {"Bosnia & Herzegovina", "BA", "387"},
    {"Botswana", "BW", "267"},
    {"Brazil", "BR", "55"},
    {"British Virgin Islands", "VG", "1284"},
    {"Brunei Darussalam", "BN", "673"},
    {"Bulgaria", "BG", "359"},
    {"Burkina Faso", "BF", "226"},
    {"Burundi", "BI", "257"},
    {"Cambodia", "KH", "855"},
    {"Cameroon", "CM", "237"},
    {"Canada", "CA", "1"},
    {"Cape Verde", "CV", "238"},
    {"Cayman Islands", "KY", "1345"},
    {"Central African Republic", "CF", "236"},
    {"Chad", "TD", "235"},
    {"Chile", "CL", "56"},
    {"China", "CN", "86"},
    {"Colombia", "CO", "57"},
    {"Comoros", "KM", "269"},
    {"Congo", "CG", "242"},
    {"Congo, Democratic Republic", "CD", "243"},
    {"Cook Islands", "CK", "682"},
    {"Costa Rica", "CR", "506"},
    {"Croatia", "HR", "385"},
    {"Cuba", "CU", "53"},
    {"Curaçao", "CW", "599"},
    {"Cyprus", "CY", "357"},
    {"Czech Republic", "CZ", "420"},
    {"Côte d`Ivoire", "CI", "225"},
    {"Denmark", "DK", "45"},
    {"Diego Garcia", "IO", "246"},
    {"Djibouti", "DJ", "253"},
    {"Dominica", "DM", "1767"},
    {"Dominican Republic", "DO", "1"},
    {"Ecuador", "EC", "593"},
    {"Egypt", "EG", "20"},
    {"El Salvador", "SV", "503"},
    {"Equatorial Guinea", "GQ", "240"},
    {"Eritrea", "ER", "291"},
    {"Estonia", "EE", "372"},
    {"Ethiopia", "ET", "251"},
    {"Falkland Islands", "FK", "500"},
    {"Faroe Islands", "FO", "298"},
    {"Fiji", "FJ", "679"},
    {"Finland", "FI", "358"},
    {"France", "FR", "33"},
    {"French Guiana", "GF", "594"},
    {"French Polynesia", "PF", "689"},
    {"Gabon", "GA", "241"},
    {"Gambia", "GM", "220"},
    {"Georgia", "GE", "995"},
    {"Germany", "DE", "49"},
    {"Ghana", "GH", "233"},
    {"Gibraltar", "GI", "350"},
    {"Greece", "GR", "30"},
    {"Greenland", "GL", "299"},
    {"Grenada", "GD", "1473"},
    {"Guadeloupe", "GP", "590"},
    {"Guam", "GU", "1671"},
    {"Guatemala", "GT", "502"},
    {"Guinea", "GN", "224"},
    {"Guinea-Bissau", "GW", "245"},
    {"Guyana", "GY", "592"},
    {"Haiti", "HT", "509"},
    {"Honduras", "HN", "504"},
    {"Hong Kong", "HK", "852"},
    {"Hungary", "HU", "36"},
    {"Iceland", "IS", "354"},
    {"India", "IN", "91"},
    {"Indonesia", "ID", "62"},
    {"Iran", "IR", "98"},
    {"Iraq", "IQ", "964"},
    {"Ireland", "IE", "353"},
    {"Israel", "IL", "972"},
    {"Italy", "IT", "39"},
    {"Jamaica", "JM", "1876"},
    {"Japan", "JP", "81"},
    {"Jordan", "JO", "962"},
    {"Kazakhstan", "KZ", "7"},
    {"Kenya", "KE", "254"},
    {"Kiribati", "KI", "686"},
    {"Kuwait", "KW", "965"},
    {"Kyrgyzstan", "KG", "996"},
    {"Laos", "LA", "856"},
    {"Latvia", "LV", "371"},
    {"Lebanon", "LB", "961"},
    {"Lesotho", "LS", "266"},
    {"Liberia", "LR", "231"},
    {"Libya", "LY", "218"},
    {"Liechtenstein", "LI", "423"},
    {"Lithuania", "LT", "370"},
    {"Luxembourg", "LU", "352"},
    {"Macau", "MO", "853"},
    {"Macedonia", "MK", "389"},
    {"Madagascar", "MG", "261"},
    {"Malawi", "MW", "265"},
    {"Malaysia", "MY", "60"},
    {"Maldives", "MV", "960"},
    {"Mali", "ML", "223"},
    {"Malta", "MT", "356"},
    {"Marshall Islands", "MH", "692"},
    {"Martinique", "MQ", "596"},
    {"Mauritania", "MR", "222"},
    {"Mauritius", "MU", "230"},
    {"Mexico", "MX", "52"},
    {"Micronesia", "FM", "691"},
    {"Moldova", "MD", "373"},
    {"Monaco", "MC", "377"},
    {"Mongolia", "MN", "976"},
    {"Montenegro", "ME", "382"},
    {"Montserrat", "MS", "1664"},
    {"Morocco", "MA", "212"},
    {"Mozambique", "MZ", "258"},
    {"Myanmar", "MM", "95"},
    {"Namibia", "NA", "264"},
    {"Nauru", "NR", "674"},
    {"Nepal", "NP", "977"},
    {"Netherlands", "NL", "31"},
    {"New Caledonia", "NC", "687"},
    {"New Zealand", "NZ", "64"},
    {"Nicaragua", "NI", "505"},
    {"Niger", "NE", "227"},
    {"Nigeria", "NG", "234"},
    {"Niue", "NU", "683"},
    {"Norfolk Island", "NF", "672"},
    {"North Korea", "KP", "850"},
    {"Northern Mariana Islands", "MP", "1670"},
    {"Norway", "NO", "47"},
    {"Oman", "OM", "968"},
    {"Pakistan", "PK", "92"},
    {"Palau", "PW", "680"},
    {"Palestine", "PS", "970"},
    {"Panama", "PA", "507"},
    {"Papua New Guinea", "PG", "675"},
    {"Paraguay", "PY", "595"},
    {"Peru", "PE", "51"},
    {"Philippines", "PH", "63"},
    {"Poland", "PL", "48"},
    {"Portugal", "PT", "351"},
    {"Puerto Rico", "PR", "1"},
    {"Qatar", "QA", "974"},
    {"Romania", "RO", "40"},
    {"Russian Federation", "RU", "7"},
    {"Rwanda", "RW", "250"},
    {"Réunion", "RE", "262"},
    {"Saint Helena", "SH", "247"},
    {"Saint Helena", "SH2", "290"},
    {"Saint Kitts & Nevis", "KN", "1869"},
    {"Saint Lucia", "LC", "1758"},
    {"Saint Pierre & Miquelon", "PM", "508"},
    {"Saint Vincent & the Grenadines", "VC", "1784"},
    {"Samoa", "WS", "685"},
    {"San Marino", "SM", "378"},
    {"Saudi Arabia", "SA", "966"},
    {"Senegal", "SN", "221"},
    {"Serbia", "RS", "381"},
    {"Seychelles", "SC", "248"},
    {"Sierra Leone", "SL", "232"},
    {"Singapore", "SG", "65"},
    {"Sint Maarten", "SX", "1721"},
    {"Slovakia", "SK", "421"},
    {"Slovenia", "SI", "386"},
    {"Solomon Islands", "SB", "677"},
    {"Somalia", "SO", "252"},
    {"South Africa", "ZA", "27"},
    {"South Korea", "KR", "82"},
    {"South Sudan", "SS", "211"},
    {"Spain", "ES", "34"},
    {"Sri Lanka", "LK", "94"},
    {"Sudan", "SD", "249"},
    {"Suriname", "SR", "597"},
    {"Swaziland", "SZ", "268"},
    {"Sweden", "SE", "46"},
    {"Switzerland", "CH", "41"},
    {"Syrian Arab Republic", "SY", "963"},
    {"São Tomé & Príncipe", "ST", "239"},
    {"Taiwan", "TW", "886"},
    {"Tajikistan", "TJ", "992"},
    {"Tanzania", "TZ", "255"},
    {"Thailand", "TH", "66"},
    {"Timor-Leste", "TL", "670"},
    {"Togo", "TG", "228"},
    {"Tokelau", "TK", "690"},
    {"Tonga", "TO", "676"},
    {"Trinidad & Tobago", "TT", "1868"},
    {"Tunisia", "TN", "216"},
    {"Turkey", "TR", "90"},
    {"Turkmenistan", "TM", "993"},
    {"Turks & Caicos Islands", "TC", "1649"},
    {"Tuvalu", "TV", "688"},
    {"US Virgin Islands", "VI", "1340"},
    {"United States of America", "US", "1"},
    {"Uganda", "UG", "256"},
    {"Ukraine", "UA", "380"},
    {"United Arab Emirates", "AE", "971"},
    {"United Kingdom", "GB", "44"},
    {"Uruguay", "UY", "598"},
    {"Uzbekistan", "UZ", "998"},
    {"Vanuatu", "VU", "678"},
    {"Venezuela", "VE", "58"},
    {"Vietnam", "VN", "84"},
    {"Wallis & Futuna", "WF", "681"},
    {"Yemen", "YE", "967"},
    {"Zambia", "ZM", "260"},
    {"Zimbabwe", "ZW", "263"},
}};

}  // namespace

QTM_USE_NAMESPACE

CountryModel::CountryModel(QObject *parent)
    : QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles.insert(Qt::DisplayRole, "name");
    roles.insert(Iso2Role, "iso2");
    roles.insert(CodeRole, "code");

    setRoleNames(roles);
}

int CountryModel::rowCount(const QModelIndex &) const
{
    return countries.size();
}

QVariant CountryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    auto [name, iso2, code] = countries.at(index.row());

    switch (role)
    {
        case Qt::DisplayRole:
            return name;
        case Iso2Role:
            return iso2;
        case CodeRole:
            return code;
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
    return countries.size();
}

int CountryModel::getDefaultIndex() const noexcept
{
    QSystemNetworkInfo info;

    auto it = std::ranges::find_if(countries,
                                   [&info](const auto &value) { return info.currentMobileCountryCode() == QString::fromUtf8(value.code); });

    if (it != countries.end())
    {
        return int(std::distance(countries.begin(), it));
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

    QVariantMap chatFilter;
    chatFilter.insert("id", 0);
    chatFilter.insert("title", tr("FilterAllChats"));

    m_items.clear();
    m_items.append(chatFilter);
}

int ChatFilterModel::rowCount(const QModelIndex &index) const
{
    return m_items.count();
}

QVariant ChatFilterModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (auto item = m_items.value(index.row()); role)
    {
        case IdRole:
            return item.toMap().value("id").toInt();
        case Qt::DisplayRole:
            return item.toMap().value("title").toString();
        case IconNameRole:
            return item.toMap().value("icon_name").toString();
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
    return m_items.count();
}

void ChatFilterModel::handleChatFilters(const QVariantList &chatFilters)
{
    std::ranges::for_each(chatFilters, [this](const auto &chatFilter) {
        if (std::ranges::none_of(
                m_items, [chatFilter](const auto &id) { return id.toMap().value("id").toInt() == chatFilter.toMap().value("id").toInt(); }))
        {
            beginInsertRows(QModelIndex(), rowCount(), rowCount());

            m_items.append(chatFilter);

            endInsertRows();
        }
    });

    emit countChanged();
}
