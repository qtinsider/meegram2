#include "TdApi.hpp"

#include "Common.hpp"
#include "Serialize.hpp"

#include "td/telegram/Client.h"
#include "td/telegram/td_api.h"
#include "td/telegram/td_api_json.h"

#include "td/utils/JsonBuilder.h"
#include "td/utils/Slice.h"
#include "td/utils/SliceBuilder.h"
#include "td/utils/StackAllocator.h"
#include "td/utils/StringBuilder.h"
#include "td/utils/common.h"
#include "td/utils/port/thread_local.h"

#include <QDebug>
#include <QDir>
#include <QEventLoop>
#include <QStringBuilder>
#include <QTimer>

namespace {

td::td_api::object_ptr<td::td_api::Function> toRequest(const std::string &request)
{
    auto request_str = request;
    auto r_json_value = td::json_decode(request_str);
    if (r_json_value.is_error())
    {
        return {};
    }

    auto json_value = r_json_value.move_as_ok();
    if (json_value.type() != td::JsonValue::Type::Object)
    {
        return {};
    }

    td::td_api::object_ptr<td::td_api::Function> func;
    auto status = from_json(func, std::move(json_value));
    if (status.is_error())
    {
        return {};
    }

    return func;
}

nlohmann::json fromResponse(const td::td_api::Object &object, int client_id)
{
    auto value = td::json_encode<std::string>(td::ToJson(object));

    auto result = nlohmann::json::parse(value);
    if (client_id != 0)
    {
        result.emplace("@client_id", client_id);
    }

    return result;
}

}  // namespace

namespace detail {

QString getAuthenticationCodeTitle(const QVariantMap &codeInfo)
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

QString getAuthenticationCodeSubtitle(const QVariantMap &codeInfo)
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

QString getAuthenticationCodeNextTypeString(const QVariantMap &codeInfo)
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

bool getAuthenticationCodeIsNextTypeSms(const QVariantMap &codeInfo)
{
    auto codeInfoType = codeInfo.value("next_type").toMap().value("@type").toByteArray();

    return codeInfoType == "authenticationCodeTypeSms";
}

int getAuthenticationCodeLength(const QVariantMap &codeInfo)
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

}  // namespace detail

TdApi::TdApi()
{
    // disable TDLib logging
    td::ClientManager::execute(td::td_api::make_object<td::td_api::setLogVerbosityLevel>(1));

    initEvents();
}

TdApi &TdApi::getInstance()
{
    static TdApi staticObject;
    return staticObject;
}

std::atomic<std::uint64_t> g_requestId;

void TdApi::sendRequest(const QVariantMap &object, std::function<void(const QVariantMap &)> callback)
{
    auto requestId = g_requestId.fetch_add(1, std::memory_order_relaxed);
    if (callback)
    {
        m_handlers.emplace(requestId, std::move(callback));
    }

    auto request = toRequest(nlohmann::json(object).dump());
    td::ClientManager::get_manager_singleton()->send(clientId, requestId, std::move(request));
}

void TdApi::log(const QVariantMap &object) noexcept
{
    nlohmann::json json(object);
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
    QVariantMap result;
    result.insert("@type", "logOut");

    sendRequest(result);
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
    QVariantMap result;
    result.insert("@type", "resendAuthenticationCode");

    sendRequest(result);
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
    QVariantMap result;
    result.insert("@type", "close");

    sendRequest(result);

    // 1 sec delay
    QEventLoop loop;
    QTimer::singleShot(1000, &loop, SLOT(quit()));
    loop.exec();
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

void TdApi::getChatFilter(qint32 chatFilterId)
{
    QVariantMap result;
    result.insert("@type", "getChatFilter");
    result.insert("chat_filter_id", chatFilterId);

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

void TdApi::sendChatAction(qint64 chatId, qint64 messageThreadId, const QVariantMap &action)
{
    QVariantMap result;
    result.insert("@type", "sendChatAction");
    result.insert("chat_id", chatId);
    result.insert("message_thread_id", messageThreadId);
    result.insert("action", action);

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
            auto response = td::ClientManager::get_manager_singleton()->receive(30.0);

            if (response.object != nullptr)
            {
                if (auto result = fromResponse(*response.object, response.client_id); response.request_id == 0)
                {
                    if (auto it = m_events.find(result["@type"].template get<std::string>()); it != m_events.end())
                        it->second(result);
                }
                else if (auto it = m_handlers.find(response.request_id); it != m_handlers.end())
                {
                    it->second(result);
                    m_handlers.erase(it);
                }
            }
        }
    });
}

