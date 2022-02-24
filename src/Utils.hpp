#pragma once

#include <QImage>
#include <QObject>
#include <QVariant>

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

    static QVariantMap getChatPosition(qint64 chatId, const QVariantMap &chatList);
    static bool isChatPinned(qint64 chatId, const QVariantMap &chatList);
    static qint64 getChatOrder(qint64 chatId, const QVariantMap &chatList);
    static bool chatListEquals(const QVariantMap &list1, const QVariantMap &list2);

    static QString getChatTitle(qint64 chatId, bool showSavedMessages = false);
    static bool isChatMuted(qint64 chatId);
    static int getChatMuteFor(qint64 chatId);

    static QString getServiceMessageContent(const QVariantMap &message, bool openUser = false);
    static bool isServiceMessage(const QVariantMap &message);

    Q_INVOKABLE static QString getUserShortName(qint64 userId) noexcept;

    Q_INVOKABLE static QString getContent(const QVariantMap &message) noexcept;
    Q_INVOKABLE static QString getFormattedText(const QVariantMap &formattedText, const QVariantMap &options = {}) noexcept;
    Q_INVOKABLE static QString getTitle(const QVariantMap &message) noexcept;
    Q_INVOKABLE static QString getMessageDate(const QVariantMap &message) noexcept;
    Q_INVOKABLE static QString getMessageSenderName(const QVariantMap &message) noexcept;

    Q_INVOKABLE static bool isChatUnread(qint64 chatId) noexcept;
    Q_INVOKABLE static bool isMessageUnread(qint64 chatId, const QVariantMap &message) noexcept;

    Q_INVOKABLE static QString getFileSize(const QVariantMap &file) noexcept;

    Q_INVOKABLE void copyToClipboard(const QVariantMap &content) noexcept;
    Q_INVOKABLE QImage getThumb(const QVariantMap &thumbnail) const noexcept;
    Q_INVOKABLE QString getViews(int views) const noexcept;

    Q_INVOKABLE QString formatTime(int totalSeconds) const noexcept;
};
