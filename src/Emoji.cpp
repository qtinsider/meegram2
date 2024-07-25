#include "Emoji.hpp"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QDir>
#include <QPainter>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextLayout>

EmojiTextObjectInterface::EmojiTextObjectInterface(EmojiTextObject *emojiTextObject)
    : QObject(emojiTextObject)
    , m_emojiTextObject(emojiTextObject)
{
}

void EmojiTextObjectInterface::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument,
                                          const QTextFormat &format)
{
    Q_UNUSED(doc);
    Q_UNUSED(posInDocument);

    const auto code = format.property(EmojiTextObject::EmojiUnicode).value<Emoji>();
    const auto drawRect = rect.adjusted(1, 0, -1, 0);

    painter->save();

    if (!m_emojiTextObject->hasEmoticon(code))
    {
        painter->setPen(QPen(Qt::darkGray, 1, Qt::SolidLine));
        painter->setBrush(QColor(240, 240, 240));
        painter->drawRect(drawRect.adjusted(1, 0, -1, 0));  // adjust again
        painter->setFont(QFont("typewriter", qMax(6, int(drawRect.height() * 0.6))));
        painter->drawText(drawRect, Qt::AlignCenter, QLatin1String("?"));
    }
    else
    {
        const QPixmap pix = m_emojiTextObject->getEmoticon(code, drawRect.size().toSize());

        painter->setPen(Qt::NoPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPixmap(drawRect, pix, QRectF(QPointF(0, 0), pix.size()));
    }

    painter->restore();
}

QSizeF EmojiTextObjectInterface::intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format)
{
    int height = m_emojiTextObject->computeLineHeight(posInDocument, format);
    QSize size(height + 2, height);
    if (m_emojiTextObject->m_minimumEmojiSize.isValid())
        size = size.expandedTo(m_emojiTextObject->m_minimumEmojiSize);

    return size;
}

EmojiTextObject::EmojiTextObject(QTextDocument *document)
    : m_textDocument(document)
    , m_maxEmojiCharCodeCount(8)
    , m_minimumEmojiSize(20, 20)
    , m_isApplyingFormats(false)
{
    connect(document, &QTextDocument::contentsChanged, this, &EmojiTextObject::onTextDocumentContentsChanged);

    auto *textObjectInterface = new EmojiTextObjectInterface(this);
    m_textDocument->documentLayout()->registerHandler(EmojiTextObject::EmojiTextFormatObjectType, textObjectInterface);
    applyTextCharFormat();
}

QString EmojiTextObject::getEmojiString(const Emoji &code)
{
    QString emojiStr;
    emojiStr.reserve(code.size() * 2);  // Reserve space to avoid multiple allocations
    for (uint character : code)
    {
        emojiStr += getEmojiString(character);
    }
    return emojiStr;
}

QString EmojiTextObject::getEmojiString(uint character)
{
    QString emojiStr;
    if (QChar::requiresSurrogates(character))
    {
        QChar chars[] = {QChar(QChar::highSurrogate(character)), QChar(QChar::lowSurrogate(character))};
        emojiStr = QString(chars, 2);
    }
    else
    {
        emojiStr = QChar(character);
    }
    return emojiStr;
}

void EmojiTextObject::onTextDocumentContentsChanged(int position, int /*charsRemoved*/, int charsAdded)
{
    if (charsAdded > 0)
    {
        applyTextCharFormat(position);
    }
}

bool EmojiTextObject::isEmoji(uint character) const
{
    return isEmoji(Emoji() << character);
}

bool EmojiTextObject::isEmoji(const Emoji &code) const
{
    return !code.isEmpty() && AllEmojis.contains(code);
}

Emoji EmojiTextObject::extractEmojiCode(QTextCursor cursor, int *selectionLength)
{
    if (selectionLength)
    {
        *selectionLength = 0;
    }

    if (cursor.isNull() || cursor.atEnd())
    {
        return Emoji();
    }

    int start = cursor.position();
    cursor.setPosition(start, QTextCursor::MoveAnchor);
    int end = start;

    while (cursor.selectedText().length() < (m_maxEmojiCharCodeCount * 2))
    {
        if (!cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1))
        {
            break;
        }
        end = cursor.position();
    }

    while (end > start)
    {
        cursor.setPosition(start, QTextCursor::MoveAnchor);
        cursor.setPosition(end, QTextCursor::KeepAnchor);

        const auto selection = cursor.selectedText();

        Emoji code;
        code.reserve(selection.length());  // Reserve space to avoid multiple allocations

        for (int i = 0; i < selection.length(); ++i)
        {
            const ushort prevCharacter = i > 0 ? selection[i - 1].unicode() : 0x0;
            const ushort unicodeCharacter = selection[i].unicode();
            uint emojiCharCode = 0x0;

            if (QChar::isHighSurrogate(unicodeCharacter))
            {
                continue;
            }

            if (QChar::isLowSurrogate(unicodeCharacter) && QChar::isHighSurrogate(prevCharacter))
            {
                emojiCharCode = QChar::surrogateToUcs4(prevCharacter, unicodeCharacter);
            }
            else
            {
                emojiCharCode = unicodeCharacter;
            }

            if (emojiCharCode > 0)
            {
                code << emojiCharCode;
            }
            else
            {
                break;
            }
        }

        if (isEmoji(code))
        {
            if (selectionLength)
            {
                *selectionLength = selection.length();
            }
            return code;
        }

        if (!cursor.movePosition(QTextCursor::PreviousCharacter, QTextCursor::KeepAnchor, 1))
        {
            break;
        }
        end = cursor.position();
    }

    return Emoji();
}

