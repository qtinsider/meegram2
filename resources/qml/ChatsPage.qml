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
        title: app.getString("Chats") + app.emptyString

        Image {
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: 16
            }
            source: "image://theme/meegotouch-combobox-indicator-inverted"
        }

         onClicked: chatFolderDialog.open()
    }

    Menu {
        id: myMenu
        MenuLayout {
            MenuItem {
                text: app.getString("ArchivedChats") + app.emptyString
                onClicked: pageStack.push(Qt.createComponent("ArchivedChatPage.qml"), { locale: app.locale, storage: app.storageManager })
            }
            MenuItem {
                text: app.getString("SETTINGS") + app.emptyString
                onClicked: pageStack.push(Qt.createComponent("SettingsPage.qml"))
            }
            MenuItem {
                text: "About"
                onClicked: aboutDialog.open()
            }
        }
    }

    ListView {
        id: listView

        anchors {
            top: header.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        cacheBuffer: listView.height * 2

        delegate: ChatItem {
            onPressAndHold: {
                listView.currentIndex = index;
                contextMenu.open();
            }
        }

        model: chatModel
        snapMode: ListView.SnapToItem
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

    SelectionDialog {
        id: chatFolderDialog
        titleText: app.getString("Filters") + app.emptyString
        selectedIndex: 0
        model: chatFolderModel

        onAccepted: {
            if (model.get(selectedIndex).id === 0) {
                chatModel.chatList = TdApi.ChatListMain;
            } else {
                chatModel.chatList = TdApi.ChatListFolder;
                chatModel.chatFolderId = model.get(selectedIndex).id;
            }
        }
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
                onClicked: chatModel.toggleChatIsPinned(listView.currentIndex)
            }

            MenuItem {
                text: chatModel.isMuted(listView.currentIndex)
                    ? app.getString("ChatsUnmute") + app.emptyString
                    : app.getString("ChatsMute") + app.emptyString
                onClicked: chatModel.toggleChatNotificationSettings(listView.currentIndex)
            }
        }
    }

    Timer {
        id: populateTimer
        interval: 200
        repeat: false
        onTriggered: chatModel.populate()
    }

    ScrollDecorator {
        flickableItem: listView
    }

    AboutDialog {
        id: aboutDialog
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-view-menu"
            anchors.right: (parent !== undefined) ? parent.right : undefined
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }

    Component.onCompleted: {
        chatModel.chatList = TdApi.ChatListMain

        chatModel.refresh()
    }
}
