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
                        onClicked: pageStack.push(Qt.createComponent("ChatInfoPage.qml"))
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
                        sourceComponent: model.isServiceMessage ? textMessageComponent : deleglateChooser.get(model.contentType)

                        Component {
                            id: textMessageComponent

                            MessageBubble {
                                childrenWidth: messageText.paintedWidth

                                content: FormattedText {
                                    id: messageText
                                    formattedText: model.isService ? model.serviceMessage.trim() : model.content
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
                            function get(contentType) {
                                switch (contentType) {
                                case "text":
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

                function reverseItems() {
                    var children = listView.contentItem.children;
                    var yPosition = 0;

                    for (var i = children.length - 1; i >= 0; i--) {
                        var item = children[i];
                        item.y = yPosition;       // Set the item's y position
                        yPosition += item.height; // Accumulate the height for the next item
                    }
                }

                onAtYBeginningChanged: {
                    // if (atYBeginning)
                    //     myMessageModel.loadHistory()
                }

                onContentYChanged: {
                    // reverseItems();
                    // Adjust scrolling to show the bottom-most item
                    // listView.contentY = listView.contentHeight - listView.height;
                }

                onContentHeightChanged: {
                    // reverseItems();
                    // Adjust scrolling to show the bottom-most item
                    // listView.contentY = listView.contentHeight - listView.height;
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

                Component.onCompleted: {
                    // reverseItems();  // Initial reverse layout on startup
                    // listView.contentY = listView.contentHeight - listView.height;  // Ensure the view starts at the bottom
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

    MessageModel {
        id: myMessageModel
        onMoreHistoriesLoaded: {
            listView.positionViewAtIndex(modelIndex - 1, ListView.Beginning)
        }
    }

    Component.onDestruction: { myMessageModel.closeChat() }
}
