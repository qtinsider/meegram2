#include "MessageService.hpp"

MessageService::MessageService(td::td_api::object_ptr<td::td_api::MessageContent> content, QObject *parent)
    : QObject(parent)
{
    switch (content->get_id())
    {
        case td::td_api::messageBasicGroupChatCreate::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageBasicGroupChatCreate>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            for (const auto &member : message->member_user_ids_)
            {
                m_addedMembers.append(QString::number(member));
            }
            break;
        }
        case td::td_api::messageSupergroupChatCreate::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageSupergroupChatCreate>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            break;
        }
        case td::td_api::messageChatChangeTitle::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatChangeTitle>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            break;
        }
        case td::td_api::messageChatAddMembers::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatAddMembers>(content);
            for (auto member : message->member_user_ids_)
            {
                m_addedMembers.append(QString::number(member));
            }
            break;
        }
        case td::td_api::messageChatDeleteMember::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatDeleteMember>(content);
            m_removedMember = message->user_id_;
            break;
        }
        case td::td_api::messageChatUpgradeTo::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatUpgradeTo>(content);
            m_upgradedToSupergroup = message->supergroup_id_;
            break;
        }
        case td::td_api::messageChatUpgradeFrom::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatUpgradeFrom>(content);
            m_groupTitle = QString::fromStdString(message->title_);
            m_upgradedFromGroup = message->basic_group_id_;
            break;
        }
        case td::td_api::messagePinMessage::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messagePinMessage>(content);
            m_pinnedMessage = message->message_id_;
            break;
        }
        case td::td_api::messageChatSetMessageAutoDeleteTime::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageChatSetMessageAutoDeleteTime>(content);
            m_autoDeleteTime = message->message_auto_delete_time_;
            break;
        }
        case td::td_api::messageCustomServiceAction::ID: {
            auto message = td::td_api::move_object_as<td::td_api::messageCustomServiceAction>(content);
            m_customAction = QString::fromStdString(message->text_);
            break;
        }
        default: {
            break;
        }
    }
}

QString MessageService::groupTitle() const
{
    return m_groupTitle;
}

QStringList MessageService::addedMembers() const
{
    return m_addedMembers;
}

qlonglong MessageService::removedMember() const
{
    return m_removedMember;
}

qlonglong MessageService::upgradedToSupergroup() const
{
    return m_upgradedToSupergroup;
}

qlonglong MessageService::upgradedFromGroup() const
{
    return m_upgradedFromGroup;
}

qlonglong MessageService::pinnedMessage() const
{
    return m_pinnedMessage;
}

int MessageService::autoDeleteTime() const
{
    return m_autoDeleteTime;
}

QString MessageService::customAction() const
{
    return m_customAction;
}
