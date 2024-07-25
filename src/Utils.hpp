#pragma once

#include <QImage>
#include <QObject>
#include <QVariant>

class Message;
class Chat;
class User;

class Locale;
class StorageManager;

namespace Utils {
QVariantMap getChatPosition(qint64 chatId, const QVariantMap &chatList,
                            StorageManager *store);
bool isChatPinned(qint64 chatId, const QVariantMap &chatList,
                  StorageManager *store);
qint64 getChatOrder(qint64 chatId, const QVariantMap &chatList,
                    StorageManager *store);
bool chatListEquals(const QVariantMap &list1, const QVariantMap &list2);

QString getChatTitle(qint64 chatId, StorageManager *store, Locale *locale,
                     bool showSavedMessages = false);
bool isChatMuted(qint64 chatId, StorageManager *store);
int getChatMuteFor(qint64 chatId, StorageManager *store);

bool isMeChat(const Chat *chat, StorageManager *store) noexcept;

QString getServiceMessageContent(const Message *message, StorageManager *store,
                                 Locale *locale, bool openUser = false);
bool isServiceMessage(const Message *message);

QString getUserShortName(qint64 userId, StorageManager *store,
                         Locale *locale) noexcept;

QString getContent(const Message *message, StorageManager *store,
                   Locale *locale) noexcept;
QString getFormattedText(const QVariantMap &formattedText) noexcept;
QString getTitle(const Message *message, StorageManager *store,
                 Locale *locale) noexcept;
QString getMessageDate(const Message *message, Locale *locale) noexcept;
QString getMessageSenderName(const Message *message, StorageManager *store,
                             Locale *locale) noexcept;

bool isChatUnread(qint64 chatId, StorageManager *store) noexcept;
bool isMessageUnread(qint64 chatId, const QVariantMap &message,
                     StorageManager *store) noexcept;

QString getFileSize(const QVariantMap &file) noexcept;

void copyToClipboard(const QVariantMap &content) noexcept;
QImage getThumb(const QVariantMap &thumbnail) noexcept;
QString getViews(int views) noexcept;

QString formatTime(int totalSeconds) noexcept;
} // namespace Utils