void TdApi::initEvents()
{
    clientId = td::ClientManager::get_manager_singleton()->create_client_id();

    // start the client by sending request to it
    setOption("version", {});

    m_events.emplace("updateAuthorizationState",
                     [this](const QVariantMap &data) { emit updateAuthorizationState(data.value("authorization_state").toMap()); });
    m_events.emplace("updateNewMessage", [this](const QVariantMap &data) { emit updateNewMessage(data.value("message").toMap()); });
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
        auto message = data.value("message").toMap();
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
    m_events.emplace("updateMessageUnreadReactions", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto unreadReactions = data.value("unread_reactions").toList();
        auto unreadReactionCount = data.value("unread_reaction_count").toInt();
        emit updateMessageUnreadReactions(chatId, messageId, unreadReactions, unreadReactionCount);
    });
    m_events.emplace("updateMessageLiveLocationViewed", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        emit updateMessageLiveLocationViewed(chatId, messageId);
    });
    m_events.emplace("updateNewChat", [this](const QVariantMap &data) { emit updateNewChat(data.value("chat").toMap()); });
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
    m_events.emplace("updateChatActionBar", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto actionBar = data.value("action_bar").toMap();
        emit updateChatActionBar(chatId, actionBar);
    });
    m_events.emplace("updateChatAvailableReactions", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto availableReactions = data.value("available_reactions").toStringList();
        emit updateChatAvailableReactions(chatId, availableReactions);
    });
    m_events.emplace("updateChatDraftMessage", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto draftMessage = data.value("draft_message").toMap();
        auto positions = data.value("positions").toList();
        emit updateChatDraftMessage(chatId, draftMessage, positions);
    });
    m_events.emplace("updateChatMessageSender", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageSenderId = data.value("message_sender_id").toMap();
        emit updateChatMessageSender(chatId, messageSenderId);
    });
    m_events.emplace("updateChatMessageTtl", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageTtl = data.value("message_ttl").toInt();
        emit updateChatMessageTtl(chatId, messageTtl);
    });
    m_events.emplace("updateChatNotificationSettings", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto notificationSettings = data.value("notification_settings").toMap();
        emit updateChatNotificationSettings(chatId, notificationSettings);
    });
    m_events.emplace("updateChatPendingJoinRequests", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto pendingJoinRequests = data.value("pending_join_requests").toMap();
        emit updateChatPendingJoinRequests(chatId, pendingJoinRequests);
    });
    m_events.emplace("updateChatReplyMarkup", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto replyMarkupMessageId = data.value("reply_markup_message_id").toLongLong();
        emit updateChatReplyMarkup(chatId, replyMarkupMessageId);
    });
    m_events.emplace("updateChatTheme", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto themeName = data.value("theme_name").toString();
        emit updateChatTheme(chatId, themeName);
    });
    m_events.emplace("updateChatUnreadMentionCount", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto unreadMentionCount = data.value("unread_mention_count").toInt();
        emit updateChatUnreadMentionCount(chatId, unreadMentionCount);
    });
    m_events.emplace("updateChatUnreadReactionCount", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto unreadReactionCount = data.value("unread_reaction_count").toInt();
        emit updateChatUnreadReactionCount(chatId, unreadReactionCount);
    });
    m_events.emplace("updateChatVideoChat", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto videoChat = data.value("video_chat").toMap();
        emit updateChatVideoChat(chatId, videoChat);
    });
    m_events.emplace("updateChatDefaultDisableNotification", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto defaultDisableNotification = data.value("default_disable_notification").toBool();
        emit updateChatDefaultDisableNotification(chatId, defaultDisableNotification);
    });
    m_events.emplace("updateChatHasProtectedContent", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto hasProtectedContent = data.value("has_protected_content").toBool();
        emit updateChatHasProtectedContent(chatId, hasProtectedContent);
    });
    m_events.emplace("updateChatHasScheduledMessages", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto hasScheduledMessages = data.value("has_scheduled_messages").toBool();
        emit updateChatHasScheduledMessages(chatId, hasScheduledMessages);
    });
    m_events.emplace("updateChatIsBlocked", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto isBlocked = data.value("is_blocked").toBool();
        emit updateChatIsBlocked(chatId, isBlocked);
    });
    m_events.emplace("updateChatIsMarkedAsUnread", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto isMarkedAsUnread = data.value("is_marked_as_unread").toBool();
        emit updateChatIsMarkedAsUnread(chatId, isMarkedAsUnread);
    });
    m_events.emplace("updateChatFilters", [this](const QVariantMap &data) { emit updateChatFilters(data.value("chat_filters").toList()); });
    m_events.emplace("updateChatOnlineMemberCount", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto onlineMemberCount = data.value("online_member_count").toInt();
        emit updateChatOnlineMemberCount(chatId, onlineMemberCount);
    });

    m_events.emplace("updateScopeNotificationSettings", [this](const QVariantMap &data) {
        auto scope = data.value("scope").toMap();
        auto notificationSettings = data.value("notification_settings").toMap();
        emit updateScopeNotificationSettings(scope, notificationSettings);
    });
    m_events.emplace("updateNotification", [this](const QVariantMap &data) {
        auto notificationGroupId = data.value("notification_group_id").toInt();
        auto notification = data.value("notification").toMap();
        emit updateNotification(notificationGroupId, notification);
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
        emit updateNotificationGroup(notificationGroupId, type, chatId, notificationSettingsChatId, isSilent, totalCount,
                                     addedNotifications, removedNotificationIds);
    });
    m_events.emplace("updateActiveNotifications",
                     [this](const QVariantMap &data) { emit updateActiveNotifications(data.value("groups").toList()); });
    m_events.emplace("updateHavePendingNotifications", [this](const QVariantMap &data) {
        auto haveDelayedNotifications = data.value("have_delayed_notifications").toBool();
        auto haveUnreceivedNotifications = data.value("message_id").toBool();
        emit updateHavePendingNotifications(haveDelayedNotifications, haveUnreceivedNotifications);
    });
    m_events.emplace("updateDeleteMessages", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageIds = data.value("message_ids").toList();
        auto isPermanent = data.value("is_permanent").toBool();
        auto fromCache = data.value("from_cache").toBool();
        emit updateDeleteMessages(chatId, messageIds, isPermanent, fromCache);
    });
    m_events.emplace("updateChatAction", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageThreadId = data.value("message_thread_id").toLongLong();
        auto senderId = data.value("sender_id").toMap();
        auto action = data.value("action").toMap();
        emit updateChatAction(chatId, messageThreadId, senderId, action);
    });
    m_events.emplace("updateUserStatus", [this](const QVariantMap &data) {
        auto userId = data.value("user_id").toLongLong();
        auto status = data.value("status").toMap();
        emit updateUserStatus(userId, status);
    });
    m_events.emplace("updateUser", [this](const QVariantMap &data) { emit updateUser(data.value("user").toMap()); });
    m_events.emplace("updateBasicGroup", [this](const QVariantMap &data) { emit updateBasicGroup(data.value("basic_group").toMap()); });
    m_events.emplace("updateSupergroup", [this](const QVariantMap &data) { emit updateSupergroup(data.value("supergroup").toMap()); });
    m_events.emplace("updateSecretChat", [this](const QVariantMap &data) { emit updateSecretChat(data.value("secret_chat").toMap()); });
    m_events.emplace("updateUserFullInfo", [this](const QVariantMap &data) {
        auto userId = data.value("user_id").toLongLong();
        auto userFullInfo = data.value("user_full_info").toMap();
        emit updateUserFullInfo(userId, userFullInfo);
    });
    m_events.emplace("updateBasicGroupFullInfo", [this](const QVariantMap &data) {
        auto basicGroupId = data.value("basic_group_id").toLongLong();
        auto basicGroupFullInfo = data.value("basic_group_full_info").toMap();
        emit updateBasicGroupFullInfo(basicGroupId, basicGroupFullInfo);
    });
    m_events.emplace("updateSupergroupFullInfo", [this](const QVariantMap &data) {
        auto supergroupId = data.value("supergroup_id").toLongLong();
        auto supergroupFullInfo = data.value("supergroup_full_info").toMap();
        emit updateSupergroupFullInfo(supergroupId, supergroupFullInfo);
    });
    m_events.emplace("updateServiceNotification", [this](const QVariantMap &data) {
        auto type = data.value("type").toString();
        auto content = data.value("message_id").toMap();
        emit updateServiceNotification(type, content);
    });
    m_events.emplace("updateFile", [this](const QVariantMap &data) { emit updateFile(data.value("file").toMap()); });
    m_events.emplace("updateFileGenerationStart", [this](const QVariantMap &data) {
        auto generationId = data.value("generation_id").toLongLong();
        auto originalPath = data.value("original_path").toString();
        auto destinationPath = data.value("destination_path").toString();
        auto conversion = data.value("conversion").toString();
        emit updateFileGenerationStart(generationId, originalPath, destinationPath, conversion);
    });
    m_events.emplace("updateFileGenerationStop",
                     [this](const QVariantMap &data) { emit updateFileGenerationStop(data.value("generation_id").toLongLong()); });
    m_events.emplace("updateCall", [this](const QVariantMap &data) { emit updateCall(data.value("call").toMap()); });
    m_events.emplace("updateGroupCall", [this](const QVariantMap &data) { emit updateGroupCall(data.value("group_call").toMap()); });
    m_events.emplace("updateGroupCallParticipant", [this](const QVariantMap &data) {
        auto groupCallId = data.value("group_call_id").toInt();
        auto participant = data.value("participant").toMap();
        emit updateGroupCallParticipant(groupCallId, participant);
    });
    m_events.emplace("updateGroupCallParticipant", [this](const QVariantMap &data) {
        auto groupCallId = data.value("group_call_id").toInt();
        auto participant = data.value("participant").toMap();
        emit updateGroupCallParticipant(groupCallId, participant);
    });
    m_events.emplace("updateNewCallSignalingData", [this](const QVariantMap &data) {
        auto callId = data.value("call_id").toInt();
        auto _data = data.value("data").toByteArray();
        emit updateNewCallSignalingData(callId, _data);
    });
    m_events.emplace("updateUserPrivacySettingRules", [this](const QVariantMap &data) {
        auto setting = data.value("setting").toMap();
        auto rules = data.value("rules").toMap();
        emit updateUserPrivacySettingRules(setting, rules);
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
    m_events.emplace("updateOption", [this](const QVariantMap &data) {
        auto name = data.value("name").toString();
        auto value = data.value("value").toMap();
        emit updateOption(name, value);
    });
    m_events.emplace("updateStickerSet", [this](const QVariantMap &data) { emit updateStickerSet(data.value("sticker_set").toMap()); });
    m_events.emplace("updateInstalledStickerSets", [this](const QVariantMap &data) {
        auto isMasks = data.value("is_masks").toBool();
        auto stickerSetIds = data.value("sticker_set_ids").toList();
        emit updateInstalledStickerSets(isMasks, stickerSetIds);
    });
    m_events.emplace("updateTrendingStickerSets",
                     [this](const QVariantMap &data) { emit updateTrendingStickerSets(data.value("sticker_sets").toMap()); });
    m_events.emplace("updateRecentStickers", [this](const QVariantMap &data) {
        auto isAttached = data.value("is_attached").toBool();
        auto stickerIds = data.value("sticker_ids").toList();
        emit updateRecentStickers(isAttached, stickerIds);
    });
    m_events.emplace("updateFavoriteStickers",
                     [this](const QVariantMap &data) { emit updateFavoriteStickers(data.value("sticker_ids").toList()); });
    m_events.emplace("updateSavedAnimations",
                     [this](const QVariantMap &data) { emit updateSavedAnimations(data.value("animation_ids").toList()); });
    m_events.emplace("updateSelectedBackground", [this](const QVariantMap &data) {
        auto forDarkTheme = data.value("for_dark_theme").toBool();
        auto background = data.value("background").toMap();
        emit updateSelectedBackground(forDarkTheme, background);
    });
    m_events.emplace("updateChatThemes", [this](const QVariantMap &data) { emit updateChatThemes(data.value("chat_themes").toList()); });
    m_events.emplace("updateLanguagePackStrings", [this](const QVariantMap &data) {
        auto localizationTarget = data.value("localization_target").toString();
        auto languagePackId = data.value("language_pack_id").toString();
        auto strings = data.value("strings").toList();
        emit updateLanguagePackStrings(localizationTarget, languagePackId, strings);
    });
    m_events.emplace("updateConnectionState", [this](const QVariantMap &data) { emit updateConnectionState(data.value("state").toMap()); });
    m_events.emplace("updateTermsOfService", [this](const QVariantMap &data) {
        auto termsOfServiceId = data.value("terms_of_service_id").toString();
        auto termsOfService = data.value("terms_of_service").toMap();
        emit updateTermsOfService(termsOfServiceId, termsOfService);
    });
    m_events.emplace("updateUsersNearby", [this](const QVariantMap &data) { emit updateUsersNearby(data.value("users_nearby").toList()); });
    m_events.emplace("updateReactions", [this](const QVariantMap &data) { emit updateReactions(data.value("reactions").toList()); });
    m_events.emplace("updateDiceEmojis", [this](const QVariantMap &data) { emit updateDiceEmojis(data.value("emojis").toStringList()); });
    m_events.emplace("updateAnimatedEmojiMessageClicked", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto sticker = data.value("sticker").toMap();
        emit updateAnimatedEmojiMessageClicked(chatId, messageId, sticker);
    });
    m_events.emplace("updateAnimationSearchParameters", [this](const QVariantMap &data) {
        auto provider = data.value("provider").toString();
        auto emojis = data.value("emojis").toStringList();
        emit updateAnimationSearchParameters(provider, emojis);
    });
    m_events.emplace("updateSuggestedActions", [this](const QVariantMap &data) {
        auto addedActions = data.value("added_actions").toList();
        auto removedActions = data.value("removed_actions").toList();
        emit updateSuggestedActions(addedActions, removedActions);
    });
    m_events.emplace("updateNewInlineQuery", [this](const QVariantMap &data) {
        auto id = data.value("id").toLongLong();
        auto senderUserId = data.value("sender_user_id").toLongLong();
        auto userLocation = data.value("user_location").toMap();
        auto chatType = data.value("chat_type").toMap();
        auto query = data.value("query").toString();
        auto offset = data.value("offset").toString();
        emit updateNewInlineQuery(id, senderUserId, userLocation, chatType, query, offset);
    });

    m_events.emplace("updateNewChosenInlineResult", [this](const QVariantMap &data) {
        auto senderUserId = data.value("sender_user_id").toLongLong();
        auto userLocation = data.value("user_location").toMap();
        auto query = data.value("query").toString();
        auto resultId = data.value("result_id").toString();
        auto inlineMessageId = data.value("inline_message_id").toString();
        emit updateNewChosenInlineResult(senderUserId, userLocation, query, resultId, inlineMessageId);
    });
    m_events.emplace("updateNewCallbackQuery", [this](const QVariantMap &data) {
        auto id = data.value("id").toLongLong();
        auto senderUserId = data.value("sender_user_id").toLongLong();
        auto chatId = data.value("chat_id").toLongLong();
        auto messageId = data.value("message_id").toLongLong();
        auto chatInstance = data.value("chat_instance").toLongLong();
        auto payload = data.value("payload").toMap();
        emit updateNewCallbackQuery(id, senderUserId, chatId, messageId, chatInstance, payload);
    });
    m_events.emplace("updateNewInlineCallbackQuery", [this](const QVariantMap &data) {
        auto id = data.value("id").toLongLong();
        auto senderUserId = data.value("sender_user_id").toLongLong();
        auto inlineMessageId = data.value("inline_message_id").toString();
        auto chatInstance = data.value("chat_instance").toLongLong();
        auto payload = data.value("payload").toMap();
        emit updateNewInlineCallbackQuery(id, senderUserId, inlineMessageId, chatInstance, payload);
    });
    m_events.emplace("updateNewShippingQuery", [this](const QVariantMap &data) {
        auto id = data.value("id").toLongLong();
        auto senderUserId = data.value("sender_user_id").toLongLong();
        auto invoicePayload = data.value("invoice_payload").toString();
        auto shippingAddress = data.value("shipping_address").toMap();
        emit updateNewShippingQuery(id, senderUserId, invoicePayload, shippingAddress);
    });
    m_events.emplace("updateNewPreCheckoutQuery", [this](const QVariantMap &data) {
        auto id = data.value("id").toLongLong();
        auto senderUserId = data.value("sender_user_id").toLongLong();
        auto currency = data.value("currency").toString();
        auto totalAmount = data.value("total_amount").toLongLong();
        auto invoicePayload = data.value("invoice_payload").toByteArray();
        auto shippingOptionId = data.value("shipping_option_id").toString();
        auto orderInfo = data.value("order_info").toMap();
        emit updateNewPreCheckoutQuery(id, senderUserId, currency, totalAmount, invoicePayload, shippingOptionId, orderInfo);
    });
    m_events.emplace("updateNewCustomEvent",
                     [this](const QVariantMap &data) { emit updateNewCustomEvent(data.value("event").toString()); });
    m_events.emplace("updateNewCustomQuery", [this](const QVariantMap &data) {
        auto id = data.value("id").toLongLong();
        auto _data = data.value("data").toString();
        auto timeout = data.value("timeout").toInt();
        emit updateNewCustomQuery(id, _data, timeout);
    });
    m_events.emplace("updatePoll", [this](const QVariantMap &data) { emit updatePoll(data.value("poll").toMap()); });
    m_events.emplace("updatePollAnswer", [this](const QVariantMap &data) {
        auto pollId = data.value("poll_id").toLongLong();
        auto userId = data.value("user_id").toLongLong();
        auto optionIds = data.value("option_ids").toList();
        emit updatePollAnswer(pollId, userId, optionIds);
    });
    m_events.emplace("updateChatMember", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto actorUserId = data.value("actor_user_id").toLongLong();
        auto date = data.value("date").toInt();
        auto inviteLink = data.value("invite_link").toMap();
        auto oldChatMember = data.value("old_chat_member").toMap();
        auto newChatMember = data.value("new_chat_member").toMap();
        emit updateChatMember(chatId, actorUserId, date, inviteLink, oldChatMember, newChatMember);
    });
    m_events.emplace("updateNewChatJoinRequest", [this](const QVariantMap &data) {
        auto chatId = data.value("chat_id").toLongLong();
        auto request = data.value("request").toMap();
        auto inviteLink = data.value("invite_link").toMap();
        emit updateNewChatJoinRequest(chatId, request, inviteLink);
    });

    // Returns ...
    m_events.emplace("message", [this](const QVariantMap &data) { emit message(data); });
    m_events.emplace("ok", [this](const QVariantMap &data) { emit ok(data); });
    m_events.emplace("proxy", [this](const QVariantMap &data) { emit proxy(data); });
    m_events.emplace("stickers", [this](const QVariantMap &data) { emit stickers(data); });
    m_events.emplace("stickerSet", [this](const QVariantMap &data) { emit stickerSet(data); });
    m_events.emplace("CanTransferOwnershipResult", [this](const QVariantMap &data) { emit canTransferOwnershipResult(data); });
    m_events.emplace("importedContacts", [this](const QVariantMap &data) { emit importedContacts(data); });
    m_events.emplace("authenticationCodeInfo", [this](const QVariantMap &data) { emit authenticationCodeInfo(data); });
    m_events.emplace("chatInviteLinkInfo", [this](const QVariantMap &data) { emit chatInviteLinkInfo(data); });
    m_events.emplace("OptionValue", [this](const QVariantMap &data) { emit optionValue(data); });
    m_events.emplace("passportAuthorizationForm", [this](const QVariantMap &data) { emit passportAuthorizationForm(data); });
    m_events.emplace("passportElementsWithErrors", [this](const QVariantMap &data) { emit passportElementsWithErrors(data); });
    m_events.emplace("PassportElement", [this](const QVariantMap &data) { emit passportElement(data); });
    m_events.emplace("testBytes", [this](const QVariantMap &data) { emit testBytes(data); });
    m_events.emplace("testString", [this](const QVariantMap &data) { emit testString(data); });
    m_events.emplace("testVectorInt", [this](const QVariantMap &data) { emit testVectorInt(data); });
    m_events.emplace("testVectorString", [this](const QVariantMap &data) { emit testVectorString(data); });
    m_events.emplace("testVectorStringObject", [this](const QVariantMap &data) { emit testVectorStringObject(data); });
    m_events.emplace("error", [this](const QVariantMap &data) { emit error(data); });
    m_events.emplace("testInt", [this](const QVariantMap &data) { emit testInt(data); });
    m_events.emplace("tMeUrls", [this](const QVariantMap &data) { emit tMeUrls(data); });
    m_events.emplace("textEntities", [this](const QVariantMap &data) { emit textEntities(data); });
    m_events.emplace("userFullInfo", [this](const QVariantMap &data) { emit userFullInfo(data); });
    m_events.emplace("userPrivacySettingRules", [this](const QVariantMap &data) { emit userPrivacySettingRules(data); });
    m_events.emplace("chatPhotos", [this](const QVariantMap &data) { emit chatPhotos(data); });
    m_events.emplace("chatMembers", [this](const QVariantMap &data) { emit chatMembers(data); });
    m_events.emplace("chatsNearby", [this](const QVariantMap &data) { emit chatsNearby(data); });
    m_events.emplace("emojis", [this](const QVariantMap &data) { emit emojis(data); });
    m_events.emplace("hashtags", [this](const QVariantMap &data) { emit hashtags(data); });
    m_events.emplace("customRequestResult", [this](const QVariantMap &data) { emit customRequestResult(data); });
    m_events.emplace("emailAddressAuthenticationCodeInfo",
                     [this](const QVariantMap &data) { emit emailAddressAuthenticationCodeInfo(data); });
    m_events.emplace("ResetPasswordResult", [this](const QVariantMap &data) { emit resetPasswordResult(data); });
    m_events.emplace("background", [this](const QVariantMap &data) { emit background(data); });
    m_events.emplace("networkStatistics", [this](const QVariantMap &data) { emit networkStatistics(data); });
    m_events.emplace("CheckChatUsernameResult", [this](const QVariantMap &data) { emit checkChatUsernameResult(data); });
    m_events.emplace("passwordState", [this](const QVariantMap &data) { emit passwordState(data); });
    m_events.emplace("CheckStickerSetNameResult", [this](const QVariantMap &data) { emit checkStickerSetNameResult(data); });
    m_events.emplace("text", [this](const QVariantMap &data) { emit text(data); });
    m_events.emplace("sticker", [this](const QVariantMap &data) { emit sticker(data); });
    m_events.emplace("session", [this](const QVariantMap &data) { emit session(data); });
    m_events.emplace("chat", [this](const QVariantMap &data) { emit chat(data); });
    m_events.emplace("callId", [this](const QVariantMap &data) { emit callId(data); });
    m_events.emplace("chatFilterInfo", [this](const QVariantMap &data) { emit chatFilterInfo(data); });
    m_events.emplace("chatInviteLink", [this](const QVariantMap &data) { emit chatInviteLink(data); });
    m_events.emplace("temporaryPasswordState", [this](const QVariantMap &data) { emit temporaryPasswordState(data); });
    m_events.emplace("groupCallId", [this](const QVariantMap &data) { emit groupCallId(data); });
    m_events.emplace("file", [this](const QVariantMap &data) { emit file(data); });
    m_events.emplace("messages", [this](const QVariantMap &data) { emit messages(data); });
    m_events.emplace("accountTtl", [this](const QVariantMap &data) { emit accountTtl(data); });
    m_events.emplace("sessions", [this](const QVariantMap &data) { emit sessions(data); });
    m_events.emplace("passportElements", [this](const QVariantMap &data) { emit passportElements(data); });
    m_events.emplace("animatedEmoji", [this](const QVariantMap &data) { emit animatedEmoji(data); });
    m_events.emplace("JsonValue", [this](const QVariantMap &data) { emit jsonValue(data); });
    m_events.emplace("httpUrl", [this](const QVariantMap &data) { emit httpUrl(data); });
    m_events.emplace("AuthorizationState", [this](const QVariantMap &data) { emit authorizationState(data); });
    m_events.emplace("stickerSets", [this](const QVariantMap &data) { emit stickerSets(data); });
    m_events.emplace("autoDownloadSettingsPresets", [this](const QVariantMap &data) { emit autoDownloadSettingsPresets(data); });
    m_events.emplace("backgrounds", [this](const QVariantMap &data) { emit backgrounds(data); });
    m_events.emplace("bankCardInfo", [this](const QVariantMap &data) { emit bankCardInfo(data); });
    m_events.emplace("basicGroup", [this](const QVariantMap &data) { emit basicGroup(data); });
    m_events.emplace("basicGroupFullInfo", [this](const QVariantMap &data) { emit basicGroupFullInfo(data); });
    m_events.emplace("messageSenders", [this](const QVariantMap &data) { emit messageSenders(data); });
    m_events.emplace("callbackQueryAnswer", [this](const QVariantMap &data) { emit callbackQueryAnswer(data); });
    m_events.emplace("chatAdministrators", [this](const QVariantMap &data) { emit chatAdministrators(data); });
    m_events.emplace("chatEvents", [this](const QVariantMap &data) { emit chatEvents(data); });
    m_events.emplace("chatFilter", [this](const QVariantMap &data) { emit chatFilter(data); });
    m_events.emplace("chatInviteLinkCounts", [this](const QVariantMap &data) { emit chatInviteLinkCounts(data); });
    m_events.emplace("chatInviteLinkMembers", [this](const QVariantMap &data) { emit chatInviteLinkMembers(data); });
    m_events.emplace("chatJoinRequests", [this](const QVariantMap &data) { emit chatJoinRequests(data); });
    m_events.emplace("chatInviteLinks", [this](const QVariantMap &data) { emit chatInviteLinks(data); });
    m_events.emplace("chatLists", [this](const QVariantMap &data) { emit chatLists(data); });
    m_events.emplace("Update", [this](const QVariantMap &data) { emit update(data); });
    m_events.emplace("validatedOrderInfo", [this](const QVariantMap &data) { emit validatedOrderInfo(data); });
    m_events.emplace("paymentForm", [this](const QVariantMap &data) { emit paymentForm(data); });
    m_events.emplace("paymentReceipt", [this](const QVariantMap &data) { emit paymentReceipt(data); });
    m_events.emplace("phoneNumberInfo", [this](const QVariantMap &data) { emit phoneNumberInfo(data); });
    m_events.emplace("proxies", [this](const QVariantMap &data) { emit proxies(data); });
    m_events.emplace("pushReceiverId", [this](const QVariantMap &data) { emit pushReceiverId(data); });
    m_events.emplace("paymentResult", [this](const QVariantMap &data) { emit paymentResult(data); });
    m_events.emplace("recommendedChatFilters", [this](const QVariantMap &data) { emit recommendedChatFilters(data); });
    m_events.emplace("recoveryEmailAddress", [this](const QVariantMap &data) { emit recoveryEmailAddress(data); });
    m_events.emplace("animations", [this](const QVariantMap &data) { emit animations(data); });
    m_events.emplace("orderInfo", [this](const QVariantMap &data) { emit orderInfo(data); });
    m_events.emplace("scopeNotificationSettings", [this](const QVariantMap &data) { emit scopeNotificationSettings(data); });
    m_events.emplace("secretChat", [this](const QVariantMap &data) { emit secretChat(data); });
    m_events.emplace("storageStatistics", [this](const QVariantMap &data) { emit storageStatistics(data); });
    m_events.emplace("storageStatisticsFast", [this](const QVariantMap &data) { emit storageStatisticsFast(data); });
    m_events.emplace("supergroup", [this](const QVariantMap &data) { emit supergroup(data); });
    m_events.emplace("supergroupFullInfo", [this](const QVariantMap &data) { emit supergroupFullInfo(data); });
    m_events.emplace("seconds", [this](const QVariantMap &data) { emit seconds(data); });
    m_events.emplace("StatisticalGraph", [this](const QVariantMap &data) { emit statisticalGraph(data); });
    m_events.emplace("webPageInstantView", [this](const QVariantMap &data) { emit webPageInstantView(data); });
    m_events.emplace("webPage", [this](const QVariantMap &data) { emit webPage(data); });
    m_events.emplace("chatMember", [this](const QVariantMap &data) { emit chatMember(data); });
    m_events.emplace("messageCalendar", [this](const QVariantMap &data) { emit messageCalendar(data); });
    m_events.emplace("count", [this](const QVariantMap &data) { emit count(data); });
    m_events.emplace("chats", [this](const QVariantMap &data) { emit chats(data); });
    m_events.emplace("messagePositions", [this](const QVariantMap &data) { emit messagePositions(data); });
    m_events.emplace("sponsoredMessage", [this](const QVariantMap &data) { emit sponsoredMessage(data); });
    m_events.emplace("ChatStatistics", [this](const QVariantMap &data) { emit chatStatistics(data); });
    m_events.emplace("botCommands", [this](const QVariantMap &data) { emit botCommands(data); });
    m_events.emplace("connectedWebsites", [this](const QVariantMap &data) { emit connectedWebsites(data); });
    m_events.emplace("users", [this](const QVariantMap &data) { emit users(data); });
    m_events.emplace("countries", [this](const QVariantMap &data) { emit countries(data); });
    m_events.emplace("updates", [this](const QVariantMap &data) { emit updates(data); });
    m_events.emplace("databaseStatistics", [this](const QVariantMap &data) { emit databaseStatistics(data); });
    m_events.emplace("deepLinkInfo", [this](const QVariantMap &data) { emit deepLinkInfo(data); });
    m_events.emplace("LoginUrlInfo", [this](const QVariantMap &data) { emit loginUrlInfo(data); });
    m_events.emplace("gameHighScores", [this](const QVariantMap &data) { emit gameHighScores(data); });
    m_events.emplace("groupCall", [this](const QVariantMap &data) { emit groupCall(data); });
    m_events.emplace("filePart", [this](const QVariantMap &data) { emit filePart(data); });
    m_events.emplace("inlineQueryResults", [this](const QVariantMap &data) { emit inlineQueryResults(data); });
    m_events.emplace("InternalLinkType", [this](const QVariantMap &data) { emit internalLinkType(data); });
    m_events.emplace("languagePackInfo", [this](const QVariantMap &data) { emit languagePackInfo(data); });
    m_events.emplace("LanguagePackStringValue", [this](const QVariantMap &data) { emit languagePackStringValue(data); });
    m_events.emplace("languagePackStrings", [this](const QVariantMap &data) { emit languagePackStrings(data); });
    m_events.emplace("localizationTargetInfo", [this](const QVariantMap &data) { emit localizationTargetInfo(data); });
    m_events.emplace("LogStream", [this](const QVariantMap &data) { emit logStream(data); });
    m_events.emplace("logVerbosityLevel", [this](const QVariantMap &data) { emit logVerbosityLevel(data); });
    m_events.emplace("logTags", [this](const QVariantMap &data) { emit logTags(data); });
    m_events.emplace("formattedText", [this](const QVariantMap &data) { emit formattedText(data); });
    m_events.emplace("user", [this](const QVariantMap &data) { emit user(data); });
    m_events.emplace("addedReactions", [this](const QVariantMap &data) { emit addedReactions(data); });
    m_events.emplace("availableReactions", [this](const QVariantMap &data) { emit availableReactions(data); });
    m_events.emplace("MessageFileType", [this](const QVariantMap &data) { emit messageFileType(data); });
    m_events.emplace("messageLink", [this](const QVariantMap &data) { emit messageLink(data); });
    m_events.emplace("messageLinkInfo", [this](const QVariantMap &data) { emit messageLinkInfo(data); });
    m_events.emplace("foundMessages", [this](const QVariantMap &data) { emit foundMessages(data); });
    m_events.emplace("messageStatistics", [this](const QVariantMap &data) { emit messageStatistics(data); });
    m_events.emplace("messageThreadInfo", [this](const QVariantMap &data) { emit messageThreadInfo(data); });

    connect(this, SIGNAL(updateAuthorizationState(const QVariantMap &)), SLOT(handleAuthorizationState(const QVariantMap &)));
}

