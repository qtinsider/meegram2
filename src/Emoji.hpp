#pragma once

#include <QObject>

#include <array>

class Emoji
{
    Q_GADGET
    Q_PROPERTY(QString unicode READ unicode CONSTANT)
    Q_PROPERTY(QString filename READ filename CONSTANT)
    Q_PROPERTY(QString description READ description CONSTANT)
    Q_PROPERTY(Category category READ category CONSTANT)

public:
    enum Category { Emotion, People, Animals, Food, Places, Activities, Objects, Symbols, Flags };

    Q_ENUMS(Category)

    constexpr Emoji() = default;

    constexpr Emoji(std::u16string_view unicode, std::u16string_view filename, std::u16string_view description, Category category)
        : m_unicode(unicode)
        , m_filename(filename)
        , m_description(description)
        , m_category(category)
    {
    }

    QString unicode() const noexcept
    {
        return QString::fromUtf16(reinterpret_cast<const ushort *>(m_unicode.data()), m_unicode.size());
    }

    QString filename() const noexcept
    {
        return QString::fromUtf16(reinterpret_cast<const ushort *>(m_filename.data()), m_filename.size());
    }

    QString description() const noexcept
    {
        return QString::fromUtf16(reinterpret_cast<const ushort *>(m_description.data()), m_description.size());
    }

    Category category() const noexcept
    {
        return m_category;
    }

    static const std::array<Emoji, 3773> emojis();

private:
    std::u16string_view m_unicode;
    std::u16string_view m_filename;
    std::u16string_view m_description;
    Category m_category;
};
