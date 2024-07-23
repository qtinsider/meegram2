#include "TextFormatter.hpp"

#include <QTextCharFormat>
#include <QTextCursor>
#include <QTextDocument>

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

class FormatterFunctions
{
public:
    static const std::unordered_map<std::string, std::function<void(QTextCharFormat &, const QString &, const QVariantMap &)>> &
        getFormatters();
};

const std::unordered_map<std::string, std::function<void(QTextCharFormat &, const QString &, const QVariantMap &)>> &
    FormatterFunctions::getFormatters()
{
    static const std::unordered_map<std::string, std::function<void(QTextCharFormat &, const QString &, const QVariantMap &)>> formatters =
        {{"textEntityTypeBold", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontWeight(QFont::Bold); }},
         {"textEntityTypeBotCommand",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("botCommand:" + entityText);
          }},
         {"textEntityTypeEmailAddress",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("mailto:" + entityText);
          }},
         {"textEntityTypeItalic", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontItalic(true); }},
         {"textEntityTypeMentionName",
          [](QTextCharFormat &format, const QString &, const QVariantMap &type) {
              format.setAnchor(true);
              format.setAnchorHref("userId:" + type.value("user_id").toLongLong());
          }},
         {"textEntityTypeMention",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("username:" + entityText);
          }},
         {"textEntityTypePhoneNumber",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref("tel:" + entityText);
          }},
         {"textEntityTypeCode", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypePre", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypePreCode", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontFixedPitch(true); }},
         {"textEntityTypeStrikethrough",
          [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontStrikeOut(true); }},
         {"textEntityTypeTextUrl",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &type) {
              QString url = type.value("url").toString();
              if (url.isEmpty())
              {
                  url = entityText;
              }
              format.setAnchor(true);
              format.setAnchorHref(url);
              format.setFontUnderline(true);
          }},
         {"textEntityTypeUrl",
          [](QTextCharFormat &format, const QString &entityText, const QVariantMap &) {
              format.setAnchor(true);
              format.setAnchorHref(entityText);
              format.setFontUnderline(true);
          }},
         {"textEntityTypeUnderline", [](QTextCharFormat &format, const QString &, const QVariantMap &) { format.setFontUnderline(true); }}};
    return formatters;
}

TextFormatter::TextFormatter(QObject *parent)
    : QObject(parent)
    , m_document(0)
    , m_cursor(0)
{
    m_document = new QTextDocument();
    m_cursor = new QTextCursor(m_document);

    connect(m_document, SIGNAL(contentsChanged()), this, SIGNAL(richTextChanged()));
}

TextFormatter::~TextFormatter()
{
    delete m_cursor;
    delete m_document;
}

QString TextFormatter::richText() const
{
    return m_document->toHtml();
}

void TextFormatter::setRichText(const QString &value)
{
    if (value == richText())
        return;

    if (Qt::mightBeRichText(value))
        m_document->setHtml(value);
    else
        m_document->setHtml(Qt::convertFromPlainText(value, Qt::WhiteSpacePre));

    update();
}

QFont TextFormatter::defaultFont() const
{
    return m_document->defaultFont();
}

void TextFormatter::setDefaultFont(const QFont &font)
{
    if (font != defaultFont())
    {
        m_document->setDefaultFont(font);

        emit defaultFontChanged();
    }
}

QVariant TextFormatter::formattedText() const
{
    return m_formattedText;
}

void TextFormatter::setFormattedText(const QVariant &formattedText)
{
    m_formattedText = formattedText;
    emit formattedTextChanged();
}

void TextFormatter::update()
{
    const auto &formatters = FormatterFunctions::getFormatters();

    for (const auto &entityVariant : m_entities)
    {
        const auto entity = entityVariant.toMap();
        const auto offset = entity.value("offset").toInt();
        const auto length = entity.value("length").toInt();
        const auto type = entity.value("type").toMap();
        const auto entityType = type.value("@type").toString().toStdString();
        const auto entityText = m_text.mid(offset, length);

        m_cursor.setPosition(offset);
        m_cursor.setPosition(offset + length, QTextCursor::KeepAnchor);

        QTextCharFormat format;

        if (auto it = formatters.find(entityType); it != formatters.end())
        {
            it->second(format, entityText, type);
            m_cursor.mergeCharFormat(format);
        }
    }
}
