#pragma once

#include <QObject>
#include <QVariant>

#include <array>
#include <cstdint>

constexpr auto AppName = "MeeGram";
constexpr auto AppVersion = "0.1.6";

constexpr auto ApiId = 142713;
constexpr auto ApiHash = "9e9e687a70150c6436afe3a2b6bfd7d7";

constexpr auto DatabaseDirectory = "/.meegram/tdlib";

constexpr auto DeviceModel = "Nokia N9";
constexpr auto SystemVersion = "MeeGo 1.2 Harmattan";

constexpr auto WaitTimeout = 30.0;  // 30 sec

[[maybe_unused]] constexpr std::array<int, 3> ServiceNotificationsUserIds = {42777, 333000, 777000};
constexpr auto ChatSliceLimit = 25;
constexpr auto MessageSliceLimit = 20;

constexpr auto MutedValueMax = 2147483647;  // int32.max = 2^32 - 1
constexpr auto MutedValueMin = 0;

namespace fnv {
constexpr uint32_t offsetBasis = 0x811c9dc5;
constexpr uint32_t prime = 0x1000193;

consteval uint32_t hash(const char *str, const uint32_t value = offsetBasis) noexcept
{
    return *str ? hash(str + 1, (value ^ *str) * static_cast<unsigned long long>(prime)) : value;
}

[[nodiscard]] constexpr uint32_t hashRuntime(const char *str) noexcept
{
    uint32_t value = offsetBasis;

    while (*str)
    {
        value ^= *str++;
        value *= prime;
    }
    return value;
}
}  // namespace fnv

class TdApi : public QObject
{
    Q_OBJECT
    Q_ENUMS(AuthorizationState ChatList)

public:
    explicit TdApi(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    enum AuthorizationState {
        AuthorizationStateWaitTdlibParameters,
        AuthorizationStateWaitEncryptionKey,
        AuthorizationStateWaitPhoneNumber,
        AuthorizationStateWaitCode,
        AuthorizationStateWaitOtherDeviceConfirmation,
        AuthorizationStateWaitRegistration,
        AuthorizationStateWaitPassword,
        AuthorizationStateReady,
        AuthorizationStateLoggingOut,
        AuthorizationStateClosing,
        AuthorizationStateClosed,
    };

    enum ChatList {
        ChatListMain,
        ChatListArchive,
        ChatListFilter,
    };
};
