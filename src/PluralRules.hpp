#include <memory>

class PluralRules
{
public:
    virtual ~PluralRules() = default;

    enum class Quantity {
        QuantityOther = 0x0000,
        QuantityZero = 0x0001,
        QuantityOne = 0x0002,
        QuantityTwo = 0x0004,
        QuantityFew = 0x0008,
        QuantityMany = 0x0010,
    };

    virtual Quantity quantityForNumber(int count) const = 0;
    virtual std::unique_ptr<PluralRules> clone() const = 0;
};

class PluralRules_Zero : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Welsh : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Two : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Tachelhit : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Slovenian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Romanian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Polish : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_One : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_None : public PluralRules
{
public:
    Quantity quantityForNumber(int /*count*/) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Maltese : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Macedonian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Lithuanian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Latvian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Langi : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_French : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Czech : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Breton : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Balkan : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Serbian : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};

class PluralRules_Arabic : public PluralRules
{
public:
    Quantity quantityForNumber(int count) const override;

    std::unique_ptr<PluralRules> clone() const override;
};
