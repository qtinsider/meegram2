#pragma once

#include "Chat.hpp"
#include "ChatPosition.hpp"
#include "User.hpp"

#include <td/telegram/td_api.h>

#include <QObject>

class Locale;
class StorageManager;

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

    static td::td_api::object_ptr<td::td_api::ChatList> toChatList(const std::unique_ptr<ChatList> &list) noexcept;

    static QString formattedText(const td::td_api::object_ptr<td::td_api::formattedText> &value) noexcept;

    Q_INVOKABLE QString getChatTitle(qlonglong chatId, bool showSavedMessages = false) const noexcept;
    Q_INVOKABLE QString formatTime(int totalSeconds) const noexcept;

private:
    bool isMeChat(const std::shared_ptr<Chat> &chat) const noexcept;
    bool isMeUser(qlonglong userId) const noexcept;

    Locale *m_locale{};
    StorageManager *m_storageManager{};
};