QString EmojiTextObject::extractDocumentText(bool html) const
{
    if (!m_textDocument)
    {
        return QString();
    }

    QScopedPointer<QTextDocument> doc(m_textDocument->clone());
    QTextCursor cursor(doc);

    cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    while (!cursor.atEnd())
    {
        if (!cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1))
        {
            break;
        }
        const auto selection = cursor.selectedText().trimmed();
        if (selection == QString(QChar::ObjectReplacementCharacter))
        {
            const QTextCharFormat format = cursor.charFormat();
            if (format.objectType() == EmojiTextObject::EmojiTextFormatObjectType)
            {
                const auto code = format.property(EmojiTextObject::EmojiUnicode).value<Emoji>();
                const auto replaceText = format.stringProperty(EmojiTextObject::EmojiString);

                cursor.insertText(replaceText);
            }
        }

        cursor.setPosition(cursor.position(), QTextCursor::MoveAnchor);
    }

    QString text = html ? doc->toHtml() : doc->toPlainText();

    return text;
}

QString EmojiTextObject::extractCursorText(const QTextCursor &cursor, bool html) const
{
    if (!cursor.document() || !cursor.hasSelection())
    {
        return QString();
    }

    QScopedPointer<QTextDocument> doc(m_textDocument->clone());
    QTextCursor cursorCopy(doc);
    cursorCopy.setPosition(qMin(cursor.position(), cursor.anchor()), QTextCursor::MoveAnchor);
    cursorCopy.setPosition(qMax(cursor.position(), cursor.anchor()), QTextCursor::KeepAnchor);

    int start = cursorCopy.anchor();
    int end = cursorCopy.position();

    cursorCopy.setPosition(start, QTextCursor::MoveAnchor);
    while (cursorCopy.position() <= end)
    {
        if (!cursorCopy.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1))
        {
            break;
        }
        const QString selection = cursorCopy.selectedText().trimmed();
        if (selection == QString(QChar::ObjectReplacementCharacter))
        {
            const QTextCharFormat format = cursorCopy.charFormat();
            if (format.objectType() == EmojiTextObject::EmojiTextFormatObjectType)
            {
                const auto code = format.property(EmojiTextObject::EmojiUnicode).value<Emoji>();

                const QString replaceText = format.stringProperty(EmojiTextObject::EmojiString);

                end += (replaceText.length() - 1);
                cursorCopy.insertText(replaceText, QTextCharFormat());
            }
        }

        cursorCopy.setPosition(cursorCopy.position(), QTextCursor::MoveAnchor);
    }

    cursorCopy.setPosition(start, QTextCursor::MoveAnchor);
    cursorCopy.setPosition(end, QTextCursor::KeepAnchor);
    QTextDocumentFragment fragment = cursorCopy.selection();
    QString text = html ? fragment.toHtml() : fragment.toPlainText();

    return text;
}

void EmojiTextObject::applyTextCharFormat(int pos)
{
    if (m_isApplyingFormats)
    {
        return;
    }

    m_isApplyingFormats = true;
    const bool isModified = m_textDocument->isModified();

    QTextCursor cursor(m_textDocument);
    if (pos == 0)
    {
        cursor.movePosition(QTextCursor::Start, QTextCursor::MoveAnchor);
    }
    else
    {
        cursor.setPosition(pos, QTextCursor::MoveAnchor);
    }

    cursor.beginEditBlock();
    while (!cursor.atEnd())
    {
        int increase = 1;

        if (const auto code = extractEmojiCode(cursor, &increase); !code.isEmpty())
        {
            QTextCharFormat emojiFormat;
            emojiFormat.setObjectType(EmojiTextObject::EmojiTextFormatObjectType);
            emojiFormat.setProperty(EmojiTextObject::EmojiUnicode, QVariant::fromValue<Emoji>(code));
            emojiFormat.setProperty(EmojiTextObject::EmojiString, QVariant::fromValue<QString>(getEmojiString(code)));
            emojiFormat.setVerticalAlignment(QTextCharFormat::AlignBottom);

            cursor.setPosition(cursor.position() + increase, QTextCursor::KeepAnchor);
            cursor.insertText(QString(QChar::ObjectReplacementCharacter), emojiFormat);
        }
        else if (!cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::MoveAnchor, 1))
        {
            break;
        }
    }
    cursor.endEditBlock();

    m_textDocument->setModified(isModified);
    m_isApplyingFormats = false;
}
