#pragma once

#include <QFont>
#include <QTextCursor>
#include <QTextDocument>

#include <memory>

class TextFormatter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString text READ text NOTIFY textChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QVariant formattedText READ formattedText WRITE setFormattedText NOTIFY formattedTextChanged)

public:
    explicit TextFormatter(QObject *parent = nullptr);
    ~TextFormatter() override;

    QString text() const;

    QFont font() const;
    void setFont(const QFont &font);

    QVariant formattedText() const;
    void setFormattedText(const QVariant &formattedText);

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
};
