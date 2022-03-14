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
        text: Localization.getString("Chats") + Localization.emptyString

        Image {
            anchors {
                right: parent.right
                rightMargin: 16
                verticalCenter: parent.verticalCenter
            }
            source: "image://theme/meegotouch-combobox-indicator-inverted"
        }

        MouseArea {
            anchors.fill: parent
            onClicked: chatFilterDialog.open()
        }
    }

    Menu {
        id: myMenu
        MenuLayout {
            MenuItem {
                text: Localization.getString("ArchivedChats") + Localization.emptyString
                onClicked: {
                    pageStack.push(Qt.createComponent("ArchivedChatPage.qml"))
                }
            }
            MenuItem {
                text: Localization.getString("SETTINGS") + Localization.emptyString
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
                listView.currentIndex = -1;
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
        text: Localization.getString("NoChats") + Localization.emptyString
        visible: myChatModel.count === 0 && !populateTimer.running && !myChatModel.loading
    }

    BusyIndicator {
        anchors.centerIn: listView
        running: visible
        visible: populateTimer.running || myChatModel.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }

    SelectionDialog {
        id: chatFilterDialog
        titleText: Localization.getString("Filters") + Localization.emptyString
        selectedIndex: 0
        model: ChatFilterModel { id: chatFilterModel }

        onAccepted: {
            if (model.get(selectedIndex).id === 0) {
                myChatModel.chatList = TdApi.ChatListMain
                return
            }

            myChatModel.chatList = TdApi.ChatListFilter
            myChatModel.chatFilterId = model.get(selectedIndex).id
        }
    }

    ChatModel {
        id: myChatModel
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
                text: myChatModel.get(listView.currentIndex).isPinned ? Localization.getString("UnpinFromTop") + Localization.emptyString : Localization.getString("PinToTop") + Localization.emptyString
                onClicked: {
                    myChatModel.toggleChatIsPinned(myChatModel.get(listView.currentIndex).id, !myChatModel.get(listView.currentIndex).isPinned)
                    populateTimer.restart()
                }
            }

            MenuItem {
                text: myChatModel.get(listView.currentIndex).isMuted ? Localization.getString("ChatsUnmute") + Localization.emptyString : Localization.getString("ChatsMute") + Localization.emptyString
                onClicked: {
                    myChatModel.toggleChatNotificationSettings(myChatModel.get(listView.currentIndex).id, !myChatModel.get(listView.currentIndex).isMuted);
                    populateTimer.restart()
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

    Component.onCompleted: myChatModel.refresh()
}
