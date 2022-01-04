#include "TdApi.hpp"

#include "Common.hpp"
#include "Serialize.hpp"

#include <fnv-cpp/fnv.h>
#include <td/telegram/td_json_client.h>

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QStringBuilder>
#include <QTimer>

namespace {

 QString authentication_code_title(const QVariantMap &codeInfo)
{
    auto type = codeInfo.value("type").toMap();

    auto codeInfoType = type.value("@type").toByteArray();
    switch (fnv::hashRuntime(codeInfoType.constData()))
    {
        case fnv::hash("authenticationCodeTypeTelegramMessage"): {
            return QObject::tr("SentAppCodeTitle");
        }
        case fnv::hash("authenticationCodeTypeCall"):
        case fnv::hash("authenticationCodeTypeSms"): {
            return QObject::tr("SentSmsCodeTitle");
        }
    }

    return "Title";
}

 QString authentication_code_subtitle(const QVariantMap &codeInfo)
{
    auto phoneNumber = codeInfo.value("phone_number").toString();
    auto type = codeInfo.value("type").toMap();

    auto codeInfoType = type.value("@type").toByteArray();
    switch (fnv::hashRuntime(codeInfoType.constData()))
    {
        case fnv::hash("authenticationCodeTypeCall"): {
            return QObject::tr("SentCallCode").arg(phoneNumber);
        }
        case fnv::hash("authenticationCodeTypeFlashCall"): {
            return QObject::tr("SentCallOnly").arg(phoneNumber);
        }
        case fnv::hash("authenticationCodeTypeSms"): {
            return QObject::tr("SentSmsCode").arg(phoneNumber);
        }
        case fnv::hash("authenticationCodeTypeTelegramMessage"): {
            return QObject::tr("SentAppCode");
        }
    }

    return {};
}

 QString authentication_code_next_type_string(const QVariantMap &codeInfo)
{
    auto nextType = codeInfo.value("next_type").toMap();

    auto codeInfoType = nextType.value("@type").toByteArray();
    switch (fnv::hashRuntime(codeInfoType.constData()))
    {
        case fnv::hash("authenticationCodeTypeCall"): {
            return QObject::tr("CallText");
        }
        case fnv::hash("authenticationCodeTypeSms"): {
            return QObject::tr("SmsText");
        }
    }

    return {};
}

 bool authentication_code_is_next_type_sms(const QVariantMap &codeInfo)
{
    auto codeInfoType = codeInfo.value("next_type").toMap().value("@type").toByteArray();

    return codeInfoType == "authenticationCodeTypeSms";
}

 int authentication_code_length(const QVariantMap &codeInfo)
{
    auto type = codeInfo.value("type").toMap();

    auto codeInfoType = type.value("@type").toByteArray();
    switch (fnv::hashRuntime(codeInfoType.constData()))
    {
        case fnv::hash("authenticationCodeTypeCall"): {
            return type.value("length").toInt();
        }
        case fnv::hash("authenticationCodeTypeFlashCall"): {
            return {};
        }
        case fnv::hash("authenticationCodeTypeSms"): {
            return type.value("length").toInt();
        }
        case fnv::hash("authenticationCodeTypeTelegramMessage"): {
            return type.value("length").toInt();
        }
    }

    return {};
}

}  // namespace

TdApi::TdApi()
    : basicGroupStore(&emplace<BasicGroupStore>())
    , chatStore(&emplace<ChatStore>())
    , fileStore(&emplace<FileStore>())
    , optionStore(&emplace<OptionStore>())
    , supergroupStore(&emplace<SupergroupStore>())
    , userStore(&emplace<UserStore>())
{
    // disable TDLib logging
    td_execute(R"({"@type":"setLogVerbosityLevel","new_verbosity_level":0})");
}

TdApi &TdApi::getInstance()
{
    static TdApi staticObject;
    return staticObject;
}

void TdApi::initialize()
{
    for (auto &store : stores)
    {
        store->initialize(this);
    }

    initEvents();
}

