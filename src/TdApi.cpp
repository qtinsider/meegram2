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

class JsonClient final
{
public:
    template <typename T = nlohmann::json>
    JsonClient(std::initializer_list<std::pair<std::string, T>> list)
    {
        for (auto &value : list)
        {
            json[value.first] = value.second;
        }
    }

    void sendObject(int clientId)
    {
        td_send(clientId, json.dump().c_str());
    }

private:
    nlohmann::json json;
};

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

TdApi::AuthorizationState TdApi::getAuthorizationState() const noexcept
{
    return m_state;
}

void TdApi::checkCode(const QString &code) noexcept
{
    JsonClient json{
        {"@type", "checkAuthenticationCode"},
        {"code", code},
    };

    json.sendObject(clientId);
}

void TdApi::checkPassword(const QString &password) noexcept
{
    JsonClient json{
        {"@type", "checkAuthenticationPassword"},
        {"password", password},
    };

    json.sendObject(clientId);
}

void TdApi::logOut() noexcept
{
    td_send(clientId, R"({"@type":"logOut"})");
}

void TdApi::registerUser(const QString &firstName, const QString &lastName) noexcept
{
    JsonClient json{
        {"@type", "registerUser"},
        {"first_name", firstName},
        {"last_name", lastName},
    };

    json.sendObject(clientId);
}

void TdApi::setPhoneNumber(const QString &phoneNumber) noexcept
{
    JsonClient json{
        {"@type", "setAuthenticationPhoneNumber"},
        {"phone_number", phoneNumber},
    };

    json.sendObject(clientId);
}

void TdApi::resendCode() noexcept
{
    td_send(clientId, R"({"@type":"resendAuthenticationCode"})");
}

void TdApi::deleteAccount(const QString &reason) noexcept
{
    JsonClient json{
        {"@type", "deleteAccount"},
        {"reason", reason},
    };

    json.sendObject(clientId);
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
    JsonClient json{
        {"@type", "closeChat"},
        {"chat_id", chatId},
    };

    json.sendObject(clientId);
}

void TdApi::closeSecretChat(qint32 secretChatId)
{
    JsonClient json{
        {"@type", "closeSecretChat"},
        {"secret_chat_id", secretChatId},
    };

    json.sendObject(clientId);
}

void TdApi::createBasicGroupChat(qint32 basicGroupId, bool force)
{
    JsonClient json{
        {"@type", "createBasicGroupChat"},
        {"basic_group_id", basicGroupId},
        {"force", force},
    };

    json.sendObject(clientId);
}

void TdApi::createNewBasicGroupChat(const QList<qint32> &userIds, const QString &title)
{
    JsonClient json{
        {"@type", "createNewBasicGroupChat"},
        {"user_ids", userIds},
        {"title", title},
    };

    json.sendObject(clientId);
}

void TdApi::createNewSecretChat(qint32 userId)
{
    JsonClient json{
        {"@type", "createNewSecretChat"},
        {"user_id", userId},
    };

    json.sendObject(clientId);
}

void TdApi::createNewSupergroupChat(const QString &title, bool isChannel, const QString &description, const QVariantMap &location)
{
    // clang-format off
    JsonClient json{
        {"@type", "createNewSupergroupChat"},
        {"title", title},
        {"is_channel", isChannel},
        {"description", description},
        {"location", location},
    };

    // clang-format on
    json.sendObject(clientId);
}

void TdApi::createPrivateChat(qint32 userId, bool force)
{
    JsonClient json{
        {"@type", "createPrivateChat"},
        {"user_id", userId},
        {"force", force},
    };

    json.sendObject(clientId);
}

void TdApi::createSecretChat(qint32 secretChatId)
{
    JsonClient json{
        {"@type", "createSecretChat"},
        {"secret_chat_id", secretChatId},
    };

    json.sendObject(clientId);
}

void TdApi::createSupergroupChat(qint32 supergroupId, bool force)
{
    JsonClient json{
        {"@type", "createSupergroupChat"},
        {"supergroup_id", supergroupId},
        {"force", force},
    };

    json.sendObject(clientId);
}

