#include "Emoji.hpp"

#include <QAbstractTextDocumentLayout>
#include <QDebug>
#include <QDir>
#include <QDomDocument>
#include <QFile>
#include <QPainter>
#include <QPixmapCache>
#include <QSvgRenderer>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextLayout>

#include <algorithm>
#include <memory>
#include <ranges>

EmojiTextObjectInterface::EmojiTextObjectInterface(EmojiTextObject *emojiTextObject)
    : QObject(emojiTextObject)
    , m_emojiTextObject(emojiTextObject)
{
}

void EmojiTextObjectInterface::drawObject(QPainter *painter, const QRectF &rect, QTextDocument *, int, const QTextFormat &format)
{
    const auto code = format.property(EmojiTextObject::EmojiUnicode).value<Emoji>();
    const auto drawRect = rect.adjusted(1, 0, -1, 0);

    painter->save();

    if (!m_emojiTextObject->hasEmoticon(code))
    {
        painter->setPen(QPen(Qt::darkGray, 1, Qt::SolidLine));
        painter->setBrush(QColor(240, 240, 240));
        painter->drawRect(drawRect);
        painter->setFont(QFont("typewriter", qMax(6, int(drawRect.height() * 0.6))));
        painter->drawText(drawRect, Qt::AlignCenter, "?");
    }
    else
    {
        const auto pix = m_emojiTextObject->getEmoji(code, drawRect.size().toSize());
        painter->drawPixmap(drawRect, pix, QRectF(QPointF(0, 0), pix.size()));
    }

    painter->restore();
}

QSizeF EmojiTextObjectInterface::intrinsicSize(QTextDocument *, int posInDocument, const QTextFormat &format)
{
    auto height = m_emojiTextObject->computeLineHeight(posInDocument, format);

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
    connect(document, SIGNAL(contentsChange(int, int, int)), this, SLOT(onTextDocumentContentsChanged(int, int, int)));

    auto *textObjectInterface = new EmojiTextObjectInterface(this);
    m_textDocument->documentLayout()->registerHandler(EmojiTextObject::EmojiTextFormatObjectType, textObjectInterface);
    applyTextCharFormat();
}

QString EmojiTextObject::getEmojiString(const Emoji &code)
{
    QString emojiStr;
    for (auto character : code)
    {
        emojiStr += getEmojiString(character);
    }
    return emojiStr;
}

QString EmojiTextObject::getEmojiString(unsigned int character)
{
    if (QChar::requiresSurrogates(character))
    {
        return QString(QChar::highSurrogate(character)) + QString(QChar::lowSurrogate(character));
    }
    return QString(QChar(character));
}

QString EmojiTextObject::getEmojiPath(const Emoji &code) const
{
    auto simplifiedCode = code | std::views::filter([](unsigned int ch) { return ch != 0xFE0E && ch != 0xFE0F; });

    QString hexString;
    hexString.reserve(std::ranges::distance(simplifiedCode) * 6);  // 5 hex digits + 1 hyphen per character

    for (auto it = simplifiedCode.begin(); it != simplifiedCode.end(); ++it)
    {
        if (it != simplifiedCode.begin())
        {
            hexString += QLatin1Char('-');
        }
        hexString += QString("%1").arg(*it, 5, 16, QLatin1Char('0'));
    }

    return QString(":/emoji/") + hexString + ".svg";
}

bool EmojiTextObject::hasEmoticon(const Emoji &code) const
{
    const auto fileName = getEmojiPath(code);
    return QFile::exists(fileName);
}

QPixmap EmojiTextObject::getEmoji(const Emoji &code, const QSize &size) const
{
    const auto fileName = getEmojiPath(code);
    auto pix = svgToPixmap(fileName, size);
    return pix;
}

void EmojiTextObject::onTextDocumentContentsChanged(int position, int, int charsAdded)
{
    if (charsAdded > 0)
    {
        applyTextCharFormat(position);
    }
}

bool EmojiTextObject::isEmoji(uint character) const
{
    return isEmoji(Emoji{character});
}

bool EmojiTextObject::isEmoji(const Emoji &code) const
{
    return !code.empty() && AllEmojis.contains(code);
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

    cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, m_maxEmojiCharCodeCount);

    while (cursor.hasSelection())
    {
        Emoji code;
        const auto selection = cursor.selectedText();
        for (int i = 0; i < selection.length(); ++i)
        {
            auto emojiCharCode = selection[i].unicode();
            if (i > 0 && QChar::isLowSurrogate(emojiCharCode) && QChar::isHighSurrogate(selection[i - 1].unicode()))
            {
                emojiCharCode = QChar::surrogateToUcs4(selection[i - 1].unicode(), emojiCharCode);
            }
            code.push_back(emojiCharCode);
        }

        if (isEmoji(code))
        {
            if (selectionLength)
            {
                *selectionLength = selection.length();
            }
            return code;
        }

        cursor.setPosition(cursor.position() - 1, QTextCursor::KeepAnchor);
    }

    return Emoji();
}

