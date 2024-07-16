#pragma once

#include <QLocale>

#include <array>
#include <cstdint>

constexpr auto AppName = "MeeGram";
constexpr auto AppVersion = "0.1.6";

constexpr auto ApiId = 142713;
constexpr auto ApiHash = "9e9e687a70150c6436afe3a2b6bfd7d7";

constexpr auto DatabaseDirectory = "/.meegram/tdlib4";

static auto DefaultLanguageCode = QLocale::system().name().left(2);

constexpr auto DeviceModel = "Nokia N9";
constexpr auto SystemVersion = "MeeGo 1.2 Harmattan";

constexpr auto WaitTimeout = 30.0;  // 30 sec

[[maybe_unused]] constexpr std::array<int, 3> ServiceNotificationsUserIds = {42777, 333000, 777000};

constexpr auto ChatSliceLimit = 25;
constexpr auto MessageSliceLimit = 20;

constexpr auto MutedValueMax = 2147483647;  // int32.max = 2^32 - 1
constexpr auto MutedValueMin = 0;
