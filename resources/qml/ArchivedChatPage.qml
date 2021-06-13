import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 0.1
import "components"

Page {
    id: root

    TopBar {
        id: header
        text: "Archived Chats"
        isArchived: true

        MouseArea { anchors.fill: parent }
    }

    ListView {
        id: listView

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: header.bottom
        }

        cacheBuffer: listView.height * 2

        delegate: ChatItem { }

        model: myChatModel
    }

    ScrollDecorator {
        flickableItem: listView
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }

    Component.onDestruction: myChatModel.chatList = TdApi.ChatListMain
}
