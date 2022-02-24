#pragma once

#include <cstdint>
#include <initializer_list>

static constexpr auto AppName = "MeeGram";
static constexpr auto AppVersion = "0.1.4";

static constexpr auto ApiId = 142713;
static constexpr auto ApiHash = "9e9e687a70150c6436afe3a2b6bfd7d7";

static constexpr auto DatabaseDirectory = "/.meegram/tdlib";

static constexpr auto DefaultLanguageCode = "en";
static constexpr auto DeviceModel = "Nokia N9";
static constexpr auto SystemVersion = "MeeGo 1.2 Harmattan";

static constexpr auto WaitTimeout = 300.0;  // 30 sec

[[maybe_unused]] static constexpr auto ServiceNotificationsUserIds = {42777, 333000, 777000};
static constexpr auto ChatSliceLimit = 25;
static constexpr auto MessageSliceLimit = 20;

static constexpr auto MutedValueMax = 2147483647;  // int32.max = 2^32 - 1
static constexpr auto MutedValueMin = 0;

namespace fnv {
constexpr uint32_t offsetBasis = 0x811c9dc5;
constexpr uint32_t prime = 0x1000193;

constexpr uint32_t hash(const char *str, const uint32_t value = offsetBasis) noexcept
{
    return *str ? hash(str + 1, (value ^ *str) * static_cast<unsigned long long>(prime)) : value;
}

constexpr uint32_t hashRuntime(const char *str) noexcept
{
    auto value = offsetBasis;

    while (*str)
    {
        value ^= *str++;
        value *= prime;
    }
    return value;
}
}  // namespace fnv
