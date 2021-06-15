#pragma once

#include <qplatformdefs.h>  // MEEGO_EDITION_HARMATTAN

#include <initializer_list>

inline constexpr auto AppName = "MeeGram";
inline constexpr auto AppVersion = "0.0.1";

inline constexpr auto ApiId = 142713;
inline constexpr auto ApiHash = "9e9e687a70150c6436afe3a2b6bfd7d7";

#if defined(MEEGO_EDITION_HARMATTAN)
inline constexpr auto DatabaseDirectory = "/.meegram/tdlib";
#else
inline constexpr auto DatabaseDirectory = "/.tdlib";
#endif

inline constexpr auto WaitTimeout = 30.0;  // 30 sec

inline constexpr auto ServiceNotificationsUserIds = {42777, 333000, 777000};
inline constexpr auto ChatSliceLimit = 25;
inline constexpr auto MessageSliceLimit = 20;