void TdApi::handleAuthorizationState(const QVariantMap &authorizationState)
{
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
            parameters.insert("system_language_code", DefaultLanguageCode);
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
            QVariantMap result;
            result.insert("@type", "checkDatabaseEncryptionKey");
            result.insert("encryption_key", "");

            sendRequest(result);
            break;
        }
        case fnv::hash("authorizationStateWaitPhoneNumber"): {
            emit isAuthorizedChanged();
            break;
        }
        case fnv::hash("authorizationStateWaitCode"): {
            const auto codeInfo = authorizationState.value("code_info").toMap();

            QVariantMap result;
            result.insert("subtitle", detail::getAuthenticationCodeSubtitle(codeInfo));
            result.insert("title", detail::getAuthenticationCodeTitle(codeInfo));
            result.insert("length", detail::getAuthenticationCodeLength(codeInfo));

            result.insert("isNextTypeSms", detail::getAuthenticationCodeIsNextTypeSms(codeInfo));
            result.insert("nextTypeString", detail::getAuthenticationCodeNextTypeString(codeInfo));

            result.insert("timeout", codeInfo.value("timeout").toInt());

            emit codeRequested(result);

            emit isAuthorizedChanged();
            break;
        }
        case fnv::hash("authorizationStateWaitPassword"): {
            const auto password = authorizationState.value("password").toMap();

            QVariantMap result;
            result.insert("passwordHint", password.value("password_hint").toString());
            result.insert("hasRecoveryEmailAddress", password.value("has_recovery_email_address").toBool());
            result.insert("recoveryEmailAddressPattern", password.value("recovery_email_address_pattern").toString());

            emit passwordRequested(result);

            emit isAuthorizedChanged();
            break;
        }
        case fnv::hash("authorizationStateWaitRegistration"): {
            const auto termsOfService = authorizationState.value("terms_of_service").toMap();

            QVariantMap result;
            result.insert("text", termsOfService.value("text").toMap().value("text").toString());
            result.insert("minUserAge", termsOfService.value("min_user_age").toInt());
            result.insert("showPopup", termsOfService.value("show_popup").toBool());

            emit registrationRequested(result);

            emit isAuthorizedChanged();
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
        case fnv::hash("authorizationStateLoggingOut"): {
            m_isAuthorized = false;

            emit isAuthorizedChanged();
            break;
        }
        case fnv::hash("authorizationStateClosed"): {
            m_worker.request_stop();
            break;
        }
    }
}