void TdApi::deleteChatHistory(qint64 chatId, bool removeFromChatList, bool revoke)
{
    JsonClient json{
        {"@type", "deleteChatHistory"},
        {"chat_id", chatId},
        {"remove_from_chat_list", removeFromChatList},
        {"revoke", revoke},
    };

    json.sendObject(clientId);
}

void TdApi::deleteMessages(qint64 chatId, const QList<qint64> &messageIds, bool revoke)
{
    JsonClient json{
        {"@type", "deleteMessages"},
        {"chat_id", chatId},
        {"message_ids", messageIds},
        {"revoke", revoke},
    };

    json.sendObject(clientId);
}

void TdApi::editMessageText(qint64 chatId, qint64 messageId, const QVariantMap &replyMarkup, const QVariantMap &inputMessageContent)
{
    JsonClient json{
        {"@type", "editMessageText"},
        {"chat_id", chatId},
        {"message_id", messageId},
        {"reply_markup", replyMarkup},
        {"input_message_content", inputMessageContent},
    };

    json.sendObject(clientId);
}

void TdApi::getChat(qint64 chatId)
{
    JsonClient json{
        {"@type", "getChat"},
        {"chat_id", chatId},
    };

    json.sendObject(clientId);
}

void TdApi::getChats(const QVariantMap &chatList, qint64 offsetOrder, qint64 offsetChatId, qint32 limit)
{
    // clang-format off
    JsonClient json{
        {"@type", "getChats"},
        {"chat_list", chatList},
        {"offset_order", offsetOrder},
        {"offset_chat_id", offsetChatId},
        {"limit", limit},
    };

    // clang-format on
    json.sendObject(clientId);
}

void TdApi::getChatHistory(qint64 chatId, qint64 fromMessageId, qint32 offset, qint32 limit, bool onlyLocal)
{
    // clang-format off
    JsonClient json{
        {"@type", "getChatHistory"},
        {"chat_id", chatId},
        {"from_message_id", fromMessageId},
        {"offset", offset},
        {"limit", limit},
        {"only_local", onlyLocal},
    };
    // clang-format on
    json.sendObject(clientId);
}

void TdApi::getMessage(qint64 chatId, qint64 messageId)
{
    JsonClient json{
        {"@type", "getMessage"},
        {"chat_id", chatId},
        {"message_id", messageId},
    };

    json.sendObject(clientId);
}

void TdApi::getMessages(qint64 chatId, const QList<qint64> &messageIds)
{
    JsonClient json{
        {"@type", "getMessages"},
        {"chat_id", chatId},
        {"message_ids", messageIds},
    };
    json.sendObject(clientId);
}

void TdApi::getSecretChat(qint32 secretChatId)
{
    JsonClient json{
        {"@type", "getSecretChat"},
        {"secret_chat_id", secretChatId},
    };

    json.sendObject(clientId);
}

void TdApi::getSupergroup(qint32 supergroupId)
{
    JsonClient json{
        {"@type", "getSupergroup"},
        {"supergroup_id", supergroupId},
    };

    json.sendObject(clientId);
}

void TdApi::getSupergroupFullInfo(qint32 supergroupId)
{
    JsonClient json{
        {"@type", "getSupergroupFullInfo"},
        {"supergroup_id", supergroupId},
    };

    json.sendObject(clientId);
}

void TdApi::joinChat(qint64 chatId)
{
    JsonClient json{
        {"@type", "joinChat"},
        {"chat_id", chatId},
    };

    json.sendObject(clientId);
}

void TdApi::leaveChat(qint64 chatId)
{
    JsonClient json{
        {"@type", "leaveChat"},
        {"chat_id", chatId},
    };

    json.sendObject(clientId);
}

void TdApi::openChat(qint64 chatId)
{
    JsonClient json{
        {"@type", "openChat"},
        {"chat_id", chatId},
    };

    json.sendObject(clientId);
}

void TdApi::sendChatAction(qint64 chatId, qint64 messageThreadId, const QVariantMap &action)
{
    JsonClient json{
        {"@type", "sendChatAction"},
        {"chat_id", chatId},
        {"message_thread_id", messageThreadId},
        {"action", action},
    };

    json.sendObject(clientId);
}

