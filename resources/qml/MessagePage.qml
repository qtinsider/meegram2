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

    Flickable {
        id: contentArea

        contentHeight: parent.height
        contentWidth: parent.width

        anchors.fill: parent

        Column {
            id: contentColumn

            width: parent.width
            height: parent.height

            Image {
                id: topBar

                height: isPortrait ? 72 : 64
                width: parent.width
                z: 1

                source: "image://theme/meegotouch-view-header-fixed"

                Row {
                    id: backButtonRow

                    anchors { left: parent.left; leftMargin: 12 }

                    height: parent.height

                    Button {
                        anchors.verticalCenter: parent.verticalCenter

                        platformStyle: ButtonStyle {
                            background: "image://theme/meegotouch-sheet-button-background"
                            pressedBackground: "image://theme/meegotouch-sheet-button-background-pressed"

                            buttonWidth: 100
                            buttonHeight: 42
                        }

                        text: "Chats"

                        onClicked: pageStack.pop()
                    }
                }

                Row {
                    anchors {
                        left: backButtonRow.right
                        leftMargin: 70
                        right: parent.right
                        rightMargin: 12
                    }

                    height: parent.height
                    width: parent.width

                    spacing: 12
                    layoutDirection: Qt.RightToLeft

                    Image {
                        id: photo

                        anchors.verticalCenter: parent.verticalCenter
                        height: 50
                        width: 50

                        source: chat.photo ? "image://telegram/" + chat.photo.small.local.path : "image://theme/icon-l-content-avatar-placeholder"

                        MouseArea {
                            anchors.fill: parent
                        }
                    }

                    Column {
                        anchors.verticalCenter: parent.verticalCenter

                        width: parent.width

                        Label {
                            text: myMessageModel.chatTitle
                            font.bold: true
                            width: parent.width
                            horizontalAlignment: Text.AlignRight
                            elide: Text.ElideRight
                        }

                        Label {
                            text: myMessageModel.chatSubtitle
                            font { weight: Font.Light; pixelSize: 20 }
                            width: parent.width
                            horizontalAlignment: Text.AlignRight
                            elide: Text.ElideRight
                        }
                    }
                }
            }

            Item {
                id: item

                height: parent.height - topBar.height - inputPanelHolder.height
                width: parent.width

                ListView {
                    id: listView

                    anchors.fill: parent
                    clip: true

                    spacing: 6

                    cacheBuffer: listView.height * 2

                    delegate: Component {
                        Loader {
                            id: loader

                            width: parent.width
                            height: childrenRect.height
                            sourceComponent: model.isServiceMessage ? serviceMessageComponent : deleglateChooser.get(model.content)

                            Component {
                                id: serviceMessageComponent

                                Item {
                                    id: serviceMessageItem

                                    height: serviceMessageLabel.height + 28
                                    width: parent.width

                                    BorderImage {
                                        anchors.centerIn: parent

                                        height: parent.height
                                        width: serviceMessageLabel.paintedWidth + 24

                                        source: "qrc:/images/notification.png"

                                        border { left: 22; right: 22; bottom: 22; top: 22; }
                                    }

                                    Label {
                                        id: serviceMessageLabel

                                        anchors { top: parent.top; topMargin: 14 }

                                        width: parent.width

                                        font {
                                            pixelSize: 18
                                            weight: Font.Light
                                        }

                                        horizontalAlignment: Text.AlignHCenter
                                        wrapMode: Text.Wrap

                                        color: "gray"

                                        text: model.serviceMessage
                                    }
                                }
                            }

                            Component {
                                id: textMessageComponent

                                MessageBubble {

                                    childrenWidth: messageText.paintedWidth

                                    content: Text {
                                        id: messageText

                                        anchors {
                                            left: parent.left
                                            leftMargin: model.isOutgoing ? 20 : 80
                                        }

                                        width: isPortrait ? 380 : 754

                                        font {
                                            pixelSize: 23
                                            weight: Font.Light
                                        }

                                        wrapMode: Text.Wrap
                                        horizontalAlignment: model.isOutgoing ? Text.AlignLeft : Text.AlignRight

                                        color: model.isOutgoing ? "black" : "white"

                                        text: Utils.getFormattedText(model.content.text)

                                        onLinkActivated: Qt.openUrlExternally(link)
                                    }
                                }
                            }

                            Component {
                                id: notSupportedMessageComponent

                                MessageBubble {

                                    childrenWidth: notSupportedMessage.paintedWidth

                                    content: Label {
                                        id: notSupportedMessage

                                        anchors {
                                            left: parent.left
                                            leftMargin: model.isOutgoing ? 20 : 80
                                        }

                                        width: isPortrait ? 380 : 754

                                        font {
                                            bold: true
                                            pixelSize: 23
                                        }

                                        horizontalAlignment: model.isOutgoing ? Text.AlignLeft : Text.AlignRight
                                        wrapMode: Text.Wrap

                                        color: model.isOutgoing ? "black" : "white"

                                        text: "The message is not supported on MeeGram yet"
                                    }
                                }
                            }

                            QtObject {
                                id: deleglateChooser

                                function get(content) {
                                    switch (content['@type']) {
                                    case 'messageText':
                                        return textMessageComponent;

                                    default:
                                        return notSupportedMessageComponent;
                                    }
                                }
                            }
                        }
                    }

                    model: myMessageModel

                    footer: Item { height: 10 }

                    header: Item {
                        width: listView.width
                        height: Math.max(0, item.height - listView.contentHeight)
                    }

                    onAtYBeginningChanged: if (atYBeginning) myMessageModel.loadHistory()

                    Connections {
                        target: inputContext
                        onSoftwareInputPanelVisibleChanged: {
                            if (inputContext.softwareInputPanelVisible) {
                                listView.positionViewAtEnd();
                            }
                        }
                    }

                    ScrollDecorator {
                        flickableItem: listView
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter

                    width: parent.width
                    height: busyIndicator.height

                    visible: myMessageModel.count === 0

                    BusyIndicator  {
                        id: busyIndicator

                        anchors.horizontalCenter: parent.horizontalCenter
                        running: true
                        platformStyle: BusyIndicatorStyle { size: "large" }
                    }
                }
            }

            Column {
                id: inputPanelHolder

                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width

                TextArea {
                    id: textArea

                    height: 64
                    width: parent.width

                    placeholderText: "Write your message here"

                    platformStyle: TextAreaStyle {
                        background: "qrc:/images/messaging-textedit-background.png"
                        backgroundError: "qrc:/images/messaging-textedit-background.png"
                        backgroundDisabled: "qrc:/images/messaging-textedit-background.png"
                        backgroundSelected: "qrc:/images/messaging-textedit-background.png"
                        backgroundCornerMargin: 1
                    }
                }

                Rectangle {
                    id: controls

                    anchors {
                        left: parent.left
                        right: parent.right
                    }

                    height: 0

                    clip: true

                    color: "white"

                    Rectangle {
                        anchors.left: parent.left
                        anchors.leftMargin: 16
                        anchors.right: parent.right
                        anchors.rightMargin: 16
                        height: 1
                        opacity: 0.5
                        color: "#cccccc"
                    }

                    Button {
                        id: sendButton
                        anchors {
                            right: parent.right
                            rightMargin: 16
                            verticalCenter: parent.verticalCenter
                        }

                        height: 48
                        width: 120

                        platformStyle: ButtonStyle { inverted: true }

                        text: "Send"

                        onClicked: {
                            internal.sendMessage(textArea.text)
                            textArea.text = ""
                        }
                    }
                }

                states: [
                    State {
                        name: "open"
                        when: textArea.activeFocus
                        PropertyChanges { target: controls; height: 64 }
                    }
                ]

                transitions: [
                    Transition {
                        from: "open"; to: "*"
                        ParallelAnimation {
                            PropertyAnimation { target: controls; duration: 300; properties: "height"; easing.type: Easing.InOutQuart }
                        }
                    },
                    Transition {
                        from: "*"; to: "open"
                        ParallelAnimation {
                            PropertyAnimation { target: controls; duration: 300; properties: "height"; easing.type: Easing.OutQuart }
                        }
                    }
                ]
            }
        }
    }

    Timer {
        id: positioner
        interval: 100
        onTriggered: {
            if (!listView.moving)
                listView.positionViewAtEnd()
        }
    }

    Connections {
        target: myMessageModel

        onMoreHistoriesLoaded: {
            listView.positionViewAtIndex(modelIndex - 1, ListView.Beginning)
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

    Component.onCompleted: myMessageModel.openChat(chat)
    Component.onDestruction: myMessageModel.closeChat()
}
