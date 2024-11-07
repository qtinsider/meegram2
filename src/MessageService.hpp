#pragma once

#include <td/telegram/td_api.h>

#include "MessageContent.hpp"

#include <QMetaType>
#include <QStringList>


class MessageService : public QObject, public MessageContent
{
    Q_OBJECT

    Q_PROPERTY(QString groupTitle READ groupTitle CONSTANT)
    Q_PROPERTY(QStringList addedMembers READ addedMembers CONSTANT)
    Q_PROPERTY(qlonglong removedMember READ removedMember CONSTANT)
    Q_PROPERTY(qlonglong upgradedToSupergroup READ upgradedToSupergroup CONSTANT)
    Q_PROPERTY(qlonglong upgradedFromGroup READ upgradedFromGroup CONSTANT)
    Q_PROPERTY(qlonglong pinnedMessage READ pinnedMessage CONSTANT)
    Q_PROPERTY(int autoDeleteTime READ autoDeleteTime CONSTANT)
    Q_PROPERTY(QString customAction READ customAction CONSTANT)

public:
    explicit MessageService(td::td_api::object_ptr<td::td_api::MessageContent> content, QObject *parent = nullptr);

    QString groupTitle() const;
    QStringList addedMembers() const;
    qlonglong removedMember() const;
    qlonglong upgradedToSupergroup() const;
    qlonglong upgradedFromGroup() const;
    qlonglong pinnedMessage() const;
    int autoDeleteTime() const;
    QString customAction() const;

private:
    QString m_groupTitle;
    QStringList m_addedMembers;
    qlonglong m_removedMember;
    qlonglong m_upgradedToSupergroup;
    qlonglong m_upgradedFromGroup;
    qlonglong m_pinnedMessage;
    int m_autoDeleteTime;
    QString m_customAction;
};

Q_DECLARE_METATYPE(MessageService *)