void TdApi::sendMessage(qint64 chatId, qint64 messageThreadId, qint64 replyToMessageId, const QVariantMap &options,
                        const QVariantMap &replyMarkup, const QVariantMap &inputMessageContent)
{
    JsonClient json{
        {"@type", "sendMessage"},
        {"chat_id", chatId},
        {"message_thread_id", messageThreadId},
        {"reply_to_message_id", replyToMessageId},
        {"options", options},
        {"reply_markup", replyMarkup},
        {"input_message_content", inputMessageContent},
    };

    json.sendObject(clientId);
}

void TdApi::setChatNotificationSettings(qint64 chatId, const QVariantMap &notificationSettings)
{
    JsonClient json{
        {"@type", "setChatNotificationSettings"},
        {"chat_id", chatId},
        {"notification_settings", notificationSettings},
    };

    json.sendObject(clientId);
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

    JsonClient json{
        {"@type", "setOption"},
        {"name", name},
        {"value", optionValue},
    };

    json.sendObject(clientId);
}

void TdApi::toggleChatIsMarkedAsUnread(qint64 chatId, bool isMarkedAsUnread)
{
    JsonClient json{
        {"@type", "toggleChatIsMarkedAsUnread"},
        {"chat_id", chatId},
        {"is_marked_as_unread", isMarkedAsUnread},
    };

    json.sendObject(clientId);
}

void TdApi::viewMessages(qint64 chatId, qint64 messageThreadId, const QList<qint64> &messageIds, bool forceRead)
{
    // clang-format off
    JsonClient json{
        {"@type", "viewMessages"},
        {"chat_id", chatId},
        {"message_thread_id", messageThreadId},
        {"message_ids", messageIds},
        {"force_read", forceRead},
    };
    // clang-format on

    json.sendObject(clientId);
}

void TdApi::downloadFile(qint32 fileId, qint32 priority, qint32 offset, qint32 limit, bool synchronous)
{
    // clang-format off
    JsonClient json{
        {"@type", "downloadFile"},
        {"file_id", fileId},
        {"priority", priority},
        {"offset", offset},
        {"limit", limit},
        {"synchronous", synchronous},
    };

    // clang-format on

    json.sendObject(clientId);
}

void TdApi::getMe()
{
    td_send(clientId, R"({"@type":"getMe"})");
}

