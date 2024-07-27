#pragma once

#include <QHash>
#include <QLocale>
#include <QString>

#include <array>
#include <cstdint>
#include <vector>

constexpr auto AppName = "MeeGram";
constexpr auto AppVersion = "0.1.6";

constexpr auto ApiId = 142713;
constexpr auto ApiHash = "9e9e687a70150c6436afe3a2b6bfd7d7";

constexpr auto DatabaseDirectory = "/.meegram/tdlib";

static auto DefaultLanguageCode = QLocale::system().name().left(2);

constexpr auto DeviceModel = "Nokia N9";
constexpr auto SystemVersion = "MeeGo 1.2 Harmattan";

constexpr auto WaitTimeout = 30.0;  // 30 sec

[[maybe_unused]] constexpr std::array<int, 3> ServiceNotificationsUserIds = {42777, 333000, 777000};

constexpr auto ChatSliceLimit = 25;
constexpr auto MessageSliceLimit = 20;

constexpr auto MutedValueMax = 2147483647;  // int32.max = 2^32 - 1
constexpr auto MutedValueMin = 0;

namespace detail {

template <class... Ts>
struct Overloaded : Ts...
{
    using Ts::operator()...;
};

template <class... Ts>
Overloaded(Ts...) -> Overloaded<Ts...>;

}  // namespace detail

namespace std {
template <>
struct hash<QString>
{
    std::size_t operator()(const QString &s) const
    {
        return qHash(s);
    }
};

template <>
struct hash<std::vector<unsigned int>>
{
    std::size_t operator()(const auto &emoji) const
    {
        std::size_t seed = 0;
        for (const auto &element : emoji)
        {
            seed ^= std::hash<uint>()(element) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
};
}  // namespace std