void TdApi::sendRequest(const QVariantMap &js)
{
    nlohmann::json json(js);
    td_send(clientId, json.dump().c_str());
}

void TdApi::log(const QVariantMap &js) noexcept
{
    nlohmann::json json(js);
    qDebug() << json.dump(2).c_str();
}

bool TdApi::isAuthorized() const noexcept
{
    return m_isAuthorized;
}

void TdApi::checkCode(const QString &code) noexcept
{
    QVariantMap result;
    result.insert("@type", "checkAuthenticationCode");
    result.insert("code", code);

    sendRequest(result);
}

void TdApi::checkPassword(const QString &password) noexcept
{
    QVariantMap result;
    result.insert("@type", "checkAuthenticationPassword");
    result.insert("password", password);

    sendRequest(result);
}

void TdApi::logOut() noexcept
{
    td_send(clientId, R"({"@type":"logOut"})");
}

void TdApi::registerUser(const QString &firstName, const QString &lastName) noexcept
{
    QVariantMap result;
    result.insert("@type", "registerUser");
    result.insert("first_name", firstName);
    result.insert("last_name", lastName);

    sendRequest(result);
}

void TdApi::setPhoneNumber(const QString &phoneNumber) noexcept
{
    QVariantMap result;
    result.insert("@type", "setAuthenticationPhoneNumber");
    result.insert("phone_number", phoneNumber);

    sendRequest(result);
}

void TdApi::resendCode() noexcept
{
    td_send(clientId, R"({"@type":"resendAuthenticationCode"})");
}

void TdApi::deleteAccount(const QString &reason) noexcept
{
    QVariantMap result;
    result.insert("@type", "deleteAccount");
    result.insert("reason", reason);

    sendRequest(result);
}

void TdApi::close() noexcept
{
    td_send(clientId, R"({"@type":"close"})");

    // 1 sec delay
    QEventLoop loop;
    QTimer::singleShot(1000, &loop, SLOT(quit()));
    loop.exec();
}

void TdApi::closeChat(qint64 chatId)
{
    QVariantMap result;
    result.insert("@type", "closeChat");
    result.insert("chat_id", chatId);

    sendRequest(result);
}

void TdApi::closeSecretChat(qint32 secretChatId)
{
    QVariantMap result;
    result.insert("@type", "closeSecretChat");
    result.insert("secret_chat_id", secretChatId);

    sendRequest(result);
}

void TdApi::createBasicGroupChat(qint32 basicGroupId, bool force)
{
    QVariantMap result;
    result.insert("@type", "createBasicGroupChat");
    result.insert("basic_group_id", basicGroupId);
    result.insert("force", force);

    sendRequest(result);
}

void TdApi::createNewBasicGroupChat(const QVariantList &userIds, const QString &title)
{
    QVariantMap result;
    result.insert("@type", "createNewBasicGroupChat");
    result.insert("user_ids", userIds);
    result.insert("title", title);

    sendRequest(result);
}

void TdApi::createNewSecretChat(qint32 userId)
{
    QVariantMap result;
    result.insert("@type", "createNewSecretChat");
    result.insert("user_id", userId);

    sendRequest(result);
}

void TdApi::createNewSupergroupChat(const QString &title, bool isChannel, const QString &description, const QVariantMap &location)
{
    QVariantMap result;
    result.insert("@type", "createNewSupergroupChat");
    result.insert("title", title);
    result.insert("is_channel", isChannel);
    result.insert("description", description);
    result.insert("location", location);

    sendRequest(result);
}

void TdApi::createPrivateChat(qint32 userId, bool force)
{
    QVariantMap result;
    result.insert("@type", "createPrivateChat");
    result.insert("user_id", userId);
    result.insert("force", force);

    sendRequest(result);
}

void TdApi::createSecretChat(qint32 secretChatId)
{
    QVariantMap result;
    result.insert("@type", "createSecretChat");
    result.insert("secret_chat_id", secretChatId);

    sendRequest(result);
}

