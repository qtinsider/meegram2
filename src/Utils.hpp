#pragma once

#include "Chat.hpp"
#include "ChatPosition.hpp"
#include "User.hpp"

#include <td/telegram/td_api.h>

#include <QObject>

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

    static td::td_api::object_ptr<td::td_api::ChatList> toChatList(const std::unique_ptr<ChatList> &list) noexcept;

    static QString formattedText(const td::td_api::object_ptr<td::td_api::formattedText> &value) noexcept;

    Q_INVOKABLE static QString formatTime(int totalSeconds) noexcept;

    Q_INVOKABLE static QString getChatTitle(qlonglong chatId, bool showSavedMessages = false) noexcept;
    Q_INVOKABLE static QString getUserName(qlonglong userId, bool openUser = false) noexcept;
    Q_INVOKABLE static QString getUserShortName(qlonglong userId) noexcept;
    Q_INVOKABLE static QString getUserFullName(qlonglong userId) noexcept;

    static bool isMeUser(qlonglong userId) noexcept;
    static bool isMeChat(const std::shared_ptr<Chat> &chat) noexcept;
};
