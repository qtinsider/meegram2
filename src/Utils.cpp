#include "Utils.hpp"

#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"

#include <QStringBuilder>
#include <QTextStream>

Utils::Utils(QObject *parent)
    : QObject(parent)
    , m_locale(&Locale::instance())
    , m_storageManager(&StorageManager::instance())
{
}

td::td_api::object_ptr<td::td_api::ChatList> Utils::toChatList(const std::unique_ptr<ChatList> &list) noexcept
{
    switch (list->type())
    {
        case ChatList::Archive:
            return td::td_api::make_object<td::td_api::chatListArchive>();
        case ChatList::Folder:
            return td::td_api::make_object<td::td_api::chatListFolder>(list->folderId());
        default:
            return td::td_api::make_object<td::td_api::chatListMain>();
    }
}

QString Utils::formattedText(const td::td_api::object_ptr<td::td_api::formattedText> &value) noexcept
{
    if (!value)
    {
        return {};
    }

    const auto text = QString::fromStdString(value->text_);
    const auto &entities = value->entities_;

    QString html;
    html.reserve(int(text.length() * 1.1) + entities.size() * 10);  // Rough estimate

    int currentIndex = 0;

    auto escapeHtml = [](const QString &plain) {
        QString rich;
        rich.reserve(plain.size() * 1.1);

        for (const QChar &ch : plain)
        {
            switch (ch.unicode())
            {
                case '<':
                    rich += QLatin1String("&lt;");
                    break;
                case '>':
                    rich += QLatin1String("&gt;");
                    break;
                case '&':
                    rich += QLatin1String("&amp;");
                    break;
                case '"':
                    rich += QLatin1String("&quot;");
                    break;
                default:
                    rich += ch;
                    break;
            }
        }
        return rich;
    };

    for (const auto &entity : entities)
    {
        int offset = entity->offset_;
        int length = entity->length_;
        int entityType = entity->type_->get_id();

        if (currentIndex < offset)
        {
            html += escapeHtml(text.midRef(currentIndex, offset - currentIndex).toString());
        }

        QString entityText = escapeHtml(text.midRef(offset, length).toString());

        // Use switch to add HTML tags around entity text
        switch (entityType)
        {
            case td::td_api::textEntityTypeBold::ID:
                html += QLatin1String("<b>") % entityText % QLatin1String("</b>");
                break;
            case td::td_api::textEntityTypeItalic::ID:
                html += QLatin1String("<i>") % entityText % QLatin1String("</i>");
                break;
            case td::td_api::textEntityTypeUnderline::ID:
                html += QLatin1String("<u>") % entityText % QLatin1String("</u>");
                break;
            case td::td_api::textEntityTypeStrikethrough::ID:
                html += QLatin1String("<s>") % entityText % QLatin1String("</s>");
                break;
            case td::td_api::textEntityTypeCode::ID:
            case td::td_api::textEntityTypePre::ID:
                html += QLatin1String("<code>") % entityText % QLatin1String("</code>");
                break;
            case td::td_api::textEntityTypeTextUrl::ID: {
                auto urlEntity = static_cast<const td::td_api::textEntityTypeTextUrl *>(entity->type_.get());
                QString url = QString::fromStdString(urlEntity->url_);
                html += QLatin1String("<a href=\"") % (url.isEmpty() ? entityText : url) % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            }
            case td::td_api::textEntityTypeUrl::ID:
                html += QLatin1String("<a href=\"") % entityText % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            case td::td_api::textEntityTypeEmailAddress::ID:
                html += QLatin1String("<a href=\"mailto:") % entityText % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            case td::td_api::textEntityTypePhoneNumber::ID:
                html += QLatin1String("<a href=\"tel:") % entityText % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            case td::td_api::textEntityTypeMention::ID:
                html += QLatin1String("<a href=\"mention:") % entityText % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            case td::td_api::textEntityTypeMentionName::ID: {
                auto mentionEntity = static_cast<const td::td_api::textEntityTypeMentionName *>(entity->type_.get());
                html += QLatin1String("<a href=\"mention_name:") % QString::number(mentionEntity->user_id_) % QLatin1String("\">") % entityText %
                        QLatin1String("</a>");
                break;
            }
            case td::td_api::textEntityTypeHashtag::ID:
                html += QLatin1String("<a href=\"hashtag:") % entityText % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            case td::td_api::textEntityTypeCashtag::ID:
                html += QLatin1String("<a href=\"cashtag:") % entityText % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            case td::td_api::textEntityTypeBotCommand::ID:
                html += QLatin1String("<a href=\"botCommand:") % entityText % QLatin1String("\">") % entityText % QLatin1String("</a>");
                break;
            default:
                html += entityText;
                break;
        }

        currentIndex = offset + length;
    }

    if (currentIndex < text.length())
    {
        html += escapeHtml(text.midRef(currentIndex).toString());
    }

    return html;
}

QString Utils::getChatTitle(qlonglong chatId, bool showSavedMessages) const noexcept
{
    auto chat = m_storageManager->getChat(chatId);

    if (isMeChat(chat) && showSavedMessages)
    {
        return QObject::tr("SavedMessages");
    }

    const auto title = chat->title().trimmed();

    return !title.isEmpty() ? title : QObject::tr("HiddenName");
}

QString Utils::formatTime(int totalSeconds) const noexcept
{
    QString result;
    QTextStream stream(&result);

    auto appendDuration = [&stream](int count, QChar order) {
        if (count > 0)
        {
            if (!stream.string()->isEmpty())
            {
                stream << ' ';
            }
            stream << count << order;
        }
    };

    const int seconds = totalSeconds % 60;
    const int totalMinutes = totalSeconds / 60;
    const int minutes = totalMinutes % 60;
    const int totalHours = totalMinutes / 60;
    const int hours = totalHours % 24;
    const int days = totalHours / 24;

    appendDuration(days, 'd');
    appendDuration(hours, 'h');
    appendDuration(minutes, 'm');
    appendDuration(seconds, 's');

    return result;
}

bool Utils::isMeChat(const std::shared_ptr<Chat> &chat) const noexcept
{
    if (!chat)  // Safety check for null pointers
        return false;

    const auto chatType = chat->type();
    const auto myId = m_storageManager->myId();

    if (chatType == Chat::Type::Secret)
    {
        return myId == chat->typeId();
    }
    else if (chatType == Chat::Type::Private)
    {
        return myId == chat->typeId();
    }

    return false;
}

bool Utils::isMeUser(qlonglong userId) const noexcept
{
    return m_storageManager->myId() == userId;
}
