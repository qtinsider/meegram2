#pragma once

#include <td/telegram/td_api.h>

#include <QObject>
#include <QTextCursor>
#include <QTextDocument>
#include <QVariant>

#include <functional>
#include <memory>
#include <unordered_map>

class TextFormatter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QVariant formattedText READ formattedText WRITE setFormattedText NOTIFY formattedTextChanged)

public:
    explicit TextFormatter(QObject *parent = nullptr);

    QString text() const;

    QFont font() const;
    void setFont(const QFont &value);

    QVariant formattedText() const;
    void setFormattedText(const QVariant &value);

signals:
    void textChanged();
    void fontChanged();
    void formattedTextChanged();

private slots:
    void applyFormatting();

private:
    QVariant m_formattedText;

    std::unique_ptr<QTextDocument> m_document;
    std::unique_ptr<QTextCursor> m_cursor;

    static const std::unordered_map<int, std::function<void(QTextCharFormat &, const QString &, const td::td_api::TextEntityType &)>> s_formatters;
};
