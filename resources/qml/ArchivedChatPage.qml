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
        title: app.locale.getString("ArchivedChats") + app.locale.emptyString
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
        text: app.locale.getString("NoChats") + app.locale.emptyString
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

        locale: app.locale
        storageManager: app.storageManager

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
                      ? app.locale.getString("UnpinFromTop") + app.locale.emptyString
                      : app.locale.getString("PinToTop") + app.locale.emptyString
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