void TdApi::setLogVerbosityLevel(qint32 newVerbosityLevel)
{
    JsonClient json{
        {"@type", "setLogVerbosityLevel"},
        {"new_verbosity_level", newVerbosityLevel},
    };

    json.sendObject(clientId);
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

    // clang-format off
    m_events.emplace("updateAuthorizationState", [this](const QVariantMap &data) {
        handleAuthorizationState(data);
    });

    m_events.emplace("updateConnectionState", [this](const QVariantMap &data) {
        emit updateConnectionState(data.value("state").toMap());
    });

    m_events.emplace("updateNewMessage", [this](const QVariantMap &data) {
        emit updateNewMessage(data.value("message").toMap());
    });
    m_events.emplace("updateMessageSendAcknowledged", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();

        emit updateMessageSendAcknowledged(chatId, messageId);
    });
    m_events.emplace("updateMessageSendSucceeded", [this](const QVariantMap &data) {
        auto message = data.value("message").toMap();
        auto oldMessageId = data.value("old_message_id").toLongLong();

        emit updateMessageSendSucceeded(message, oldMessageId);
    });
    m_events.emplace("updateMessageSendFailed", [this](const QVariantMap &data) {
        auto message = data.value("chat_id").toMap();
        auto oldMessageId = data.value("old_message_id").toLongLong();
        auto errorCode = data.value("error_code").toInt();
        auto errorMessage = data.value("error_message").toString();

        emit updateMessageSendFailed(message, oldMessageId, errorCode, errorMessage);
    });
    m_events.emplace("updateMessageContent", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto newContent = data.value("new_content").toMap();

        emit updateMessageContent(chatId, messageId, newContent);
    });
    m_events.emplace("updateMessageEdited", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto editDate = data.value("edit_date").toInt();
        auto replyMarkup = data.value("reply_markup").toMap();

        emit updateMessageEdited(chatId, messageId, editDate, replyMarkup);
    });
    m_events.emplace("updateMessageIsPinned", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto isPinned = data.value("is_pinned").toBool();

        emit updateMessageIsPinned(chatId, messageId, isPinned);
    });
    m_events.emplace("updateMessageInteractionInfo", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto interactionInfo = data.value("interaction_info").toMap();

        emit updateMessageInteractionInfo(chatId, messageId, interactionInfo);
    });
    m_events.emplace("updateMessageContentOpened", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();

        emit updateMessageContentOpened(chatId, messageId);
    });
    m_events.emplace("updateMessageMentionRead", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto unreadMentionCount = data.value("unread_mention_count").toInt();

        emit updateMessageMentionRead(chatId, messageId, unreadMentionCount);
    });
    m_events.emplace("updateMessageLiveLocationViewed", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();

        emit updateMessageLiveLocationViewed(chatId, messageId);
    });

    m_events.emplace("updateDeleteMessages", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageIds = data.value("message_ids").toList();
        auto isPermanent = data.value("is_permanent").toBool();
        auto fromCache = data.value("from_cache").toBool();

        emit updateDeleteMessages(chatId, messageIds, isPermanent, fromCache);
    });
    m_events.emplace("updateUnreadMessageCount", [this](const QVariantMap &data) {
        auto chatList = data.value("chat_list").toMap();
        auto unreadCount = data.value("unread_count").toInt();
        auto unreadUnmutedCount = data.value("unread_unmuted_count").toInt();

        emit updateUnreadMessageCount(chatList, unreadCount, unreadUnmutedCount);
    });
    m_events.emplace("updateUnreadChatCount", [this](const QVariantMap &data) {
        auto chatList = data.value("chat_list").toMap();
        auto totalCount = data.value("total_count").toInt();
        auto unreadCount = data.value("unread_count").toInt();
        auto unreadUnmutedCount = data.value("unread_unmuted_count").toInt();
        auto markedAsUnreadCount = data.value("marked_as_unread_count").toInt();
        auto markedAsUnreadUnmutedCount = data.value("marked_as_unread_unmuted_count").toInt();

        emit updateUnreadChatCount(chatList, totalCount, unreadCount, unreadUnmutedCount, markedAsUnreadCount, markedAsUnreadUnmutedCount);
    });
    m_events.emplace("updateActiveNotifications", [this](const QVariantMap &data) {
        emit updateActiveNotifications(data.value("groups").toList());
    });
    m_events.emplace("updateNotificationGroup", [this](const QVariantMap &data) {
        auto notificationGroupId = data.value("notification_group_id").toInt();
        auto type = data.value("type").toMap();
        auto chatId = data.value("chat_id").toLongLong();
        auto notificationSettingsChatId = data.value("notification_settings_chat_id").toLongLong();
        auto isSilent = data.value("is_silent").toBool();
        auto totalCount = data.value("total_count").toInt();
        auto addedNotifications = data.value("added_notifications").toList();
        auto removedNotificationIds = data.value("removed_notification_ids").toList();

        emit updateNotificationGroup(notificationGroupId, type, chatId,notificationSettingsChatId, isSilent, totalCount, addedNotifications, removedNotificationIds);
    });

    m_events.emplace("updateNewChat", [this](const QVariantMap &data) {
        emit updateNewChat(data.value("chat").toMap());
    });

    m_events.emplace("updateChatTitle", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto title = data.value("title").toString();
        emit updateChatTitle(chatId, title);
    });

    m_events.emplace("updateChatPhoto", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto photo = data.value("photo").toMap();
        emit updateChatPhoto(chatId, photo);
    });

    m_events.emplace("updateChatPermissions", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto permissions = data.value("permissions").toMap();
        emit updateChatPermissions(chatId, permissions);
    });

    m_events.emplace("updateChatLastMessage", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto lastMessage = data.value("last_message").toMap();
        auto positions = data.value("positions").toList();
        emit updateChatLastMessage(chatId, lastMessage, positions);
    });

    m_events.emplace("updateChatPosition", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto position = data.value("position").toMap();
        emit updateChatPosition(chatId, position);
    });

    m_events.emplace("updateChatIsMarkedAsUnread", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto isMarkedAsUnread = data.value("is_marked_as_unread").toBool();
        emit updateChatIsMarkedAsUnread(chatId, isMarkedAsUnread);
    });

    m_events.emplace("updateChatIsBlocked", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto isBlocked = data.value("is_blocked").toBool();
        emit updateChatIsBlocked(chatId, isBlocked);
    });

    m_events.emplace("updateChatHasScheduledMessages", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto hasScheduledMessages = data.value("has_scheduled_messages").toBool();
        emit updateChatHasScheduledMessages(chatId, hasScheduledMessages);
    });

    m_events.emplace("updateChatDefaultDisableNotification", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto defaultDisableNotification = data.value("default_disable_notification").toBool();
        emit updateChatDefaultDisableNotification(chatId, defaultDisableNotification);
    });

    m_events.emplace("updateChatReadInbox", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto lastReadInboxMessageId = data.value("last_read_inbox_message_id").toLongLong();
        auto unreadCount = data.value("unread_count").toInt();
        emit updateChatReadInbox(chatId, lastReadInboxMessageId, unreadCount);
    });

    m_events.emplace("updateChatReadOutbox", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto lastReadOutboxMessageId = data.value("last_read_outbox_message_id").toLongLong();
        emit updateChatReadOutbox(chatId, lastReadOutboxMessageId);
    });

    m_events.emplace("updateChatUnreadMentionCount", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto unreadMentionCount = data.value("unread_mention_count").toInt();
        emit updateChatUnreadMentionCount(chatId, unreadMentionCount);
    });

    m_events.emplace("updateChatNotificationSettings", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto notificationSettings = data.value("notification_settings").toMap();
        emit updateChatNotificationSettings(chatId, notificationSettings);
    });

    m_events.emplace("updateScopeNotificationSettings", [this](const QVariantMap &data) {
        auto scope = data.value("scope").toMap();
        auto notificationSettings = data.value("notification_settings").toMap();
        emit updateScopeNotificationSettings(scope, notificationSettings);
    });

    m_events.emplace("updateChatActionBar", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto actionBar = data.value("action_bar").toMap();
        emit updateChatActionBar(chatId, actionBar);
    });

    m_events.emplace("updateChatReplyMarkup", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto replyMarkupMessageId = data.value("reply_markup_message_id").toLongLong();
        emit updateChatReplyMarkup(chatId, replyMarkupMessageId);
    });

    m_events.emplace("updateChatDraftMessage", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto draftMessage = data.value("draft_message").toMap();
        auto position = data.value("position").toList();
        emit updateChatDraftMessage(chatId, draftMessage, position);
    });

    m_events.emplace("updateChatFilters", [this](const QVariantMap &data) {
        emit updateChatFilters(data.value("chat_filters").toList());
    });

    m_events.emplace("updateChatOnlineMemberCount", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto onlineMemberCount = data.value("onlineMemberCount").toInt();
        emit updateChatOnlineMemberCount(chatId, onlineMemberCount);
    });

    m_events.emplace("updateOption", [this](const QVariantMap &data) {
        emit updateOption(data.value("name").toString(), data.value("value").toMap());
    });

    m_events.emplace("updateUserChatAction", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageThreadId = data.value("message_thread_id").toLongLong();
        auto userId = data.value("user_id").toInt();
        auto action = data.value("action").toMap();
        emit updateUserChatAction(chatId, messageThreadId, userId, action);
    });

    m_events.emplace("updateUserStatus", [this](const QVariantMap &data) {
        auto userId = data.value("user_id").toInt();
        auto status = data.value("status").toMap();

        emit updateUserStatus(userId, status);
    });

    m_events.emplace("updateUser", [this](const QVariantMap &data) {
        emit updateUser(data.value("user").toMap());
    });

    m_events.emplace("updateBasicGroup", [this](const QVariantMap &data) {
        emit updateBasicGroup(data.value("basic_group").toMap());
    });

    m_events.emplace("updateSupergroup", [this](const QVariantMap &data) {
        emit updateSupergroup(data.value("supergroup").toMap());
    });

    m_events.emplace("updateUserFullInfo", [this](const QVariantMap &data) {
        auto userId = data.value("user_id").toInt();
        auto userFullInfo = data.value("user_full_info").toMap();
        emit updateUserFullInfo(userId, userFullInfo);
    });

    m_events.emplace("updateBasicGroupFullInfo", [this](const QVariantMap &data) {
        auto basicGroupId = data.value("basic_group_id").toInt();
        auto basicGroupFullInfo = data.value("basic_group_full_info").toMap();
        emit updateUserFullInfo(basicGroupId, basicGroupFullInfo);
    });

    m_events.emplace("updateSupergroupFullInfo", [this](const QVariantMap &data) {
        auto supergroupId = data.value("supergroup_id").toInt();
        auto supergroupFullInfo = data.value("supergroup_full_info").toMap();
        emit updateUserFullInfo(supergroupId, supergroupFullInfo);
    });

    m_events.emplace("updateFile", [this](const QVariantMap &data) {
        emit updateFile(data.value("file").toMap());
    });

    // Returns ...
    m_events.emplace("basicGroupFullInfo", [this](const QVariantMap &data) { emit basicGroupFullInfo(data); });
    m_events.emplace("chat", [this](const QVariantMap &data) { emit chat(data); });
    m_events.emplace("chatMembers", [this](const QVariantMap &data) { emit chatMembers(data); });
    m_events.emplace("chats", [this](const QVariantMap &data) { emit chats(data); });
    m_events.emplace("error", [this](const QVariantMap &data) { emit error(data); });
    m_events.emplace("file", [this](const QVariantMap &data) { emit file(data); });
    m_events.emplace("message", [this](const QVariantMap &data) { emit message(data); });
    m_events.emplace("messages", [this](const QVariantMap &data) { emit messages(data); });
    m_events.emplace("stickers", [this](const QVariantMap &data) { emit stickers(data); });
    m_events.emplace("stickerSet", [this](const QVariantMap &data) { emit stickerSet(data); });
    m_events.emplace("stickerSets", [this](const QVariantMap &data) { emit stickerSets(data); });
    m_events.emplace("supergroupFullInfo", [this](const QVariantMap &data) { emit supergroupFullInfo(data); });
    m_events.emplace("userFullInfo", [this](const QVariantMap &data) { emit userFullInfo(data); });
    m_events.emplace("userProfilePhotos", [this](const QVariantMap &data) { emit userProfilePhotos(data); });
    m_events.emplace("users", [this](const QVariantMap &data) { emit users(data); });

    // clang-format on
}