QString EmojiTextObject::extractDocumentText(bool html) const
{
    if (!m_textDocument)
    {
        return QString();
    }

    auto doc = std::make_unique<QTextDocument>(m_textDocument->clone());
    QTextCursor cursor(doc.get());

    while (!cursor.atEnd())
    {
        cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
        if (cursor.selectedText() == QString(QChar::ObjectReplacementCharacter))
        {
            const QTextCharFormat format = cursor.charFormat();
            if (format.objectType() == EmojiTextObject::EmojiTextFormatObjectType)
            {
                const QString replaceText = format.stringProperty(EmojiTextObject::EmojiString);
                cursor.insertText(replaceText);
            }
        }
    }

    return html ? doc->toHtml() : doc->toPlainText();
}

QString EmojiTextObject::extractCursorText(const QTextCursor &cursor, bool html) const
{
    if (!cursor.document() || !cursor.hasSelection())
    {
        return QString();
    }

    auto doc = std::make_unique<QTextDocument>(m_textDocument->clone());
    QTextCursor cursorCopy(doc.get());
    cursorCopy.setPosition(qMin(cursor.position(), cursor.anchor()), QTextCursor::MoveAnchor);
    cursorCopy.setPosition(qMax(cursor.position(), cursor.anchor()), QTextCursor::KeepAnchor);

    while (!cursorCopy.atEnd())
    {
        cursorCopy.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, 1);
        if (cursorCopy.selectedText() == QString(QChar::ObjectReplacementCharacter))
        {
            const QTextCharFormat format = cursorCopy.charFormat();
            if (format.objectType() == EmojiTextObject::EmojiTextFormatObjectType)
            {
                const QString replaceText = format.stringProperty(EmojiTextObject::EmojiString);
                cursorCopy.insertText(replaceText);
            }
        }
    }

    return html ? doc->toHtml() : doc->toPlainText();
}

QByteArray EmojiTextObject::svgPrepare(const QString &fileName) const
{
    QDomDocument svgDocument;
    QFile svgFile(fileName);
    if (!svgFile.open(QIODevice::ReadOnly) || !svgDocument.setContent(&svgFile))
        return QByteArray();

    /*
     * QSvgRenderer fills certain elements with "black" if they are not
     * explicitly filled
     */

    const auto fillAttribute = QLatin1String("fill");
    const auto transparentFillValue = QLatin1String("transparent");

    auto applyTransparentFill = [&](const QString &elementTag) {
        auto elements = svgDocument.elementsByTagName(elementTag);
        for (int i = 0; i < elements.size(); ++i)
        {
            if (auto element = elements.at(i).toElement(); !element.hasAttribute(fillAttribute))
                element.setAttribute(fillAttribute, transparentFillValue);
        }
    };

    applyTransparentFill(QLatin1String("path"));
    applyTransparentFill(QLatin1String("rect"));

    return svgDocument.toByteArray();
}

QPixmap EmojiTextObject::svgToPixmap(const QString &fileName, const QSize &size) const
{
    QPixmap pixmap;
    const auto cacheKey = fileName + QString("@%1x%2").arg(size.width()).arg(size.height());
    if (!QPixmapCache::find(cacheKey, &pixmap))
    {
        if (QFile::exists(fileName))
        {
            const auto svg = svgPrepare(fileName);

            if (QSvgRenderer renderer(svg); renderer.isValid())
            {
                pixmap = QPixmap(size);
                pixmap.fill(Qt::transparent);

                QPainter painter(&pixmap);
                renderer.render(&painter, QRectF());

                painter.end();

                QPixmapCache::insert(cacheKey, pixmap);
            }
        }
    }
    return pixmap;
}

void EmojiTextObject::applyTextCharFormat(int startPosition)
{
    if (!m_textDocument || m_isApplyingFormats)
    {
        return;
    }

    m_isApplyingFormats = true;

    QTextCursor cursor(m_textDocument);
    cursor.setPosition(startPosition, QTextCursor::MoveAnchor);
    const bool isModified = m_textDocument->isModified();
    cursor.beginEditBlock();

    while (!cursor.atEnd())
    {
        int increase = 0;
        const auto code = extractEmojiCode(cursor, &increase);
        if (!code.empty())
        {
            QTextCharFormat format;
            format.setObjectType(EmojiTextObject::EmojiTextFormatObjectType);
            format.setProperty(EmojiTextObject::EmojiUnicode, QVariant::fromValue(code));
            format.setProperty(EmojiTextObject::EmojiString, getEmojiString(code));

            cursor.insertText(QString(QChar::ObjectReplacementCharacter), format);
        }
        else
        {
            cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, increase);
        }
    }

    cursor.endEditBlock();
    m_textDocument->setModified(isModified);

    m_isApplyingFormats = false;
}

int EmojiTextObject::computeLineHeight(int posInDocument, const QTextFormat &format) const
{
    auto calculateBlockHeight = [&](const QTextBlockFormat &) {
        QTextBlock block = m_textDocument->findBlock(posInDocument);
        int blockHeight = qRound(block.layout()->lineAt(0).height());
        qDebug() << blockHeight;
        return blockHeight;
    };

    auto calculateCharHeight = [&](const QTextCharFormat &charFormat) {
        QFont font = charFormat.font();
        int charHeight = font.pixelSize();
        if (charHeight == -1)
            charHeight = qRound(font.pointSizeF() * 1.33333);  // pt -> px
        return charHeight;
    };

    switch (format.type())
    {
        case QTextFormat::BlockFormat:
            return calculateBlockHeight(format.toBlockFormat());
        case QTextFormat::CharFormat:
            return calculateCharHeight(format.toCharFormat());
        default:
            return 20;
    }
}
