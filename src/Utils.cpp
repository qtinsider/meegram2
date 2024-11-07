#include "Utils.hpp"

#include "StorageManager.hpp"

#include <QStringBuilder>
#include <QTextStream>

Utils::Utils(QObject *parent)
    : QObject(parent)
{
}

td::td_api::object_ptr<td::td_api::ChatList> Utils::toChatList(const std::unique_ptr<ChatList> &list) noexcept
{
    switch (list->type())
    {
        case ChatList::Type::Main:
            return td::td_api::make_object<td::td_api::chatListMain>();
        case ChatList::Type::Archive:
            return td::td_api::make_object<td::td_api::chatListArchive>();
        case ChatList::Type::Folder:
            return td::td_api::make_object<td::td_api::chatListFolder>(list->folderId());
        default:
            // Fallback to nullptr for None or any unexpected type
            return nullptr;
    }
}

QString Utils::formattedText(const td::td_api::object_ptr<td::td_api::formattedText> &value) noexcept
{
    if (!value)
        return {};

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

QString Utils::formatTime(int totalSeconds) noexcept
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

QString Utils::getChatTitle(qlonglong chatId, bool showSavedMessages) noexcept
{
    auto chat = StorageManager::instance().getChat(chatId);

    if (isMeChat(chat) && showSavedMessages)
    {
        return QObject::tr("SavedMessages");
    }

    const auto title = chat->title().trimmed();

    return !title.isEmpty() ? title : QObject::tr("HiddenName");
}

QString Utils::getUserName(qlonglong userId, bool openUser) noexcept
{
    const auto userName = getUserShortName(userId);

    if (userName.isEmpty())
    {
        return QString();
    }

    if (openUser)
    {
        const QString linkStart = QLatin1String("<a style=\"text-decoration: none; font-weight: bold; color: darkgray\" href=\"userId://");
        const QString linkEnd = QLatin1String("</a>");
        return linkStart + QString::number(userId) + QLatin1String("\">") + userName + linkEnd;
    }

    return userName;
}

QString Utils::getUserShortName(qlonglong userId) noexcept
{
    auto user = StorageManager::instance().getUser(userId);
    if (!user)
        return QString();

    switch (user->type())
    {
        case User::Type::Bot:
        case User::Type::Regular: {
            if (!user->firstName().isEmpty())
            {
                return user->firstName();
            }
            if (!user->lastName().isEmpty())
            {
                return user->lastName();
            }
            break;
        }
        case User::Type::Deleted:
        case User::Type::Unknown:
            return tr("HiddenName");
        default:
            return QString();
    }

    return QString();
}

QString Utils::getUserFullName(qlonglong userId) noexcept
{
    auto user = StorageManager::instance().getUser(userId);
    if (!user)
        return QString();

    switch (user->type())
    {
        case User::Type::Bot:
        case User::Type::Regular:
            return QString(user->firstName() + " " + user->lastName()).trimmed();

        case User::Type::Deleted:
        case User::Type::Unknown:
            return tr("HiddenName");

        default:
            return QString();
    }
}

bool Utils::isMeUser(qlonglong userId) noexcept
{
    return StorageManager::instance().myId() == userId;
}

bool Utils::isMeChat(const std::shared_ptr<Chat> &chat) noexcept
{
    if (!chat)  // Safety check for null pointers
        return false;

    const auto chatType = chat->type();
    const auto myId = StorageManager::instance().myId();

    switch (chatType)
    {
        case Chat::Type::Secret:
            return myId == chat->typeId();

        case Chat::Type::Private:
            return myId == chat->typeId();

        default:
            return {};
    }

    return false;
}