void TdApi::createSupergroupChat(qint32 supergroupId, bool force)
{
    QVariantMap result;
    result.insert("@type", "createSupergroupChat");
    result.insert("supergroup_id", supergroupId);
    result.insert("force", force);

    sendRequest(result);
}

void TdApi::deleteChatHistory(qint64 chatId, bool removeFromChatList, bool revoke)
{
    QVariantMap result;
    result.insert("@type", "deleteChatHistory");
    result.insert("chat_id", chatId);
    result.insert("remove_from_chat_list", removeFromChatList);
    result.insert("revoke", revoke);

    sendRequest(result);
}

void TdApi::deleteMessages(qint64 chatId, const QVariantList &messageIds, bool revoke)
{
    QVariantMap result;
    result.insert("@type", "deleteMessages");
    result.insert("chat_id", chatId);
    result.insert("message_ids", messageIds);
    result.insert("revoke", revoke);

    sendRequest(result);
}

void TdApi::editMessageText(qint64 chatId, qint64 messageId, const QVariantMap &replyMarkup, const QVariantMap &inputMessageContent)
{
    QVariantMap result;
    result.insert("@type", "editMessageText");
    result.insert("chat_id", chatId);
    result.insert("message_id", messageId);
    result.insert("reply_markup", replyMarkup);
    result.insert("input_message_content", inputMessageContent);

    sendRequest(result);
}

void TdApi::getChat(qint64 chatId)
{
    QVariantMap result;
    result.insert("@type", "getChat");
    result.insert("chat_id", chatId);

    sendRequest(result);
}

void TdApi::getChatFilter(qint32 chatFilterId)
{
    QVariantMap result;
    result.insert("@type", "getChatFilter");
    result.insert("chat_filter_id", chatFilterId);

    sendRequest(result);
}

void TdApi::getChatHistory(qint64 chatId, qint64 fromMessageId, qint32 offset, qint32 limit, bool onlyLocal)
{
    QVariantMap result;
    result.insert("@type", "getChatHistory");
    result.insert("chat_id", chatId);
    result.insert("from_message_id", fromMessageId);
    result.insert("offset", offset);
    result.insert("limit", limit);
    result.insert("only_local", onlyLocal);

    sendRequest(result);
}

void TdApi::getMessage(qint64 chatId, qint64 messageId)
{
    QVariantMap result;
    result.insert("@type", "getMessage");
    result.insert("chat_id", chatId);
    result.insert("message_id", messageId);

    sendRequest(result);
}

void TdApi::getMessages(qint64 chatId, const QVariantList &messageIds)
{
    QVariantMap result;
    result.insert("@type", "getMessages");
    result.insert("chat_id", chatId);
    result.insert("message_ids", messageIds);

    sendRequest(result);
}

void TdApi::joinChat(qint64 chatId)
{
    QVariantMap result;
    result.insert("@type", "joinChat");
    result.insert("chat_id", chatId);

    sendRequest(result);
}

void TdApi::leaveChat(qint64 chatId)
{
    QVariantMap result;
    result.insert("@type", "leaveChat");
    result.insert("chat_id", chatId);

    sendRequest(result);
}

void TdApi::openChat(qint64 chatId)
{
    QVariantMap result;
    result.insert("@type", "openChat");
    result.insert("chat_id", chatId);

    sendRequest(result);
}

void TdApi::sendChatAction(qint64 chatId, qint64 messageThreadId, const QVariantMap &action)
{
    QVariantMap result;
    result.insert("@type", "sendChatAction");
    result.insert("chat_id", chatId);
    result.insert("message_thread_id", messageThreadId);
    result.insert("action", action);

    sendRequest(result);
}

void TdApi::sendMessage(qint64 chatId, qint64 messageThreadId, qint64 replyToMessageId, const QVariantMap &options,
                        const QVariantMap &replyMarkup, const QVariantMap &inputMessageContent)
{
    QVariantMap result;
    result.insert("@type", "sendMessage");
    result.insert("chat_id", chatId);
    result.insert("message_thread_id", messageThreadId);
    result.insert("reply_to_message_id", replyToMessageId);
    result.insert("options", options);
    result.insert("reply_markup", replyMarkup);
    result.insert("input_message_content", inputMessageContent);

    sendRequest(result);
}

