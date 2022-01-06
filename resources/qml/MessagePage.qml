import QtQuick 1.1
import com.nokia.meego 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    property string chatId: ""
    property bool loading: true

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

                        source: myMessageModel.chat.photo ? "image://chatPhoto/" + myMessageModel.chat.photo.small.local.path : "image://theme/icon-l-content-avatar-placeholder"

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
                            sourceComponent: model.isServiceMessage ? textMessageComponent : deleglateChooser.get(model.content)

                            Component {
                                id: textMessageComponent

                                MessageBubble {

                                    childrenWidth: messageText.paintedWidth

                                    content: Text {
                                        id: messageText
                                        text:  model.isServiceMessage ? model.serviceMessage.trim() : Utils.getFormattedText(model.content.text)

                                        color: model.isServiceMessage ? "gray" : model.isOutgoing ? "black" : "white"
                                        width: isPortrait ? 380 : 754
                                        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                        anchors {
                                            left: parent.left
                                            leftMargin: model.isServiceMessage ? 50 : model.isOutgoing ? 20 : 80
                                        }
                                        font.pixelSize: model.isServiceMessage ? 18 : 23
                                        horizontalAlignment: model.isServiceMessage ? Text.AlignHCenter : model.isOutgoing ? Text.AlignLeft : Text.AlignRight
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

                    section {
                        delegate: Label {
                            id: sectionLabel
                            wrapMode: Text.WordWrap
                            color: "black"
                            font.pixelSize: 18
                            font.bold: true
                            text: section
                            anchors.horizontalCenter: parent.horizontalCenter
                            anchors.topMargin: 12
                            anchors.bottomMargin: 12
                        }

                        property: "section"
                    }

                    footer: Item { height: 10 }

                    header: Item {
                        width: listView.width
                        height: Math.max(0, item.height - listView.contentHeight)
                    }

                    onCountChanged: {
                        if (!myMessageModel.loading && loading) {
                            if (myMessageModel.chat.unread_count > 0)
                                listView.positionViewAtIndex(myMessageModel.getLastMessageIndex(), ListView.Center)
                            else
                                listView.positionViewAtEnd();

                            loading = false
                        }
                    }

                    onAtYBeginningChanged: {
                        if (atYBeginning && !loading)
                            myMessageModel.loadHistory()
                    }

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
                            myMessageModel.sendMessage(textArea.text)
                            textArea.text = ""
                        }
                    }

                    states: [
                        State {
                            name: "open"
                            when: textArea.activeFocus
                            PropertyChanges { target: controls; height: 64 }
                        }
                    ]
                }
            }
        }
    }

    MessageModel {
        id: myMessageModel

        onMoreHistoriesLoaded: {
            listView.positionViewAtIndex(modelIndex - 1, ListView.Beginning)
        }
    }

    Component.onCompleted: myMessageModel.openChat(chatId)
    Component.onDestruction: myMessageModel.closeChat()
}
