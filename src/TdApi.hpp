#pragma once

#include <QObject>

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
        ChatListFolder,
    };
};

struct ChatList
{
    int folderId{};
    TdApi::ChatList type;
};
