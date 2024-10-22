#include "Utils.hpp"

#include "Common.hpp"
#include "Localization.hpp"
#include "StorageManager.hpp"

#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QDateTime>
#include <QImageReader>
#include <QLocale>
#include <QScopedPointer>
#include <QStringList>
#include <QStringRef>
#include <QTextStream>

#include <algorithm>
#include <cmath>
#include <functional>
#include <unordered_map>
#include <unordered_set>

// namespace {

// bool isBotUser(const td::td_api::user *user) noexcept
// {
//     return user->type_->get_id() == td::td_api::userTypeBot::ID;
// }

// bool isMeUser(qlonglong userId, StorageManager *store) noexcept
// {
//     return store->myId() == userId;
// }

// bool isUserOnline(const td::td_api::user *user) noexcept
// {
//     if (std::ranges::any_of(ServiceNotificationsUserIds, [user](auto id) { return id == user->id_; }))
//     {
//         return false;
//     }

//     return user->status_->get_id() == td::td_api::userStatusOnline::ID && user->type_->get_id() != td::td_api::userTypeBot::ID;
// }

// bool isChannelChat(const td::td_api::chat *chat) noexcept
// {
//     if (chat->type_->get_id() == td::td_api::chatTypeSupergroup::ID)
//     {
//         return static_cast<const td::td_api::chatTypeSupergroup *>(chat->type_.get())->is_channel_;
//     }

//     return false;
// }

// bool isSupergroup(const td::td_api::chat *chat) noexcept
// {
//     return chat->type_->get_id() == td::td_api::chatTypeSupergroup::ID;
// }

// QString getMessageAuthor(const td::td_api::message &message, StorageManager *store, bool openUser) noexcept
// {
//     const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());
//     const QString linkStyle = "<a style=\"text-decoration: none; font-weight: bold; color: grey\" href=\"";
//     const QString linkClose = "</a>";

//     if (sender->get_id() == td::td_api::messageSenderUser::ID)
//     {
//         const auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
//         const auto userName = Utils::getUserShortName(userId, store);

//         if (openUser)
//         {
//             return linkStyle + "userId://" + QString::number(userId) + "\">" + userName + linkClose;
//         }
//         return userName;
//     }
//     else if (sender->get_id() == td::td_api::messageSenderChat::ID)
//     {
//         const auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
//         const auto chatTitle = QString::fromStdString(store->getChat(chatId)->title_);

//         if (openUser)
//         {
//             return linkStyle + "chatId://" + QString::number(chatId) + "\">" + chatTitle + linkClose;
//         }
//         return chatTitle;
//     }

//     return {};
// }

// QString getUserFullName(qlonglong userId, StorageManager *store) noexcept
// {
//     const auto user = store->getUser(userId);

//     switch (user->type_->get_id())
//     {
//         case td::td_api::userTypeBot::ID:
//         case td::td_api::userTypeRegular::ID: {
//             return QString::fromStdString(user->first_name_ + " " + user->last_name_).trimmed();
//         }
//         case td::td_api::userTypeDeleted::ID:
//         case td::td_api::userTypeUnknown::ID: {
//             return QObject::tr("HiddenName");
//         }

//         default:
//             return QString();
//     }
// }

// QString getUserName(auto userId, StorageManager *store, bool openUser)
// {
//     const auto userName = Utils::getUserShortName(userId, store);
//     if (userName.isEmpty())
//     {
//         return QString();
//     }

//     QString result;
//     if (openUser)
//     {
//         result = QString("<a style=\"text-decoration: none; font-weight: bold; color: grey\" href=\"userId://%1\">%2</a>").arg(userId).arg(userName);
//     }
//     else
//     {
//         result = QString("<span style=\"color: grey\">%1</span>").arg(userName);
//     }

//     return result;
// }

// QString getCallContent(qlonglong userId, const td::td_api::messageCall &content, StorageManager *store) noexcept
// {
//     const auto isVideo = content.is_video_;
//     const auto &discardReason = content.discard_reason_;
//     const auto isMissed = discardReason->get_id() == td::td_api::callDiscardReasonMissed::ID;
//     const auto isBusy = discardReason->get_id() == td::td_api::callDiscardReasonDeclined::ID;
//     const auto isOutgoing = isMeUser(userId, store);

//     if (isMissed)
//     {
//         if (isOutgoing)
//         {
//             return isVideo ? QObject::tr("CallMessageVideoOutgoingMissed") : QObject::tr("CallMessageOutgoingMissed");
//         }
//         return isVideo ? QObject::tr("CallMessageVideoIncomingMissed") : QObject::tr("CallMessageIncomingMissed");
//     }

//     if (isBusy)
//     {
//         return isVideo ? QObject::tr("CallMessageVideoIncomingDeclined") : QObject::tr("CallMessageIncomingDeclined");
//     }

