import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    property alias chat: myMessageModel.chat
    property alias chatInfo: myMessageModel.chatInfo

    property int replyMessageId: 0
    property int editMessageId: 0
    property bool __loading: true

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
                    text: qsTr("Chats")
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

                MaskedItem {
                    id: maskedItem
                    anchors.verticalCenter: parent.verticalCenter
                    height: 50
                    width: 50

                    mask: Image {
                        sourceSize.width: maskedItem.width
                        sourceSize.height: maskedItem.height
                        source: "qrc:/images/avatar-image-mask.png"
                    }

                    Image {
                        id: profilePhotoImage
                        anchors.fill: parent
                        cache:  false
                        smooth: true
                        fillMode: Image.PreserveAspectCrop
                        clip: true
                        source: chat.photo && chat.photo.localPath !== "" ?
                                    "image://chatPhoto/" + chat.photo.localPath :
                                    "image://theme/icon-l-content-avatar-placeholder"
                    }

                    MouseArea {
                        anchors.fill: parent
                    }
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    width: parent.width

                    Label {
                        text: chatInfo.title
                        font.bold: true
                        width: parent.width
                        horizontalAlignment: Text.AlignRight
                        elide: Text.ElideRight
                    }

                    Label {
                        text: chatInfo.status
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
                spacing: 6
                clip: true
                cacheBuffer: listView.height * 2
                pressDelay: 50

                delegate: Component {
                    Loader {
                        id: loader
                        width: listView.width
                        // height: childrenRect.height
                        sourceComponent: model.isService ? textMessageComponent  : deleglateChooser.get(model.contentType)

                        Component {
                            id: textMessageComponent

                            MessageBubble {
                                childrenWidth: messageText.paintedWidth

                                content: Label {
                                    id: messageText
                                    text: model.content.formattedText
                                    textFormat: Text.RichText
                                    color: model.isService ? "gray" : model.isOutgoing ? "white" : "black"
                                    width: isPortrait ? 380 : 754
                                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                                    anchors {
                                        left: parent.left
                                        leftMargin: model.isService ? 50 : model.isOutgoing ? 80 : 20
                                    }
                                    font.pixelSize: model.isService ? 18 : 23
                                    horizontalAlignment: model.isService ? Text.AlignHCenter : model.isOutgoing ? Text.AlignRight : Text.AlignLeft
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
                                        leftMargin: model.isOutgoing ? 80 : 20
                                    }
                                    width: isPortrait ? 380 : 754
                                    font {
                                        bold: true
                                        pixelSize: 23
                                    }
                                    horizontalAlignment: model.isOutgoing ? Text.AlignRight : Text.AlignLeft
                                    wrapMode: Text.Wrap
                                    color: model.isOutgoing ? "white" : "black"
                                    text: "The message is not supported on MeeGram yet"
                                }
                            }
                        }

                        QtObject {
                            id: deleglateChooser
                            function get(contentType) {
                                switch (contentType) {
                                case "messageText":
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

                Connections {
                    target: inputContext
                    onSoftwareInputPanelVisibleChanged: {
                        if (inputContext.softwareInputPanelVisible) {
                            listView.positionViewAtEnd();
                        }
                    }
                }

                ScrollDecorator { flickableItem: listView }
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

    MessageModel {
        id: myMessageModel
    }

    Component.onDestruction: { myMessageModel.closeChat() }
}
