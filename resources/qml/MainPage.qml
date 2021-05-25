import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    property int currentChatIndex: -1

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
                    myChatModel.chatList = TdApi.ChatListArchive
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

        delegate: Item {
            height: 88; width: parent.width
            
            BorderImage {
                id: background
                anchors.fill: parent
                visible: mouseArea.pressed
                source: "image://theme/meegotouch-panel-background-pressed"
            }

            Image {
                id: photo

                width: 64
                height: 64
                anchors {
                    left: parent.left
                    leftMargin: 16
                    verticalCenter: parent.verticalCenter
                }
                source: model.photo ? "image://telegram/" + model.photo : "image://theme/icon-l-content-avatar-placeholder"
            }

            Item {
                id: row1

                width: parent.width - photo.width - 44
                height: 45
                anchors {
                    left: photo.right
                    leftMargin: 16
                    rightMargin: 16
                }

                Label {
                    id: title
                    width: parent.width - date.width
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                    font.pixelSize: 26
                    color: mouseArea.pressed ? "#797979" : "#282828"
                    elide: Text.ElideRight
                    text: model.title
                }

                Label {
                    id: date

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    font.weight: Font.Light
                    font.pixelSize: 20
                    color: mouseArea.pressed ? "#797979" : "#505050"
                    text: model.lastMessageDate
                }
            }

            Item {
                height: 30
                width: parent.width - photo.width - 44
                anchors { left: row1.left; top: row1.bottom }
                
                Label {
                    id: lastMessage
                    width: parent.width - componentLoader.width
                    anchors.verticalCenter: parent.verticalCenter
                    font.weight: Font.Light
                    font.pixelSize: 22
                    color: mouseArea.pressed ? "#797979" : "#505050"
                    elide: Text.ElideRight
                    text: model.lastMessageContent
                }

                Loader {
                    id: componentLoader

                    anchors { verticalCenter: parent.verticalCenter; right: parent.right }

                    sourceComponent: model.unreadCount ? countBubble : moreIndicator
                }
                
                Component {
                    id: countBubble

                    CountBubble {
                        largeSized: true
                        value: model.unreadCount
                    }
                }

                Component {
                    id: moreIndicator

                    MoreIndicator {}
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent

                onClicked: {
                    pageStack.push(Qt.createComponent("MessagePage.qml"), { chat: ChatStore.get(model.id) })
                }
                onPressAndHold: contextMenu.open()
            }
        }

        model: myChatModel

        visible: isAuthorized
    }

    Label {
        anchors.centerIn: listView
        font.pixelSize: 60
        color: "gray"
        text: qsTr("NoChats")
        visible: myChatModel.count === 0 && isAuthorized && !myChatModel.loading
    }

    BusyIndicator  {
        id: busyIndicator
        anchors.centerIn: listView
        running: true
        platformStyle: BusyIndicatorStyle { size: "large" }
        visible: myChatModel.loading && isAuthorized
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

        onAccepted: {
            if (model.get(selectedIndex).value === 1) {
                myChatModel.chatList = TdApi.ChatListMain
                return
            }

            myChatModel.chatFilterId = model.get(selectedIndex).value
        }
    }

    Connections {
        target: tdapi

        onAuthorizationStateReady: myChatModel.loadChats()
        onUpdateChatFilters: {
            selectionDialog.model.clear();

            selectionDialog.model.append({name: qsTr("FilterAllChats"), value: 1});

            if (chatFilters === null)
                return;

            for (var i in chatFilters) {
                selectionDialog.model.append({name: chatFilters[i].title, value: chatFilters[i].id})
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
    }

    ContextMenu {
        id: contextMenu

        content: MenuLayout {
            MenuItem {
                text: qsTr("PinToTop") // or UnpinFromTop
                onClicked: {}
            }
            MenuItem {
                text: qsTr("MarkAsRead") // or MarkAsUnread
                onClicked: {}
            }
            MenuItem {
                text: qsTr("DeleteChat")
                onClicked: {}
            }
        }
    }

    SelectionDialog {
        id: addDialog
        titleText: qsTr("Create")
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
            iconSource: "images/help-icon.png"

            onClicked: aboutDialog.open()
        }

        ToolIcon {
            visible: isAuthorized
            platformIconId: "toolbar-add"
            onClicked: addDialog.open()
        }


        ToolIcon {
            visible: isAuthorized
            platformIconId: "toolbar-view-menu"
            anchors.right: (parent === undefined) ? undefined : parent.right
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }

    }
}