//     return isVideo ? (isOutgoing ? QObject::tr("CallMessageVideoOutgoing") : QObject::tr("CallMessageVideoIncoming"))
//                    : (isOutgoing ? QObject::tr("CallMessageOutgoing") : QObject::tr("CallMessageIncoming"));
// }

// QString getAudioTitle(const td::td_api::audio &audio) noexcept
// {
//     const auto fileName = QString::fromStdString(audio.file_name_);
//     const auto title = QString::fromStdString(audio.title_).trimmed();
//     const auto performer = QString::fromStdString(audio.performer_).trimmed();

//     if (title.isEmpty() && performer.isEmpty())
//         return fileName;

//     const auto artist = performer.isEmpty() ? "Unknown Artist" : performer;
//     const auto track = title.isEmpty() ? "Unknown Track" : title;

//     return artist + " - " + track;
// }

// bool isUserBlocked(qlonglong userId, StorageManager *store) noexcept
// {
//     const auto fullInfo = store->getUserFullInfo(userId);

//     return !fullInfo /* && fullInfo.value("is_blocked").toBool()*/;
// }

// bool isDeletedUser(qlonglong userId, StorageManager *store)
// {
//     return store->getUser(userId)->type_->get_id() == td::td_api::userTypeDeleted::ID;
// }

// }  // namespace

Utils::Utils(QObject *parent)
    : QObject(parent)
    , m_locale(&Locale::instance())
    , m_storageManager(&StorageManager::instance())
{
}

td::td_api::object_ptr<td::td_api::ChatList> Utils::toChatList(const std::unique_ptr<ChatList> &list)
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

