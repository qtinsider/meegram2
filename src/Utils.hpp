#pragma once

#include "TdApi.hpp"

#include <td/telegram/td_api.h>

#include <QImage>
#include <QVariant>

class StorageManager;

struct ChatListComparator
{
    bool operator()(const auto &a, const auto &b) const
    {
        if (!a || !b)
        {
            return false;
        }

        if (a->get_id() != b->get_id())
        {
            return false;
        }

        if (a->get_id() == td::td_api::chatListFolder::ID)
        {
            const auto *folderA = static_cast<const td::td_api::chatListFolder *>(a.get());
            const auto *folderB = static_cast<const td::td_api::chatListFolder *>(b.get());
            if (folderA && folderB)
            {
                return folderA->chat_folder_id_ == folderB->chat_folder_id_;
            }
        }

        return true;
    }
};

class Utils
{
public:
    static td::td_api::object_ptr<td::td_api::ChatList> toChatList(const ChatList &list);

    static td::td_api::object_ptr<td::td_api::chatPosition> getChatPosition(const td::td_api::chat *chat, const ChatList &chatList);

    static bool isChatPinned(const td::td_api::chat *chat, const ChatList &chatList);
    static qlonglong getChatOrder(const td::td_api::chat *chat, const ChatList &chatList);

    static QString getChatTitle(qlonglong chatId, StorageManager *store, bool showSavedMessages = false);
    static bool isChatMuted(qlonglong chatId, StorageManager *store);
    static int getChatMuteFor(qlonglong chatId, StorageManager *store);

    static bool isMeChat(const td::td_api::chat *chat, StorageManager *store) noexcept;

    static QString getServiceMessageContent(const td::td_api::message &message, StorageManager *store, bool openUser = false);
    static bool isServiceMessage(const td::td_api::message &message);

    static QString getUserShortName(qlonglong userId, StorageManager *store) noexcept;

    static QString getContent(const td::td_api::message &message, StorageManager *store) noexcept;
    static QString getTitle(const td::td_api::message &message, StorageManager *store) noexcept;
    static QString getMessageDate(const td::td_api::message &message) noexcept;
    static QString getMessageSenderName(const td::td_api::message &message, StorageManager *store) noexcept;

    static bool isChatUnread(qlonglong chatId, StorageManager *store) noexcept;
    static bool isMessageUnread(const td::td_api::message &message, StorageManager *store) noexcept;

    static QString getFileSize(const QVariantMap &file) noexcept;

    static void copyToClipboard(const QVariantMap &content) noexcept;
    static QImage getThumb(const QVariantMap &thumbnail) noexcept;
    static QString getViews(int views) noexcept;

    static QString formatTime(int totalSeconds) noexcept;
};
