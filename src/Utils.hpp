#pragma once

#include <QImage>
#include <QObject>
#include <QVariant>

class Locale;
class StorageManager;

namespace Utils
{
      QVariantMap getChatPosition(qint64 chatId, const QVariantMap &chatList);
      bool isChatPinned(qint64 chatId, const QVariantMap &chatList, StorageManager *store);
      qint64 getChatOrder(qint64 chatId, const QVariantMap &chatList, StorageManager *store);
      bool chatListEquals(const QVariantMap &list1, const QVariantMap &list2);

      QString getChatTitle(qint64 chatId, StorageManager *store, Locale *locale, bool showSavedMessages = false);
      bool isChatMuted(qint64 chatId, StorageManager *store);
      int getChatMuteFor(qint64 chatId, StorageManager *store);

      QString getServiceMessageContent(const QVariantMap &message, StorageManager *store, Locale *locale, bool openUser = false);
      bool isServiceMessage(const QVariantMap &message);

        QString getUserShortName(qint64 userId, StorageManager *store, Locale *locale) noexcept;

        QString getContent(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept;
        QString getFormattedText(const QVariantMap &formattedText, StorageManager *store, Locale *locale, const QVariantMap &options = {}) noexcept;
        QString getTitle(const QVariantMap &message) noexcept;
        QString getMessageDate(const QVariantMap &message, Locale *locale) noexcept;
        QString getMessageSenderName(const QVariantMap &message, StorageManager *store, Locale *locale) noexcept;

        bool isChatUnread(qint64 chatId, StorageManager *store) noexcept;
        bool isMessageUnread(qint64 chatId, const QVariantMap &message, StorageManager *store) noexcept;

        QString getFileSize(const QVariantMap &file) noexcept;

      void copyToClipboard(const QVariantMap &content) noexcept;
      QImage getThumb(const QVariantMap &thumbnail)  noexcept;
      QString getViews(int views) noexcept;

      QString formatTime(int totalSeconds)  noexcept;
}
