#pragma once

#include <QFont>
#include <QObject>
#include <QVariant>

class QTextDocument;
class QTextCursor;

class TextFormatter : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString richText READ richText WRITE setRichText NOTIFY richTextChanged)
    Q_PROPERTY(QFont defaultFont READ defaultFont WRITE setDefaultFont NOTIFY defaultFontChanged)

    Q_PROPERTY(QVariant formattedText READ formattedText NOTIFY formattedTextChanged)

public:
    explicit TextFormatter(QObject *parent = nullptr);
    ~TextFormatter() override;

    QString richText() const;
    void setRichText(const QString &richText);

    QFont defaultFont() const;
    void setDefaultFont(const QFont &font);

    QVariant formattedText() const;
    void setFormattedText(const QVariant &formattedText);

signals:
    void richTextChanged();
    void defaultFontChanged();
    void formattedTextChanged();

private:
    void update();

    QTextDocument *m_document;
    QTextCursor *m_cursor;

    QVariant m_formattedText;
};