void TdApi::setChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings)
{
    QVariantMap result;
    result.insert("@type", "setChatNotificationSettings");
    result.insert("chat_id", chatId);
    result.insert("notification_settings", notificationSettings);

    sendRequest(result);
}

void TdApi::setOption(const QString &name, const QVariant &value)
{
    QVariantMap optionValue;

    switch (value.type())
    {
        case QVariant::Bool:
            optionValue.insert("@type", "optionValueBoolean");
            optionValue.insert("value", value.toBool());
            break;
        case QVariant::Int:
        case QVariant::LongLong:
            optionValue.insert("@type", "optionValueInteger");
            optionValue.insert("value", value.toInt());
            break;
        case QVariant::String:
            optionValue.insert("@type", "optionValueString");
            optionValue.insert("value", value.toString());
            break;
        default:
            optionValue.insert("@type", "optionValueEmpty");
            optionValue.insert("value", QVariant());
    }

    QVariantMap result;
    result.insert("@type", "setOption");
    result.insert("name", name);
    result.insert("value", optionValue);

    sendRequest(result);
}

void TdApi::toggleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread)
{
    QVariantMap result;
    result.insert("@type", "toggleChatIsMarkedAsUnread");
    result.insert("chat_id", chatId);
    result.insert("is_marked_as_unread", isMarkedAsUnread);

    sendRequest(result);
}

void TdApi::viewMessages(qint64 chatId, qint64 messageThreadId, const QVariantList &messageIds, bool forceRead)
{
    QVariantMap result;
    result.insert("@type", "viewMessages");
    result.insert("chat_id", chatId);
    result.insert("message_thread_id", messageThreadId);
    result.insert("message_ids", messageIds);
    result.insert("force_read", forceRead);

    sendRequest(result);
}

void TdApi::downloadFile(qint32 fileId, qint32 priority, qint32 offset, qint32 limit, bool synchronous)
{
    QVariantMap result;
    result.insert("@type", "downloadFile");
    result.insert("file_id", fileId);
    result.insert("priority", priority);
    result.insert("offset", offset);
    result.insert("limit", limit);
    result.insert("synchronous", synchronous);

    sendRequest(result);
}

void TdApi::getMe()
{
    td_send(clientId, R"({"@type":"getMe"})");
}

void TdApi::setLogVerbosityLevel(qint32 newVerbosityLevel)
{
    QVariantMap result;
    result.insert("@type", "setLogVerbosityLevel");
    result.insert("new_verbosity_level", newVerbosityLevel);

    sendRequest(result);
}

void TdApi::listen()
{
    // A worker thread
    m_worker = std::jthread([this](const std::stop_token &token) {
        while (!token.stop_requested())
        {
            const char *result = td_receive(WaitTimeout);
            if (result != nullptr)
            {
                auto document = nlohmann::json::parse(result);
                auto type = document["@type"].get<std::string>();

                if (auto it = m_events.find(type); it != m_events.end())
                {
                    it->second(document.get<QVariantMap>());
                }
            }
        }
    });
}

