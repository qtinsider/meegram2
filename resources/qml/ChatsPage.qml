import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    property variant currentChat: null

    TopBar {
        id: header
        title: "MeeGram"

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
                text: qsTr("ArchivedChats")
                onClicked: pageStack.push(Qt.createComponent("ArchivedChatPage.qml"))
            }
            MenuItem {
                text: qsTr("SETTINGS")
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
                currentChat = myChatModel.get(index);
                contextMenu.open();
            }
        }

        model: myChatModel
        snapMode: ListView.SnapToItem
    }

    Column {
        anchors.centerIn: listView
        anchors.margins: UiConstants.DefaultMargin * 2
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

        width: parent.width

        visible: myChatModel.count === 0 && !populateTimer.running && !myChatModel.loading

        LottieAnimation {
            id: lottieAnimation
            anchors.horizontalCenter: parent.horizontalCenter
            width: 160
            height: 160
            source: "qrc:/tgs/Newborn.json"
            onStatusChanged: {
                if (status === LottieAnimation.Ready)
                    lottieAnimation.play();
            }

            MouseArea {
                anchors.fill: parent
                onClicked: lottieAnimation.play();
            }
        }

        Label {
            text: qsTr("NoChats")
            wrapMode: Text.WordWrap
            color: "gray"
            font.pixelSize: 42
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }
    }

    BusyIndicator {
        anchors.centerIn: listView
        running: visible
        visible: populateTimer.running || myChatModel.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }

    SelectionDialog {
        id: chatFolderDialog
        titleText: qsTr("Filters")
        selectedIndex: 0
        model: mychatFolderModel

        onAccepted: {
            if (model.get(selectedIndex).id === 0) {
                chatList.type = ChatList.Main;
            } else {
                chatList.type = ChatList.Folder;
                chatList.folderId = model.get(selectedIndex).id;
            }
        }
    }

    ChatFolderModel {
        id: mychatFolderModel
        // localeString: qsTr("FilterAllChats")
    }

    ChatModel {
        id: myChatModel
        list: chatList

        onLoadingChanged: {
            if (!loading)
                populateTimer.restart()
        }
    }

    ChatList {
        id: chatList
        type: ChatList.Main
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {
            MenuItem {
                text: currentChat && currentChat.isPinned
                      ? qsTr("UnpinFromTop")
                      : qsTr("PinToTop")
                onClicked: {
                    if (currentChat)
                        myChatModel.toggleChatIsPinned(currentChat.id, !currentChat.isPinned)
                }
            }

            MenuItem {
                text: currentChat && currentChat.isMuted
                      ? qsTr("ChatsUnmute")
                      : qsTr("ChatsMute")
                onClicked: {
                    if (currentChat)
                        myChatModel.toggleChatNotificationSettings(currentChat.id, !currentChat.isMuted)
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
            anchors.right: (parent !== undefined) ? parent.right : undefined
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }

    Component.onCompleted: { myChatModel.refresh() }
}