void TdApi::handleAuthorizationState(const QVariantMap &data)
{
    auto authorizationState = data.value("authorization_state").toMap();
    auto authorizationStateType = authorizationState.value("@type").toByteArray();

    if (authorizationStateType == "authorizationStateWaitTdlibParameters")
    {
        QVariantMap parameters;
        parameters.insert("database_directory", QString(QDir::homePath() % DatabaseDirectory));
        parameters.insert("use_file_database", true);
        parameters.insert("use_chat_info_database", true);
        parameters.insert("use_message_database", true);
        parameters.insert("use_secret_chats", true);
        parameters.insert("api_id", ApiId);
        parameters.insert("api_hash", ApiHash);
        parameters.insert("system_language_code", "en");
        parameters.insert("device_model", "Nokia N9");
        parameters.insert("system_version", "MeeGo 1.2 Harmattan");
        parameters.insert("application_version", AppVersion);

        JsonClient json{
            {"@type", "setTdlibParameters"},
            {"parameters", parameters},
        };

        json.sendObject(clientId);
    }

    if (authorizationStateType == "authorizationStateWaitEncryptionKey")
    {
        td_send(clientId, R"({"@type":"checkDatabaseEncryptionKey","encryption_key":""})");
    }

    if (authorizationStateType == "authorizationStateWaitPhoneNumber")
    {
        m_state = AuthorizationStateWaitPhoneNumber;
    }

    if (authorizationStateType == "authorizationStateReady")
    {
        m_state = AuthorizationStateReady;
        emit authorizationStateReady();
    }
    if (authorizationStateType == "authorizationStateClosed")
    {
        m_state = AuthorizationStateClosed;
        m_worker.request_stop();
    }

    if (authorizationStateType == "authorizationStateWaitCode")
    {
        m_state = AuthorizationStateWaitCode;
    }

    if (authorizationStateType == "authorizationStateWaitPassword")
    {
        m_state = AuthorizationStateWaitPassword;
    }

    if (authorizationStateType == "authorizationStateWaitRegistration")
    {
        m_state = AuthorizationStateWaitRegistration;
    }

    emit authorizationStateChanged(m_state);
    emit updateAuthorizationState(authorizationState);
}
