#include "TextFormatter.hpp"

#include <QDebug>
#include <QPainter>
#include <QTextBlock>
#include <QTextCharFormat>

// clang-format off
const std::unordered_map<QString, std::function<void(QTextCharFormat &, const QString &, const QVariantMap &)>> TextFormatter::s_formatters = {
    {"textEntityTypeBold", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontWeight(QFont::Bold); }},
    {"textEntityTypeItalic", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontItalic(true); }},
    {"textEntityTypeUnderline", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontUnderline(true); }},
    {"textEntityTypeStrikethrough", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontStrikeOut(true); }},
    {"textEntityTypeCode", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFamily("Courier"); }},
    {"textEntityTypePre", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFamily("Courier"); }},
    {"textEntityTypeTextUrl", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &type) {
         QString url = type.value("url").toString();
         if (url.isEmpty())
         {
             url = entityText;
         }
         format.setAnchor(true);
         format.setAnchorHref(url);
         format.setFontUnderline(true);
     }},
    {"textEntityTypeUrl", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
         format.setAnchor(true);
         format.setAnchorHref(entityText);
         format.setFontUnderline(true);
     }},
    {"textEntityTypeEmailAddress", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
         format.setAnchor(true);
         format.setAnchorHref("mailto:" + entityText);
     }},
    {"textEntityTypePhoneNumber", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
         format.setAnchor(true);
         format.setAnchorHref("tel:" + entityText);
     }},
    {"textEntityTypeMention", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
         format.setAnchor(true);
         format.setAnchorHref("mention:" + entityText);
     }},
    {"textEntityTypeMentionName", [](QTextCharFormat &format, const QString &, const QVariantMap &entity) {
         format.setAnchor(true);
         format.setAnchorHref("mention_name:" + QString::number(entity.value("user_id").toInt()));
     }},
    {"textEntityTypeHashtag", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
         format.setAnchor(true);
         format.setAnchorHref("hashtag:" + entityText);
     }},
    {"textEntityTypeCashtag", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
         format.setAnchor(true);
         format.setAnchorHref("cashtag:" + entityText);
     }},
    {"textEntityTypeBotCommand", [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
         format.setAnchor(true);
         format.setAnchorHref("botCommand:" + entityText);
     }}};

// clang-format on


TextFormatter::TextFormatter(QObject *parent)
    : QObject(parent)
    , m_document(std::make_unique<QTextDocument>(this))
    , m_cursor(std::make_unique<QTextCursor>(m_document.get()))
{

    connect(this, SIGNAL(formattedTextChanged()), this, SLOT(applyFormatting()));
}

QString TextFormatter::text() const
{
    return m_document->toHtml();
}

QFont TextFormatter::font() const
{
    return m_document->defaultFont();
}

void TextFormatter::setFont(const QFont &value)
{
    if (value != m_document->defaultFont())
    {
        m_document->setDefaultFont(value);
        emit fontChanged();
    }
}

QVariant TextFormatter::formattedText() const
{
    return m_formattedText;
}

void TextFormatter::setFormattedText(const QVariant &value)
{
    if (m_formattedText != value)
    {
        m_formattedText = value;
        emit formattedTextChanged();
    }
}

void TextFormatter::applyFormatting()
{
    if (m_formattedText.canConvert<QString>())
    {
        m_document->setPlainText(m_formattedText.toString());

        emit textChanged();
        return;
    }

    const auto formattedText = m_formattedText.toMap();
    if (formattedText.value("@type").toString() != "formattedText" || formattedText.value("text").toString().isEmpty())
    {
        return;
    }

    const auto text = formattedText.value("text").toString();
    const auto entities = formattedText.value("entities").toList();

    bool removeLineBreakAfterCodeBlock = false;
    int currentIndex = 0;

    auto insertTextSegment = [&](const QString &textSegment) {
        if (removeLineBreakAfterCodeBlock && textSegment.startsWith('\n'))
        {
            m_cursor->insertText(textSegment.mid(1));
            removeLineBreakAfterCodeBlock = false;
        }
        else
        {
            m_cursor->insertText(textSegment);
        }
    };

    for (const auto &entityRef : entities)
    {
        const auto entity = entityRef.toMap();
        const auto offset = entity.value("offset").toInt();
        const auto length = entity.value("length").toInt();
        const auto type = entity.value("type").toMap();
        const auto entityType = type.value("@type").toString();

        if (currentIndex > offset)
        {
            continue;
        }

        insertTextSegment(text.mid(currentIndex, offset - currentIndex));

        const auto entityText = text.mid(offset, length);
        QTextCharFormat format;

        if (auto it = s_formatters.find(entityType); it != s_formatters.end())
        {
            it->second(format, entityText, entity);
            if (entityType == "textEntityTypePre")
            {
                removeLineBreakAfterCodeBlock = true;
            }
        }

        m_cursor->insertText(entityText, format);
        currentIndex = offset + length;
    }

    if (currentIndex < text.length())
    {
        insertTextSegment(text.mid(currentIndex));
    }

    emit textChanged();
}
