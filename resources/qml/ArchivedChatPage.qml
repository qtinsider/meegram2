import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
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

        delegate: ChatItem {
            onPressAndHold: {
                listView.currentIndex = -1;
                listView.currentIndex = index;
                contextMenu.open();
            }
        }

        model: myChatModel
    }

    ChatModel {
        id: myChatModel
        chatList: TdApi.ChatListArchive

        Component.onCompleted: populate()
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {
            MenuItem {
                text: myChatModel.get(listView.currentIndex).isPinned ? qsTr("UnpinFromTop") : qsTr("PinFromTop")
                onClicked: {
                    myChatModel.toggleChatIsPinned(myChatModel.get(listView.currentIndex).id, !myChatModel.get(listView.currentIndex).isPinned)
                    myChatModel.populate()
                }
            }
        }
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
}
