import QtQuick 1.1
import com.nokia.meego 1.1
import "components"

Page {
    id: root

    property variant chat: null

    property string messageThreadId: ""
    property string replyToMessageId: ""
    property variant options: null
    property variant replyMarkup: null

    Image {
        id: topBar

        height: screen.currentOrientation === Screen.Portrait ? 72 : 64
        width: parent.width
        source: "image://theme/meegotouch-view-header-fixed"
        z: 1

        Item {
            anchors.fill: parent

            Button {
                id: button

                anchors {
                    left: parent.left
                    leftMargin: 16
                    verticalCenter: parent.verticalCenter
                }

                platformStyle: ButtonStyle {
                    background: "image://theme/meegotouch-sheet-button-background"
                    pressedBackground: "image://theme/meegotouch-sheet-button-background-pressed"

                    buttonWidth: 100
                    buttonHeight: 42
                    backgroundMarginTop: 20
                    backgroundMarginBottom: 20
                    textColor: "white"
                }

                Label {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        horizontalCenter: parent.horizontalCenter
                    }

                    font.pixelSize: 22; font.bold: true
                    text: "Chats"
                }

                onClicked: pageStack.pop()

            }

            Column {
                id: column

                anchors {
                    left: button.right
                    right: photo.left
                    leftMargin: 16
                    rightMargin: 16
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    text: Utils.getChatTitle(chat)
                    font.bold: true
                    width: parent.width
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                }

                Label {
                    text: Utils.getChatSubtitle(chat)
                    font { weight: Font.Light; pixelSize: 20 }
                    width: parent.width
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignTop
                    elide: Text.ElideRight
                }

            }

            Image {
                id: photo

                height: 50; width: 50
                anchors {
                    right: parent.right
                    rightMargin: 16
                    verticalCenter: parent.verticalCenter
                }

                source: chat.photo ? "image://telegram/" + chat.photo.small.local.path : "image://theme/icon-l-content-avatar-placeholder"

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        myMessageModel.loadHistory()

                    }
                }
            }
        }
    }

    Connections {
        target: myMessageModel


    }

    Rectangle {
        id: contentArea

        width: parent.width
        height: parent.height - topBar.height - inputButtonHolder.height
        anchors { top: parent.top; topMargin: topBar.height }

        clip: true
        color: "transparent"

        ListView {
            id: listView

            height: parent.height - myTextArea.height
            width: parent.width
            anchors.top: parent.top

            spacing: 6

            cacheBuffer: listView.height * 2

            delegate: MessageDelegate {
                sender: !model.isOutgoing ? model.sender : ""
                content: model.content
                date: Qt.formatDateTime(model.date, "dd MMM yyyy | h:mm A")
                isOutgoing: model.isOutgoing
                isServiceMessage: model.isServiceMessage
                serviceMessage: model.serviceMessage

                onClicked: console.log(ListView.isCurrentItem)
                onPressAndHold: bubbleMenu.open()
            }

            model: myMessageModel

            header: Item {
                width: listView.width
                height: Math.max(0, contentArea.height - listView.contentHeight - myTextArea.height)
            }

            footer: Item { height: 10 }

            ScrollDecorator {
                flickableItem: listView
            }

            Connections {
                target: inputContext
                onSoftwareInputPanelVisibleChanged: {
                    if (inputContext.softwareInputPanelVisible) {
                        listView.positionViewAtEnd();
                    }
                }
            }
        }

        BusyIndicator  {
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }
            running: true
            platformStyle: BusyIndicatorStyle { size: "large" }
            visible: myMessageModel.count === 0
        }

        TextArea {
            id: myTextArea

            height: 64
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }

            placeholderText: "Write your message here"

            platformStyle: TextAreaStyle {
                background: "images/messaging-textedit-background.png"
                backgroundError: "images/messaging-textedit-background.png"
                backgroundDisabled: "images/messaging-textedit-background.png"
                backgroundSelected: "images/messaging-textedit-background.png"
                backgroundCornerMargin: 1
            }
        }
    }

    Rectangle {
        id: inputButtonHolder

        width: parent.width
        height: myTextArea.activeFocus ? 64 : 0
        anchors { bottom: parent.bottom; left: parent.left }

        clip: true

        color: "white"

        Rectangle {
            anchors.top: parent.top
            height: 1; width: parent.width
            opacity: 0.4
            color: "#cccccc"
        }

        Button {
            id: sendButton

            height: 48; width: 120
            anchors { right: parent.right; bottom: parent.bottom; margins: 8 }

            platformStyle: ButtonStyle { inverted: true }
            
            text: "Send"

            onClicked: {
                internal.sendMessage(myTextArea.text)
                myTextArea.text = ""
            }
        }

        MouseArea {
            id: inputHolderArea
            anchors.fill: parent
            enabled: false
            onClicked: myTextArea.forceActiveFocus()
        }
    }

    ContextMenu {
        id: bubbleMenu

        content: MenuLayout {
            MenuItem {
                text: "Reply"
                onClicked: {}
            }
            MenuItem {
                text: "Edit"
                onClicked: {}
            }
            MenuItem {
                text: "Copy"
                onClicked: {}
            }
            MenuItem {
                text: "Delete"
                onClicked: {}
            }
        }
    }

    QtObject {
        id: internal

        function sendMessage(message) {
            var inputMessageContent = {
                '@type': "inputMessageText",
                text: {
                    '@type': "formattedText",
                    text: message
                }
            }
            tdapi.sendMessage(chat.id, messageThreadId, replyToMessageId, options, replyMarkup, inputMessageContent)
        }
    }

    Component.onCompleted: myMessageModel.openChat(chat.id)
    Component.onDestruction: myMessageModel.closeChat(chat.id)
}
