#include "PluralRules.hpp"

PluralRules::Quantity PluralRules_Zero::quantityForNumber(int count) const
{
    return (count == 0 || count == 1) ? Quantity::QuantityOne : Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Zero::clone() const
{
    return std::make_unique<PluralRules_Zero>(*this);
}

PluralRules::Quantity PluralRules_Welsh::quantityForNumber(int count) const
{
    switch (count)
    {
        case 0:
            return Quantity::QuantityZero;
        case 1:
            return Quantity::QuantityOne;
        case 2:
            return Quantity::QuantityTwo;
        case 3:
            return Quantity::QuantityFew;
        case 6:
            return Quantity::QuantityMany;
        default:
            return Quantity::QuantityOther;
    }
}

std::unique_ptr<PluralRules> PluralRules_Welsh::clone() const
{
    return std::make_unique<PluralRules_Welsh>(*this);
}

PluralRules::Quantity PluralRules_Two::quantityForNumber(int count) const
{
    if (count == 1)
        return Quantity::QuantityOne;
    if (count == 2)
        return Quantity::QuantityTwo;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Two::clone() const
{
    return std::make_unique<PluralRules_Two>(*this);
}

PluralRules::Quantity PluralRules_Tachelhit::quantityForNumber(int count) const
{
    if (count >= 0 && count <= 1)
        return Quantity::QuantityOne;
    if (count >= 2 && count <= 10)
        return Quantity::QuantityFew;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Tachelhit::clone() const
{
    return std::make_unique<PluralRules_Tachelhit>(*this);
}

PluralRules::Quantity PluralRules_Slovenian::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    if (rem100 == 1)
        return Quantity::QuantityOne;
    if (rem100 == 2)
        return Quantity::QuantityTwo;
    if (rem100 >= 3 && rem100 <= 4)
        return Quantity::QuantityFew;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Slovenian::clone() const
{
    return std::make_unique<PluralRules_Slovenian>(*this);
}

PluralRules::Quantity PluralRules_Romanian::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    if (count == 1)
        return Quantity::QuantityOne;
    if (count == 0 || (rem100 >= 1 && rem100 <= 19))
        return Quantity::QuantityFew;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Romanian::clone() const
{
    return std::make_unique<PluralRules_Romanian>(*this);
}

PluralRules::Quantity PluralRules_Polish::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    const auto rem10 = count % 10;
    if (count == 1)
        return Quantity::QuantityOne;
    if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        return Quantity::QuantityFew;
    if ((rem10 >= 0 && rem10 <= 1) || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 12 && rem100 <= 14))
        return Quantity::QuantityMany;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Polish::clone() const
{
    return std::make_unique<PluralRules_Polish>(*this);
}

PluralRules::Quantity PluralRules_One::quantityForNumber(int count) const
{
    return count == 1 ? Quantity::QuantityOne : Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_One::clone() const
{
    return std::make_unique<PluralRules_One>(*this);
}

PluralRules::Quantity PluralRules_None::quantityForNumber(int) const
{
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_None::clone() const
{
    return std::make_unique<PluralRules_None>(*this);
}

PluralRules::Quantity PluralRules_Maltese::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    if (count == 1)
        return Quantity::QuantityOne;
    if (count == 0 || (rem100 >= 2 && rem100 <= 10))
        return Quantity::QuantityFew;
    if (rem100 >= 11 && rem100 <= 19)
        return Quantity::QuantityMany;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Maltese::clone() const
{
    return std::make_unique<PluralRules_Maltese>(*this);
}

PluralRules::Quantity PluralRules_Macedonian::quantityForNumber(int count) const
{
    if (count % 10 == 1 && count != 11)
        return Quantity::QuantityOne;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Macedonian::clone() const
{
    return std::make_unique<PluralRules_Macedonian>(*this);
}

PluralRules::Quantity PluralRules_Lithuanian::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    const auto rem10 = count % 10;
    if (rem10 == 1 && !(rem100 >= 11 && rem100 <= 19))
        return Quantity::QuantityOne;
    if (rem10 >= 2 && rem10 <= 9 && !(rem100 >= 11 && rem100 <= 19))
        return Quantity::QuantityFew;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Lithuanian::clone() const
{
    return std::make_unique<PluralRules_Lithuanian>(*this);
}

PluralRules::Quantity PluralRules_Latvian::quantityForNumber(int count) const
{
    if (count == 0)
        return Quantity::QuantityZero;
    if (count % 10 == 1 && count % 100 != 11)
        return Quantity::QuantityOne;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Latvian::clone() const
{
    return std::make_unique<PluralRules_Latvian>(*this);
}

PluralRules::Quantity PluralRules_Langi::quantityForNumber(int count) const
{
    if (count == 0)
        return Quantity::QuantityZero;
    if (count == 1)
        return Quantity::QuantityOne;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Langi::clone() const
{
    return std::make_unique<PluralRules_Langi>(*this);
}

PluralRules::Quantity PluralRules_French::quantityForNumber(int count) const
{
    return (count >= 0 && count < 2) ? Quantity::QuantityOne : Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_French::clone() const
{
    return std::make_unique<PluralRules_French>(*this);
}

PluralRules::Quantity PluralRules_Czech::quantityForNumber(int count) const
{
    if (count == 1)
        return Quantity::QuantityOne;
    if (count >= 2 && count <= 4)
        return Quantity::QuantityFew;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Czech::clone() const
{
    return std::make_unique<PluralRules_Czech>(*this);
}

PluralRules::Quantity PluralRules_Breton::quantityForNumber(int count) const
{
    switch (count)
    {
        case 0:
            return Quantity::QuantityZero;
        case 1:
            return Quantity::QuantityOne;
        case 2:
            return Quantity::QuantityTwo;
        case 3:
            return Quantity::QuantityFew;
        case 6:
            return Quantity::QuantityMany;
        default:
            return Quantity::QuantityOther;
    }
}

std::unique_ptr<PluralRules> PluralRules_Breton::clone() const
{
    return std::make_unique<PluralRules_Breton>(*this);
}

PluralRules::Quantity PluralRules_Balkan::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    const auto rem10 = count % 10;
    if (rem10 == 1 && rem100 != 11)
        return Quantity::QuantityOne;
    if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        return Quantity::QuantityFew;
    if ((rem10 == 0 || (rem10 >= 5 && rem10 <= 9) || (rem100 >= 11 && rem100 <= 14)))
        return Quantity::QuantityMany;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Balkan::clone() const
{
    return std::make_unique<PluralRules_Balkan>(*this);
}

PluralRules::Quantity PluralRules_Serbian::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    const auto rem10 = count % 10;
    if (rem10 == 1 && rem100 != 11)
        return Quantity::QuantityOne;
    if (rem10 >= 2 && rem10 <= 4 && !(rem100 >= 12 && rem100 <= 14))
        return Quantity::QuantityFew;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Serbian::clone() const
{
    return std::make_unique<PluralRules_Serbian>(*this);
}

PluralRules::Quantity PluralRules_Arabic::quantityForNumber(int count) const
{
    const auto rem100 = count % 100;
    if (count == 0)
        return Quantity::QuantityZero;
    if (count == 1)
        return Quantity::QuantityOne;
    if (count == 2)
        return Quantity::QuantityTwo;
    if (rem100 >= 3 && rem100 <= 10)
        return Quantity::QuantityFew;
    if (rem100 >= 11 && rem100 <= 99)
        return Quantity::QuantityMany;
    return Quantity::QuantityOther;
}

std::unique_ptr<PluralRules> PluralRules_Arabic::clone() const
{
    return std::make_unique<PluralRules_Arabic>(*this);
}
