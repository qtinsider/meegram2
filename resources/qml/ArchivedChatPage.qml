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
        title: app.getString("ArchivedChats") + app.emptyString
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

        model: chatModel
    }

    Label {
        anchors.centerIn: listView
        font.pixelSize: 60
        color: "gray"
        text: app.getString("NoChats") + app.emptyString
        visible: chatModel.count === 0 && !populateTimer.running && !chatModel.loading
    }

    BusyIndicator {
        anchors.centerIn: listView
        running: visible
        visible: populateTimer.running || chatModel.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }

    Connections {
        target: chatModel

        onLoadingChanged: {
            if (!chatModel.loading)
                populateTimer.restart()
        }
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {
            MenuItem {
                text: chatModel.isPinned(listView.currentIndex)
                      ? app.getString("UnpinFromTop") + app.emptyString
                      : app.getString("PinToTop") + app.emptyString
                onClicked: {
                    chatModel.toggleChatIsPinned(listView.currentIndex)
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
        onTriggered: chatModel.populate()
    }

    Component.onCompleted: {
        chatModel.chatList = TdApi.ChatListArchive
        chatModel.refresh()
    }

    Component.onDestruction: {
        chatModel.chatList = TdApi.ChatListMain
        chatModel.refresh()
    }
}