void TdApi::initEvents()
{
    clientId = td_create_client_id();

    // start the client by sending request to it
    td_send(clientId, R"({"@type":"getOption", "name":"version"})");

    m_events.emplace("updateAuthorizationState", [this](const QVariantMap &data) { handleAuthorizationState(data); });

    m_events.emplace("updateConnectionState", [this](const QVariantMap &data) { emit updateConnectionState(data.value("state").toMap()); });

    m_events.emplace("updateNewMessage", [this](const QVariantMap &data) { emit updateNewMessage(data.value("message").toMap()); });
    m_events.emplace("updateMessageSendAcknowledged", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();

        emit updateMessageSendAcknowledged(chatId, messageId);
    });
    m_events.emplace("updateMessageSendSucceeded", [this](const QVariantMap &data) {
        const auto message = data.value("message").toMap();
        const auto oldMessageId = data.value("old_message_id").toLongLong();

        emit updateMessageSendSucceeded(message, oldMessageId);
    });
    m_events.emplace("updateMessageSendFailed", [this](const QVariantMap &data) {
        const auto message = data.value("chat_id").toMap();
        const auto oldMessageId = data.value("old_message_id").toLongLong();
        const auto errorCode = data.value("error_code").toInt();
        const auto errorMessage = data.value("error_message").toString();

        emit updateMessageSendFailed(message, oldMessageId, errorCode, errorMessage);
    });
    m_events.emplace("updateMessageContent", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();
        const auto newContent = data.value("new_content").toMap();

        emit updateMessageContent(chatId, messageId, newContent);
    });
    m_events.emplace("updateMessageEdited", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();
        const auto editDate = data.value("edit_date").toInt();
        const auto replyMarkup = data.value("reply_markup").toMap();

        emit updateMessageEdited(chatId, messageId, editDate, replyMarkup);
    });
    m_events.emplace("updateMessageIsPinned", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();
        const auto isPinned = data.value("is_pinned").toBool();

        emit updateMessageIsPinned(chatId, messageId, isPinned);
    });
    m_events.emplace("updateMessageInteractionInfo", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();
        const auto interactionInfo = data.value("interaction_info").toMap();

        emit updateMessageInteractionInfo(chatId, messageId, interactionInfo);
    });
    m_events.emplace("updateMessageContentOpened", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();

        emit updateMessageContentOpened(chatId, messageId);
    });
    m_events.emplace("updateMessageMentionRead", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();
        const auto unreadMentionCount = data.value("unread_mention_count").toInt();

        emit updateMessageMentionRead(chatId, messageId, unreadMentionCount);
    });
    m_events.emplace("updateMessageLiveLocationViewed", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageId = data.value("message_id").toLongLong();

        emit updateMessageLiveLocationViewed(chatId, messageId);
    });

    m_events.emplace("updateDeleteMessages", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageIds = data.value("message_ids").toList();
        const auto isPermanent = data.value("is_permanent").toBool();
        const auto fromCache = data.value("from_cache").toBool();

        emit updateDeleteMessages(chatId, messageIds, isPermanent, fromCache);
    });
    m_events.emplace("updateUnreadMessageCount", [this](const QVariantMap &data) {
        const auto chatList = data.value("chat_list").toMap();
        const auto unreadCount = data.value("unread_count").toInt();
        const auto unreadUnmutedCount = data.value("unread_unmuted_count").toInt();

        emit updateUnreadMessageCount(chatList, unreadCount, unreadUnmutedCount);
    });
    m_events.emplace("updateUnreadChatCount", [this](const QVariantMap &data) {
        const auto chatList = data.value("chat_list").toMap();
        const auto totalCount = data.value("total_count").toInt();
        const auto unreadCount = data.value("unread_count").toInt();
        const auto unreadUnmutedCount = data.value("unread_unmuted_count").toInt();
        const auto markedAsUnreadCount = data.value("marked_as_unread_count").toInt();
        const auto markedAsUnreadUnmutedCount = data.value("marked_as_unread_unmuted_count").toInt();

        emit updateUnreadChatCount(chatList, totalCount, unreadCount, unreadUnmutedCount, markedAsUnreadCount, markedAsUnreadUnmutedCount);
    });
    m_events.emplace("updateActiveNotifications",
                     [this](const QVariantMap &data) { emit updateActiveNotifications(data.value("groups").toList()); });
    m_events.emplace("updateNotificationGroup", [this](const QVariantMap &data) {
        const auto notificationGroupId = data.value("notification_group_id").toInt();
        const auto type = data.value("type").toMap();
        const auto chatId = data.value("chat_id").toLongLong();
        const auto notificationSettingsChatId = data.value("notification_settings_chat_id").toLongLong();
        const auto isSilent = data.value("is_silent").toBool();
        const auto totalCount = data.value("total_count").toInt();
        const auto addedNotifications = data.value("added_notifications").toList();
        const auto removedNotificationIds = data.value("removed_notification_ids").toList();

        emit updateNotificationGroup(notificationGroupId, type, chatId, notificationSettingsChatId, isSilent, totalCount,
                                     addedNotifications, removedNotificationIds);
    });

    m_events.emplace("updateNewChat", [this](const QVariantMap &data) {
        const auto chat = data.value("chat").toMap();

        emit updateNewChat(chat);
    });

    m_events.emplace("updateChatTitle", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto title = data.value("title").toString();

        emit updateChatTitle(chatId, title);
    });

    m_events.emplace("updateChatPhoto", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto photo = data.value("photo").toMap();

        emit updateChatPhoto(chatId, photo);
    });

    m_events.emplace("updateChatPermissions", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto permissions = data.value("permissions").toMap();

        emit updateChatPermissions(chatId, permissions);
    });

    m_events.emplace("updateChatLastMessage", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto lastMessage = data.value("last_message").toMap();
        const auto positions = data.value("positions").toList();

        emit updateChatLastMessage(chatId, lastMessage, positions);
    });

    m_events.emplace("updateChatPosition", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto position = data.value("position").toMap();

        emit updateChatPosition(chatId, position);
    });

    m_events.emplace("updateChatIsMarkedAsUnread", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto isMarkedAsUnread = data.value("is_marked_as_unread").toBool();

        emit updateChatIsMarkedAsUnread(chatId, isMarkedAsUnread);
    });

    m_events.emplace("updateChatIsBlocked", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto isBlocked = data.value("is_blocked").toBool();

        emit updateChatIsBlocked(chatId, isBlocked);
    });

    m_events.emplace("updateChatHasScheduledMessages", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto hasScheduledMessages = data.value("has_scheduled_messages").toBool();

        emit updateChatHasScheduledMessages(chatId, hasScheduledMessages);
    });

    m_events.emplace("updateChatDefaultDisableNotification", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto defaultDisableNotification = data.value("default_disable_notification").toBool();

        emit updateChatDefaultDisableNotification(chatId, defaultDisableNotification);
    });

    m_events.emplace("updateChatReadInbox", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto lastReadInboxMessageId = data.value("last_read_inbox_message_id").toLongLong();
        const auto unreadCount = data.value("unread_count").toInt();

        emit updateChatReadInbox(chatId, lastReadInboxMessageId, unreadCount);
    });

    m_events.emplace("updateChatReadOutbox", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto lastReadOutboxMessageId = data.value("last_read_outbox_message_id").toLongLong();

        emit updateChatReadOutbox(chatId, lastReadOutboxMessageId);
    });

    m_events.emplace("updateChatUnreadMentionCount", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto unreadMentionCount = data.value("unread_mention_count").toInt();

        emit updateChatUnreadMentionCount(chatId, unreadMentionCount);
    });

    m_events.emplace("updateChatNotificationSettings", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto notificationSettings = data.value("notification_settings").toMap();

        emit updateChatNotificationSettings(chatId, notificationSettings);
    });

    m_events.emplace("updateScopeNotificationSettings", [this](const QVariantMap &data) {
        const auto scope = data.value("scope").toMap();
        const auto notificationSettings = data.value("notification_settings").toMap();
        emit updateScopeNotificationSettings(scope, notificationSettings);
    });

    m_events.emplace("updateChatActionBar", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto actionBar = data.value("action_bar").toMap();

        emit updateChatActionBar(chatId, actionBar);
    });

    m_events.emplace("updateChatReplyMarkup", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto replyMarkupMessageId = data.value("reply_markup_message_id").toLongLong();

        emit updateChatReplyMarkup(chatId, replyMarkupMessageId);
    });

    m_events.emplace("updateChatDraftMessage", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto draftMessage = data.value("draft_message").toMap();
        const auto position = data.value("position").toList();

        emit updateChatDraftMessage(chatId, draftMessage, position);
    });

    m_events.emplace("updateChatFilters", [this](const QVariantMap &data) { emit updateChatFilters(data.value("chat_filters").toList()); });

    m_events.emplace("updateChatOnlineMemberCount", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto onlineMemberCount = data.value("online_member_count").toInt();
        emit updateChatOnlineMemberCount(chatId, onlineMemberCount);
    });

    m_events.emplace("updateOption",
                     [this](const QVariantMap &data) { emit updateOption(data.value("name").toString(), data.value("value").toMap()); });

    m_events.emplace("updateChatAction", [this](const QVariantMap &data) {
        const auto chatId = data.value("chat_id").toLongLong();
        const auto messageThreadId = data.value("message_thread_id").toLongLong();
        const auto userId = data.value("user_id").toLongLong();
        const auto action = data.value("action").toMap();
        emit updateChatAction(chatId, messageThreadId, userId, action);
    });

    m_events.emplace("updateUserStatus", [this](const QVariantMap &data) {
        const auto userId = data.value("user_id").toLongLong();
        const auto status = data.value("status").toMap();

        emit updateUserStatus(userId, status);
    });

    m_events.emplace("updateUser", [this](const QVariantMap &data) {
        const auto user = data.value("user").toMap();

        emit updateUser(user);
    });

    m_events.emplace("updateBasicGroup", [this](const QVariantMap &data) {
        const auto basicGroup = data.value("basic_group").toMap();

        emit updateBasicGroup(basicGroup);
    });

    m_events.emplace("updateSupergroup", [this](const QVariantMap &data) {
        const auto supergroup = data.value("supergroup").toMap();

        emit updateSupergroup(supergroup);
    });

    m_events.emplace("updateUserFullInfo", [this](const QVariantMap &data) {
        const auto userId = data.value("user_id").toLongLong();
        const auto userFullInfo = data.value("user_full_info").toMap();

        emit updateUserFullInfo(userId, userFullInfo);
    });

    m_events.emplace("updateBasicGroupFullInfo", [this](const QVariantMap &data) {
        const auto basicGroupId = data.value("basic_group_id").toLongLong();
        const auto basicGroupFullInfo = data.value("basic_group_full_info").toMap();

        emit updateUserFullInfo(basicGroupId, basicGroupFullInfo);
    });

    m_events.emplace("updateSupergroupFullInfo", [this](const QVariantMap &data) {
        const auto supergroupId = data.value("supergroup_id").toLongLong();
        const auto supergroupFullInfo = data.value("supergroup_full_info").toMap();

        emit updateUserFullInfo(supergroupId, supergroupFullInfo);
    });

    m_events.emplace("updateFile", [this](const QVariantMap &data) {
        const auto file = data.value("file").toMap();

        emit updateFile(file);
    });

    // Returns ...
    m_events.emplace("basicGroupFullInfo", [this](const QVariantMap &data) { emit basicGroupFullInfo(data); });
    m_events.emplace("chat", [this](const QVariantMap &data) { emit chat(data); });
    m_events.emplace("chatMembers", [this](const QVariantMap &data) { emit chatMembers(data); });
    m_events.emplace("chats", [this](const QVariantMap &data) { emit chats(data); });
    m_events.emplace("countries", [this](const QVariantMap &data) { emit countries(data); });
    m_events.emplace("error", [this](const QVariantMap &data) { emit error(data); });
    m_events.emplace("file", [this](const QVariantMap &data) { emit file(data); });
    m_events.emplace("message", [this](const QVariantMap &data) { emit message(data); });
    m_events.emplace("messages", [this](const QVariantMap &data) { emit messages(data); });
    m_events.emplace("ok", [this](const QVariantMap &data) { emit ok(data); });
    m_events.emplace("stickers", [this](const QVariantMap &data) { emit stickers(data); });
    m_events.emplace("stickerSet", [this](const QVariantMap &data) { emit stickerSet(data); });
    m_events.emplace("stickerSets", [this](const QVariantMap &data) { emit stickerSets(data); });
    m_events.emplace("supergroupFullInfo", [this](const QVariantMap &data) { emit supergroupFullInfo(data); });
    m_events.emplace("userFullInfo", [this](const QVariantMap &data) { emit userFullInfo(data); });
    m_events.emplace("userProfilePhotos", [this](const QVariantMap &data) { emit userProfilePhotos(data); });
    m_events.emplace("users", [this](const QVariantMap &data) { emit users(data); });
}

