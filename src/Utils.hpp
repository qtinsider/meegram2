#pragma once

#include "ChatPosition.hpp"

#include <td/telegram/td_api.h>

#include <QObject>

class Chat;
class Locale;
class Message;
class MessageAudio;
class MessageCall;
class StorageManager;
class User;

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

    static td::td_api::object_ptr<td::td_api::ChatList> toChatList(const std::unique_ptr<ChatList> &list) noexcept;

    static QString formattedText(const td::td_api::object_ptr<td::td_api::formattedText> &value) noexcept;

    Q_INVOKABLE static QString formatTime(int totalSeconds) noexcept;

    Q_INVOKABLE static QString replaceEmoji(const QString &text) noexcept;

    static QString getAudioTitle(MessageAudio *audio) noexcept;
    static QString getCallContent(MessageCall *call, bool isOutgoing) noexcept;

    static QString getMessageDate(Message *message) noexcept;

    static QString getContent(Message *message, std::shared_ptr<StorageManager> storage, std::shared_ptr<Locale> locale) noexcept;
    static QString getServiceContent(Message *message, std::shared_ptr<StorageManager> storage, std::shared_ptr<Locale> locale, bool openUser = false) noexcept;

    static QString getChatTitle(std::shared_ptr<Chat> chat, std::shared_ptr<StorageManager> storage, bool showSavedMessages = true) noexcept;

    static QString getSenderName(Message *message, std::shared_ptr<StorageManager> storage) noexcept;
    static QString getSenderAuthor(Message *message, std::shared_ptr<StorageManager> storage, bool openUser) noexcept;

    static QString getUserName(std::shared_ptr<User> user, bool openUser) noexcept;
    static QString getUserShortName(std::shared_ptr<User> user) noexcept;
    static QString getUserFullName(std::shared_ptr<User> user) noexcept;

    static bool isMeUser(std::shared_ptr<User> user, std::shared_ptr<StorageManager> storage) noexcept;
    static bool isMeChat(std::shared_ptr<Chat> chat, std::shared_ptr<StorageManager> storage) noexcept;
};