QString Utils::getServiceMessageContent(const td::td_api::message &message, StorageManager *store, bool openUser)
{
    // const auto chat = store->getChat(message.chat_id_);

    // auto &sender = message.sender_id_;
    // auto &content = message.content_;
    // auto isOutgoing = message.is_outgoing_;

    // auto isChannel = isChannelChat(chat);

    // auto author = getMessageAuthor(message, store, openUser);

    // switch (content->get_id())
    // {
    //     case td::td_api::messageExpiredPhoto::ID: {
    //         return QObject::tr("AttachPhotoExpired");
    //     }
    //     case td::td_api::messageExpiredVideo::ID: {
    //         return QObject::tr("AttachVideoExpired");
    //     }
    //     case td::td_api::messageBasicGroupChatCreate::ID: {
    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionYouCreateGroup");
    //         }

    //         return QObject::tr("ActionCreateGroup").replace("un1", author);
    //     }
    //     case td::td_api::messageSupergroupChatCreate::ID: {
    //         if (isChannel)
    //         {
    //             return QObject::tr("ActionCreateChannel");
    //         }

    //         return QObject::tr("ActionCreateMega");
    //     }
    //     case td::td_api::messageChatChangeTitle::ID: {
    //         const auto title = QString::fromStdString(static_cast<const td::td_api::messageChatChangeTitle *>(content.get())->title_);

    //         if (isChannel)
    //         {
    //             return QObject::tr("ActionChannelChangedTitle").replace("un2", title);
    //         }

    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionYouChangedTitle").replace("un2", title);
    //         }

    //         return QObject::tr("ActionChangedTitle").replace("un1", author).replace("un2", title);
    //     }
    //     case td::td_api::messageChatChangePhoto::ID: {
    //         if (isChannel)
    //         {
    //             return QObject::tr("ActionChannelChangedPhoto");
    //         }

    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionYouChangedPhoto");
    //         }

    //         return QObject::tr("ActionChangedPhoto").replace("un1", author);
    //     }
    //     case td::td_api::messageChatDeletePhoto::ID: {
    //         if (isChannel)
    //         {
    //             return QObject::tr("ActionChannelRemovedPhoto");
    //         }

    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionYouRemovedPhoto");
    //         }

    //         return QObject::tr("ActionRemovedPhoto").replace("un1", author);
    //     }
    //     case td::td_api::messageChatAddMembers::ID: {
    //         const auto &memberUserIds = static_cast<const td::td_api::messageChatAddMembers *>(content.get())->member_user_ids_;

    //         if (memberUserIds.size() == 1)
    //         {
    //             auto memberUserId = memberUserIds[0];
    //             if (static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_ == memberUserId)
    //             {
    //                 if (isSupergroup(chat) && isChannel)
    //                 {
    //                     return QObject::tr("ChannelJoined");
    //                 }

    //                 if (isSupergroup(chat) && !isChannel)
    //                 {
    //                     if (isMeUser(memberUserId, store))
    //                     {
    //                         return QObject::tr("ChannelMegaJoined");
    //                     }

    //                     return QObject::tr("ActionAddUserSelfMega").replace("un1", author);
    //                 }

    //                 if (isOutgoing)
    //                 {
    //                     return QObject::tr("ActionAddUserSelfYou");
    //                 }

    //                 return QObject::tr("ActionAddUserSelf").replace("un1", author);
    //             }

    //             if (isOutgoing)
    //             {
    //                 return QObject::tr("ActionYouAddUser").replace("un2", getUserName(memberUserId, store, openUser));
    //             }

    //             if (isMeUser(memberUserId, store))
    //             {
    //                 if (isSupergroup(chat))
    //                 {
    //                     if (!isChannel)
    //                     {
    //                         return QObject::tr("MegaAddedBy").replace("un1", author);
    //                     }

    //                     return QObject::tr("ChannelAddedBy").replace("un1", author);
    //                 }

    //                 return QObject::tr("ActionAddUserYou").replace("un1", author);
    //             }

    //             return QObject::tr("ActionAddUser").replace("un1", author).replace("un2", getUserName(memberUserId, store, openUser));
    //         }

    //         QStringList result;
    //         for (const auto &userId : memberUserIds)
    //         {
    //             result << getUserName(userId, store, openUser);
    //         }

    //         auto users = result.join(", ");

    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionYouAddUser").arg(users);
    //         }

    //         return QObject::tr("ActionAddUser").replace("un1", author).replace("un2", users);
    //     }
    //     case td::td_api::messageChatJoinByLink::ID: {
    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionInviteYou");
    //         }

    //         return QObject::tr("ActionInviteUser").replace("un1", author);
    //     }
    //     case td::td_api::messageChatDeleteMember::ID: {
    //         auto userId = static_cast<const td::td_api::messageChatDeleteMember *>(content.get())->user_id_;

    //         if (userId == static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_)
    //         {
    //             if (isOutgoing)
    //             {
    //                 return QObject::tr("ActionYouLeftUser");
    //             }

    //             return QObject::tr("ActionLeftUser").replace("un1", author);
    //         }

    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionYouKickUser").replace("un2", getUserName(userId, store, openUser));
    //         }
    //         else if (isMeUser(userId, store))
    //         {
    //             return QObject::tr("ActionKickUserYou").replace("un1", author);
    //         }

    //         return QObject::tr("ActionKickUser").replace("un1", author).replace("un2", getUserName(userId, store, openUser));
    //     }
    //     case td::td_api::messageChatUpgradeTo::ID: {
    //         return QObject::tr("ActionMigrateFromGroup");
    //     }
    //     case td::td_api::messageChatUpgradeFrom::ID: {
    //         return QObject::tr("ActionMigrateFromGroup");
    //     }
    //     case td::td_api::messagePinMessage::ID: {
    //         return QObject::tr("ActionPinned").replace("un1", author);
    //     }
    //     case td::td_api::messageScreenshotTaken::ID: {
    //         if (isOutgoing)
    //         {
    //             return QObject::tr("ActionTakeScreenshootYou");
    //         }

    //         return QObject::tr("ActionTakeScreenshoot").replace("un1", author);
    //     }
    //     case td::td_api::messageCustomServiceAction::ID: {
    //         return QString::fromStdString(static_cast<const td::td_api::messageCustomServiceAction *>(content.get())->text_);
    //     }
    //     case td::td_api::messageContactRegistered::ID: {
    //         return QObject::tr("NotificationContactJoined")
    //             .arg(getUserName(static_cast<const td::td_api::messageSenderUser *>(sender.get())->user_id_, store, openUser));
    //     }
    //     case td::td_api::messageUnsupported::ID: {
    //         return QObject::tr("UnsupportedMedia");
    //     }
    // }

    return QObject::tr("UnsupportedMedia");
}

QString Utils::getUserShortName(qlonglong userId, StorageManager *store) noexcept
{
    // const auto user = store->getUser(userId);

    // const auto firstName = user->first_name_;
    // const auto lastName = user->last_name_;

    // switch (user->type_->get_id())
    // {
    //     case td::td_api::userTypeBot::ID:
    //     case td::td_api::userTypeRegular::ID: {
    //         if (!firstName.empty())
    //         {
    //             return QString::fromStdString(firstName);
    //         }

    //         if (!lastName.empty())
    //         {
    //             return QString::fromStdString(lastName);
    //         }
    //     }
    //     case td::td_api::userTypeDeleted::ID:
    //     case td::td_api::userTypeUnknown::ID: {
    //         return QObject::tr("HiddenName");
    //     }

    //     default:
    return QString();
    // }
}

QString Utils::getTitle(const td::td_api::message &message, StorageManager *store) noexcept
{
    // const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());

    // if (sender->get_id() == td::td_api::messageSenderUser::ID)
    // {
    //     auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
    //     return getUserFullName(userId, store);
    // }
    // else if (sender->get_id() == td::td_api::messageSenderChat::ID)
    // {
    //     auto chatId = static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_;
    //     return QString::fromStdString(store->getChat(chatId)->title_);
    // }

    return {};
}

