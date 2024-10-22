#pragma once

#include "Chat.hpp"
#include "ChatPosition.hpp"
#include "User.hpp"

#include <td/telegram/td_api.h>

#include <QImage>
#include <QVariant>

class Locale;
class StorageManager;

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

    static td::td_api::object_ptr<td::td_api::ChatList> toChatList(const std::unique_ptr<ChatList> &list);

    Q_INVOKABLE QString getChatTitle(qlonglong chatId, bool showSavedMessages = false) const noexcept;

    static QString getServiceMessageContent(const td::td_api::message &message, StorageManager *store, bool openUser = false);
    static bool isServiceMessage(const td::td_api::message &message);

    static QString getUserShortName(qlonglong userId, StorageManager *store) noexcept;

    static QString getTitle(const td::td_api::message &message, StorageManager *store) noexcept;
    static QString getMessageDate(const td::td_api::message &message) noexcept;
    static QString getMessageSenderName(const td::td_api::message &message, StorageManager *store) noexcept;

    static bool isChatUnread(qlonglong chatId, StorageManager *store) noexcept;
    static bool isMessageUnread(const td::td_api::message &message, StorageManager *store) noexcept;

    static QString getFileSize(const QVariantMap &file) noexcept;

    static void copyToClipboard(const QVariantMap &content) noexcept;
    static QImage getThumb(const QVariantMap &thumbnail) noexcept;
    static QString getViews(int views) noexcept;

    Q_INVOKABLE QString formatTime(int totalSeconds) const noexcept;

private:
    bool isMeChat(const std::shared_ptr<Chat> chat) const noexcept;
    bool isMeUser(qlonglong userId) const noexcept;

    Locale *m_locale{};
    StorageManager *m_storageManager{};
};
