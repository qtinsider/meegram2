import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    TopBar {
        id: header
        text: isAuthorized ? "Chats" : "MeeGram"

        Image {
            anchors {
                right: parent.right
                rightMargin: 16
                verticalCenter: parent.verticalCenter
            }
            source: "image://theme/meegotouch-combobox-indicator-inverted"
            visible: isAuthorized
        }

        MouseArea {
            anchors.fill: parent
            onClicked: isAuthorized ? selectionDialog.open() : null
        }
    }

    Menu {
        id: myMenu
        MenuLayout {
            MenuItem {
                text: "Archived Chats"
                onClicked: {
                    pageStack.push(Qt.createComponent("ArchivedChatPage.qml"))
                }
            }
            MenuItem {
                text: "Settings"
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
        visible: isAuthorized
    }

    Label {
        anchors.centerIn: listView
        font.pixelSize: 60
        color: "gray"
        text: qsTr("NoChats")
        visible: myChatModel.count === 0 && !populateTimer.running && isAuthorized
    }

    BusyIndicator {
        anchors.centerIn: listView
        running: populateTimer.running
        visible: populateTimer.running
        platformStyle: BusyIndicatorStyle { size: "large" }
    }

    Item {
        id: signInInfo

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        clip: true
        visible: !isAuthorized

        Column {
            id: signInInfoColumn

            anchors{
                top: parent.top
                topMargin: 30
                left: parent.left
                right: parent.right
            }

            spacing: 16

            Text {
                anchors{
                    left: parent.left
                    right: parent.right
                    leftMargin: 24
                    rightMargin: 24
                }

                text: "Different, Handy, Powerful"
                wrapMode: Text.WordWrap

                font.pixelSize: 30
                color: "#777777"

                horizontalAlignment: Text.AlignHCenter
            }

            Column {
                anchors.horizontalCenter: parent.horizontalCenter

                spacing: 16

                Button {
                    text: qsTr("StartMessaging")

                    platformStyle: ButtonStyle { inverted: true }

                    onClicked: {
                        var component = Qt.createComponent("AuthenticationPage.qml")
                        if (component.status === Component.Ready) {
                            var dialog = component.createObject(root);
                            dialog.open();
                        }
                    }
                }
            }
        }
    }

    SelectionDialog {
        id: selectionDialog
        titleText: qsTr("Filters")
        selectedIndex: 0
        model: ChatFilterModel {}

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

        onChatListChanged: { populateTimer.restart() }
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {

            MenuItem {
                text: myChatModel.get(listView.currentIndex).isPinned ? qsTr("UnpinFromTop") : qsTr("PinFromTop")
                onClicked: {
                    myChatModel.toggleChatIsPinned(myChatModel.get(listView.currentIndex).id, !myChatModel.get(listView.currentIndex).isPinned)
                    populateTimer.restart()
                }
            }
        }
    }

    Connections {
        target: tdapi

        onAuthorizationStateReady: populateTimer.restart()
    }

    Timer {
        id: populateTimer

        interval: 200
        repeat: false
        onTriggered: { myChatModel.populate() }
    }

    ScrollDecorator {
        flickableItem: listView
    }

    AboutDialog {
        id: aboutDialog
    }

    tools: ToolBarLayout {
        id: commonTools

        ToolIcon {
            visible: !isAuthorized
            platformIconId: "toolbar-back"
            onClicked: Qt.quit()
        }

        ToolIcon {
            visible: !isAuthorized
            anchors.right: parent.right
            iconSource: "qrc:/images/help-icon.png"

            onClicked: aboutDialog.open()
        }

        ToolIcon {
            visible: isAuthorized
            platformIconId: "toolbar-view-menu"
            anchors.right: (parent === undefined) ? undefined : parent.right
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }
}
