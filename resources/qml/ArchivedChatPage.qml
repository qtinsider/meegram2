import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        text: Localization.getString("ArchivedChats") + Localization.emptyString
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

    Label {
        anchors.centerIn: listView
        font.pixelSize: 60
        color: "gray"
        text: Localization.getString("NoChats") + Localization.emptyString
        visible: myChatModel.count === 0 && !populateTimer.running && !myChatModel.loading
    }

    BusyIndicator {
        anchors.centerIn: listView
        running: visible
        visible: populateTimer.running || myChatModel.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }

    ChatModel {
        id: myChatModel
        chatList: TdApi.ChatListArchive

        onLoadingChanged: {
            if (!loading)
                populateTimer.restart()
        }
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {
            MenuItem {
                text: Localization.emptyString + myChatModel.get(listView.currentIndex).isPinned ? Localization.getString("UnpinFromTop") : Localization.getString("PinFromTop")
                onClicked: {
                    myChatModel.toggleChatIsPinned(myChatModel.get(listView.currentIndex).id, !myChatModel.get(listView.currentIndex).isPinned)
                    populateTimer.restart()
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

    Timer {
        id: populateTimer

        interval: 200
        repeat: false
        onTriggered: myChatModel.populate()
    }
}