QString Utils::getMessageDate(const td::td_api::message &message) noexcept
{
    const auto date = QDateTime::fromMSecsSinceEpoch(static_cast<int64_t>(message.date_) * 1000);
    const auto days = date.daysTo(QDateTime::currentDateTime());

    if (days == 0)
    {
        return date.toString(QObject::tr("formatterDay12H"));
    }
    else if (days < 7)
    {
        return date.toString(QObject::tr("formatterWeek"));
    }

    return date.toString(QObject::tr("formatterYear"));
}

bool Utils::isChatUnread(qlonglong chatId, StorageManager *store) noexcept
{
    // const auto chat = store->getChat(chatId);

    // return chat->is_marked_as_unread_ || chat->unread_count_ > 0;

    return {};
}

QString Utils::getMessageSenderName(const td::td_api::message &message, StorageManager *store) noexcept
{
    // if (isServiceMessage(message))
    //     return QString();

    // const auto sender = static_cast<const td::td_api::MessageSender *>(message.sender_id_.get());

    // const auto chat = store->getChat(message.chat_id_);

    // switch (chat->type_->get_id())
    // {
    //     case td::td_api::chatTypePrivate::ID:
    //     case td::td_api::chatTypeSecret::ID:
    //         return QString();
    //     case td::td_api::chatTypeBasicGroup::ID:
    //     case td::td_api::chatTypeSupergroup::ID: {
    //         if (isChannelChat(chat))
    //         {
    //             return QString();
    //         }

    //         switch (sender->get_id())
    //         {
    //             case td::td_api::messageSenderUser::ID: {
    //                 auto userId = static_cast<const td::td_api::messageSenderUser *>(sender)->user_id_;
    //                 if (isMeUser(userId, store))
    //                     return QObject::tr("FromYou");

    //                 return getUserShortName(userId, store);
    //             }
    //             case td::td_api::messageSenderChat::ID: {
    //                 return getChatTitle(static_cast<const td::td_api::messageSenderChat *>(sender)->chat_id_, store);
    //             }
    //         }
    //     }

    //     default:
    return QString();
    // }
}

void Utils::copyToClipboard(const QVariantMap &content) noexcept
{
    const auto contentType = content.value("@type").toString();
    if (contentType != QLatin1String("messageText"))
        return;

    auto clipboard = QApplication::clipboard();
    const auto text = content.value("text").toMap().value("text").toString();

    clipboard->setText(text);
}

QImage Utils::getThumb(const QVariantMap &thumbnail) noexcept
{
    QByteArray thumb = "data:image/jpeg;base64, " + thumbnail.value("data").toByteArray();
    QBuffer buf(&thumb);
    buf.open(QIODevice::ReadOnly);

    QImageReader reader(&buf);
    return reader.read();
}

bool Utils::isMessageUnread(const td::td_api::message &message, StorageManager *store) noexcept
{
    // const auto chat = store->getChat(message.chat_id_);

    // if (message.is_outgoing_)
    // {
    //     if (isMeChat(chat, store))
    //     {
    //         return false;
    //     }
    //     return message.id_ > chat->last_read_outbox_message_id_;
    // }

    return {};

    // return message.id_ > chat->last_read_inbox_message_id_;
}

QString Utils::getViews(int views) noexcept
{
    if (views < 1000)
    {
        return QString::number(views);
    }
    else if (views < 1000000)
    {
        auto value = views / 100;
        auto fractionDigits = (value % 10 == 0) ? 0 : 1;

        return QString("%1K").arg(static_cast<double>(value) / 10.0, 0, 'f', fractionDigits);
    }
    else
    {
        auto value = views / 100000;
        auto fractionDigits = (value % 10 == 0) ? 0 : 1;

        return QString("%1M").arg(static_cast<double>(value) / 10.0, 0, 'f', fractionDigits);
    }
}

QString Utils::getFileSize(const QVariantMap &file) noexcept
{
    const auto size = file.value("size").toInt();
    if (size <= 0)
        return QString();

    const double KB = 1024.0;
    const double MB = KB * 1024.0;
    const double GB = MB * 1024.0;

    if (size < KB)
        return QString("%1 B").arg(size);
    else if (size < MB)
        return QString("%1 KB").arg(static_cast<double>(size) / KB, 0, 'f', 2);
    else if (size < GB)
        return QString("%1 MB").arg(static_cast<double>(size) / MB, 0, 'f', 2);
    else
        return QString("%1 GB").arg(static_cast<double>(size) / GB, 0, 'f', 2);
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

bool Utils::isMeChat(const std::shared_ptr<Chat> chat) const noexcept
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
