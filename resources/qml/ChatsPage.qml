import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    property variant chatPosition: null

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
                var chat = app.getChat(model.id)
                chatPosition = myChatModel.getChatPosition(chat, chatList);
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
            source: "qrc:/tgs/Newborn.tgs"
            loop: 34
            onStatusChanged: {
                if (status === LottieAnimation.Ready)
                    lottieAnimation.play();
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
        model: ListModel { id: insertFolderModel }

        onAccepted: {
            if (model.get(selectedIndex).id === 0) {
                chatList.type = ChatList.Main;
            } else {
                chatList.type = ChatList.Folder;
                chatList.folderId = model.get(selectedIndex).id;
            }
        }

        Component.onCompleted: {
            insertFolderModel.append({ id: 0, name: qsTr("FilterAllChats") });

            for (var i = 0; i < mychatFolderModel.count; i++) {
                insertFolderModel.append(mychatFolderModel.get(i));
            }
        }
    }

    ChatFolderModel {
        id: mychatFolderModel
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
                text: chatPosition && chatPosition.isPinned
                      ? qsTr("UnpinFromTop")
                      : qsTr("PinToTop")
                onClicked: {
                    if (chatPosition)
                        myChatModel.toggleChatIsPinned(chatPosition.id, !chatPosition.isPinned)
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

    Connections {
        target: null // Replace with StorageManager
        onChatFoldersUpdated: {
            insertFolderModel.clear();
            insertFolderModel.append({ id: 0, name: qsTr("FilterAllChats") });

            for (var i = 0; i < mychatFolderModel.count; i++) {
                insertFolderModel.append(mychatFolderModel.get(i));
            }
        }
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
