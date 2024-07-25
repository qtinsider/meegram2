#pragma once

#include <QTextObjectInterface>

#include <unordered_set>
#include <vector>

class EmojiTextObject;

using Emoji = std::vector<uint>;
using EmojiSet = std::unordered_set<Emoji>;

class EmojiTextObjectInterface : public QObject, public QTextObjectInterface
{
    Q_OBJECT
    Q_INTERFACES(QTextObjectInterface)

public:
    explicit EmojiTextObjectInterface(EmojiTextObject *emojiTextObject);

    // QTextObjectInterface interface
    virtual void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc, int posInDocument,
                            const QTextFormat &format) override;
    virtual QSizeF intrinsicSize(QTextDocument *doc, int posInDocument, const QTextFormat &format) override;

private:
    EmojiTextObject *m_emojiTextObject;
};

class EmojiTextObject : public QObject
{
    Q_OBJECT
public:
    explicit EmojiTextObject(QTextDocument *textDocument);
    ~EmojiTextObject() override = default;

    enum TextFormatProperty { EmojiUnicode = QTextFormat::UserProperty, EmojiImagePath, EmojiString };
    Q_ENUMS(TextFormatProperty)

    [[nodiscard]] static QString getEmojiString(const Emoji &emojiCode);
    [[nodiscard]] static QString getEmojiString(uint character);

protected:
    static const EmojiSet AllEmojis;
    static constexpr int EmojiTextFormatObjectType = QTextFormat::UserObject + 0xEEEE;

    [[nodiscard]] bool isEmoji(uint character) const;
    [[nodiscard]] bool isEmoji(const Emoji &emojiCode) const;

    [[nodiscard]] Emoji extractEmojiCode(const QTextCursor &cursor, int *selectionLength) const;

    [[nodiscard]] QString extractDocumentText(bool html = false) const;
    [[nodiscard]] QString extractCursorText(const QTextCursor &cursor, bool html = false) const;

    void applyTextCharFormat(int pos = 0);

    [[nodiscard]] int computeLineHeight(int posInDocument, const QTextFormat &format) const;

protected slots:
    void onTextDocumentContentsChanged(int position, int charsRemoved, int charsAdded);

private:
    QTextDocument *m_textDocument = nullptr;
    QSize m_minimumEmojiSize;

    quint8 m_maxEmojiCharCodeCount = 0;

    bool m_isApplyingFormats = false;
};
