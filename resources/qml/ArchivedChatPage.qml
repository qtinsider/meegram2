import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: "ArchivedChats"
        isArchived: true
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
                listView.currentIndex = index
                contextMenu.open()
            }
        }

        model: myChatModel
    }

    Label {
        anchors.centerIn: listView
        font.pixelSize: 60
        color: "gray"
        text: app.getString("NoChats") + app.emptyString
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
                text: myChatModel.isPinned(listView.currentIndex)
                      ? app.getString("UnpinFromTop") + app.emptyString
                      : app.getString("PinToTop") + app.emptyString
                onClicked: {
                    myChatModel.toggleChatIsPinned(listView.currentIndex)
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

    Component.onCompleted: { myChatModel.refresh() }
}