void TdApi::handleAuthorizationState(const QVariantMap &data)
{
    auto authorizationState = data.value("authorization_state").toMap();
    auto authorizationStateType = authorizationState.value("@type").toByteArray();

    switch (fnv::hashRuntime(authorizationStateType.constData()))
    {
        case fnv::hash("authorizationStateWaitTdlibParameters"): {
            QVariantMap parameters;
            parameters.insert("database_directory", QString(QDir::homePath() % DatabaseDirectory));
            parameters.insert("use_file_database", true);
            parameters.insert("use_chat_info_database", true);
            parameters.insert("use_message_database", true);
            parameters.insert("use_secret_chats", true);
            parameters.insert("api_id", ApiId);
            parameters.insert("api_hash", ApiHash);
            parameters.insert("system_language_code", SystemLanguageCode);
            parameters.insert("device_model", DeviceModel);
            parameters.insert("system_version", SystemVersion);
            parameters.insert("application_version", AppVersion);

            QVariantMap result;
            result.insert("@type", "setTdlibParameters");
            result.insert("parameters", parameters);

            sendRequest(result);
            break;
        }
        case fnv::hash("authorizationStateWaitEncryptionKey"): {
            td_send(clientId, R"({"@type":"checkDatabaseEncryptionKey","encryption_key":""})");
            break;
        }
        case fnv::hash("authorizationStateWaitCode"): {
            const auto codeInfo = authorizationState.value("code_info").toMap();

            QVariantMap result;
            result.insert("subtitle", authentication_code_subtitle(codeInfo));
            result.insert("title", authentication_code_title(codeInfo));
            result.insert("length", authentication_code_length(codeInfo));

            result.insert("isNextTypeSms", authentication_code_is_next_type_sms(codeInfo));
            result.insert("nextTypeString", authentication_code_next_type_string(codeInfo));

            result.insert("timeout", codeInfo.value("timeout").toInt());

            emit codeRequested(result);
            break;
        }
        case fnv::hash("authorizationStateWaitPassword"): {
            const auto password = authorizationState.value("password").toMap();

            QVariantMap result;
            result.insert("passwordHint", password.value("password_hint").toString());
            result.insert("hasRecoveryEmailAddress", password.value("has_recovery_email_address").toBool());
            result.insert("recoveryEmailAddressPattern", password.value("recovery_email_address_pattern").toString());

            emit passwordRequested(result);
            break;
        }
        case fnv::hash("authorizationStateWaitRegistration"): {
            const auto termsOfService = authorizationState.value("terms_of_service").toMap();

            QVariantMap result;
            result.insert("text", termsOfService.value("text").toMap().value("text").toString());
            result.insert("minUserAge", termsOfService.value("min_user_age").toInt());
            result.insert("showPopup", termsOfService.value("show_popup").toBool());

            emit registrationRequested(result);
            break;
        }
        case fnv::hash("authorizationStateReady"): {
            QVariantMap result;
            result.insert("@type", "loadChats");
            result.insert("chat_list", {});
            result.insert("limit", ChatSliceLimit);

            TdApi::getInstance().sendRequest(result);

            m_isAuthorized = true;

            emit isAuthorizedChanged();
            break;
        }
        case fnv::hash("authorizationStateClosed"): {
            m_worker.request_stop();
            break;
        }
    }

    emit updateAuthorizationState(authorizationState);
}
