#pragma once

#include <QImage>
#include <QObject>
#include <QVariant>

class Utils : public QObject
{
    Q_OBJECT
public:
    explicit Utils(QObject *parent = nullptr);

    [[nodiscard]] static QString getServiceMessageContent(const QVariantMap &chat, const QVariantMap &message);
    [[nodiscard]] static bool isServiceMessage(const QVariantMap &message);

    Q_INVOKABLE static QString getUserShortName(int userId) noexcept;

    Q_INVOKABLE static QString getContent(const QVariantMap &chat) noexcept;
    Q_INVOKABLE static QString getFormattedText(const QVariantMap &formattedText, bool plainText = false) noexcept;
    Q_INVOKABLE static QString getTitle(const QVariantMap &message) noexcept;
    Q_INVOKABLE static QString getMessageDate(const QVariantMap &message) noexcept;
    Q_INVOKABLE static QString getMessageSenderName(const QVariantMap &chat, const QVariantMap &message) noexcept;

    Q_INVOKABLE static QString getChatTitle(const QVariantMap &chat) noexcept;

    Q_INVOKABLE static bool isChatUnread(const QVariantMap &chat) noexcept;
    Q_INVOKABLE static bool isMessageUnread(const QVariantMap &chat, const QVariantMap &message) noexcept;

    Q_INVOKABLE static QString getFileSize(const QVariantMap &file) noexcept;

    Q_INVOKABLE void copyToClipboard(const QString &text) noexcept;
    Q_INVOKABLE QImage getThumb(const QVariantMap &thumbnail) const noexcept;
    Q_INVOKABLE QString getViews(int views) const noexcept;

    Q_INVOKABLE QString formatTime(int totalSeconds) const noexcept;
};
