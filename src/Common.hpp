#pragma once

#include <initializer_list>

static constexpr auto AppName = "MeeGram";
static constexpr auto AppVersion = "0.1.3";

static constexpr auto ApiId = 142713;
static constexpr auto ApiHash = "9e9e687a70150c6436afe3a2b6bfd7d7";

static constexpr auto DatabaseDirectory = "/.meegram/tdlib";

static constexpr auto SystemLanguageCode = "en";
static constexpr auto DeviceModel = "Nokia N9";
static constexpr auto SystemVersion = "MeeGo 1.2 Harmattan";

static constexpr auto WaitTimeout = 30.0;  // 30 sec

[[maybe_unused]] static constexpr auto ServiceNotificationsUserIds = {42777, 333000, 777000};
static constexpr auto ChatSliceLimit = 25;
static constexpr auto MessageSliceLimit = 20;

static constexpr auto MutedValueMax = 2147483647; // int32.max = 2^32 - 1
static constexpr auto MutedValueMin = 0;
