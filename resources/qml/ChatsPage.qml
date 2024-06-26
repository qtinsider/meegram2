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
        title: locale.getString("Chats") + locale.emptyString

        Image {
            anchors {
                right: parent.right
                rightMargin: 16
                verticalCenter: parent.verticalCenter
            }
            source: "image://theme/meegotouch-combobox-indicator-inverted"
        }

        onClicked: chatFolderDialog.open()
    }

    Menu {
        id: myMenu
        MenuLayout {
            MenuItem {
                text: locale.getString("ArchivedChats") + locale.emptyString
                onClicked: {
                    pageStack.push(Qt.createComponent("ArchivedChatPage.qml"))
                }
            }
            MenuItem {
                text: locale.getString("SETTINGS") + locale.emptyString
                onClicked: {
                    pageStack.push(Qt.createComponent("SettingsPage.qml"))
                }
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
        snapMode: ListView.SnapToItem
    }

    Label {
        anchors.centerIn: listView
        font.pixelSize: 60
        color: "gray"
        text: locale.getString("NoChats") + locale.emptyString
        visible: myChatModel.count === 0 && !populateTimer.running && !myChatModel.loading
    }

    BusyIndicator {
        anchors.centerIn: listView
        running: visible
        visible: populateTimer.running || myChatModel.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }

    SelectionDialog {
        id: chatFolderDialog
        titleText: locale.getString("Filters") + locale.emptyString
        selectedIndex: 0
        model: mychatFolderModel

        onAccepted: {
            if (model.get(selectedIndex).id === 0) {
                myChatModel.chatList = TdApi.ChatListMain
                return
            }

            myChatModel.chatList = TdApi.ChatListFolder
            myChatModel.chatFolderId = model.get(selectedIndex).id
        }
    }


    ChatFolderModel {
        id: mychatFolderModel
        manager: tdManager
        chatFolders: storageManager.chatFolders
    }

    ChatModel {
        id: myChatModel
        manager: tdManager
        chatList: TdApi.ChatListMain

        onLoadingChanged: {
            if (!loading)
                populateTimer.restart()
        }
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {

            MenuItem {
                text: myChatModel.isPinned(listView.currentIndex) ? locale.getString("UnpinFromTop") + locale.emptyString : locale.getString("PinToTop") + locale.emptyString
                onClicked: {
                    myChatModel.toggleChatIsPinned(listView.currentIndex)
                }
            }

            MenuItem {
                text: myChatModel.isMuted(listView.currentIndex) ? locale.getString("ChatsUnmute") + locale.emptyString : locale.getString("ChatsMute") + locale.emptyString
                onClicked: {
                    myChatModel.toggleChatNotificationSettings(listView.currentIndex)
                }
            }
        }
    }

    Timer {
        id: populateTimer

        interval: 200
        repeat: false
        onTriggered: myChatModel.populate()
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
            anchors.right: (parent === undefined) ? undefined : parent.right
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }


    Component.onCompleted: { myChatModel.refresh() }
}
