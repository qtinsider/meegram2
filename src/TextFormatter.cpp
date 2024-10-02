#include "TextFormatter.hpp"

#include <QTextCharFormat>

// clang-format off
const std::unordered_map<int, std::function<void(QTextCharFormat &, const QString &, const td::td_api::TextEntityType &)>> TextFormatter::s_formatters = {
    {td::td_api::textEntityTypeBold::ID, [](QTextCharFormat &format, const QString &, const td::td_api::TextEntityType &) { format.setFontWeight(QFont::Bold); }},
    {td::td_api::textEntityTypeItalic::ID, [](QTextCharFormat &format, const QString &, const td::td_api::TextEntityType &) { format.setFontItalic(true); }},
    {td::td_api::textEntityTypeUnderline::ID, [](QTextCharFormat &format, const QString &, const td::td_api::TextEntityType &) { format.setFontUnderline(true); }},
    {td::td_api::textEntityTypeStrikethrough::ID, [](QTextCharFormat &format, const QString &, const td::td_api::TextEntityType &) { format.setFontStrikeOut(true); }},
    {td::td_api::textEntityTypeCode::ID, [](QTextCharFormat &format, const QString &, const td::td_api::TextEntityType &) { format.setFontFamily("Courier"); }},
    {td::td_api::textEntityTypePre::ID, [](QTextCharFormat &format, const QString &, const td::td_api::TextEntityType &) { format.setFontFamily("Courier"); }},
    {td::td_api::textEntityTypeTextUrl::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &type) {
         QString url = QString::fromStdString( static_cast<const td::td_api::textEntityTypeTextUrl &>(type).url_);
         if (url.isEmpty())
         {
             url = entityText;
         }
         format.setAnchor(true);
         format.setAnchorHref(url);
         format.setFontUnderline(true);
     }},
    {td::td_api::textEntityTypeUrl::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &) {
         format.setAnchor(true);
         format.setAnchorHref(entityText);
         format.setFontUnderline(true);
     }},
    {td::td_api::textEntityTypeEmailAddress::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &) {
         format.setAnchor(true);
         format.setAnchorHref("mailto:" + entityText);
     }},
    {td::td_api::textEntityTypePhoneNumber::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &) {
         format.setAnchor(true);
         format.setAnchorHref("tel:" + entityText);
     }},
    {td::td_api::textEntityTypeMention::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &) {
         format.setAnchor(true);
         format.setAnchorHref("mention:" + entityText);
     }},
    {td::td_api::textEntityTypeMentionName::ID, [](QTextCharFormat &format, const QString &, const td::td_api::TextEntityType &entity) {
         format.setAnchor(true);
         format.setAnchorHref("mention_name:" + QString::number(static_cast<const td::td_api::textEntityTypeMentionName &>(entity).user_id_));
     }},
    {td::td_api::textEntityTypeHashtag::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &) {
         format.setAnchor(true);
         format.setAnchorHref("hashtag:" + entityText);
     }},
    {td::td_api::textEntityTypeCashtag::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &) {
         format.setAnchor(true);
         format.setAnchorHref("cashtag:" + entityText);
     }},
    {td::td_api::textEntityTypeBotCommand::ID, [](QTextCharFormat &format, const QString &entityText, const td::td_api::TextEntityType &) {
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
        m_document->setHtml(m_formattedText.toString());
        emit textChanged();
        return;
    }

    // Safely retrieve and cast the pointer from QVariant
    auto formattedText = static_cast<td::td_api::formattedText *>(m_formattedText.value<void *>());

    if (!formattedText || formattedText->text_.empty())
    {
        // Handle the case where formattedText is null or text_ is empty
        return;
    }

    const auto &text = QString::fromStdString(formattedText->text_);
    const auto &entities = formattedText->entities_;

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

    for (const auto &entity : entities)
    {
        const auto offset = entity->offset_;
        const auto length = entity->length_;
        const auto entityType = entity->type_->get_id();

        if (currentIndex > offset)
        {
            continue;
        }

        insertTextSegment(text.mid(currentIndex, offset - currentIndex));

        const auto entityText = text.mid(offset, length);
        QTextCharFormat format;

        if (auto it = s_formatters.find(entityType); it != s_formatters.end())
        {
            it->second(format, entityText, *entity->type_);
            if (entityType == td::td_api::textEntityTypePre::ID)
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
