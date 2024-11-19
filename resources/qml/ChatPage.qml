import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    property alias chat: myMessageModel.chat
    property alias chatInfo: myMessageModel.chatInfo

    property int replyMessageId: 0

    property QtObject platformStyle: SheetStyle {}

    Connections {
        id: fetchConnections
        target: null

        onBackFetchable: {
            if (listView.model.canFetchMoreBack()) {
                fetchMoreTimer.running = true;
            }
        }
        onBackFetched: {
            fetchMoreTimer.fetching = false;
            if (fetchMoreTimer.oldIndex != -1) {
                listView.positionViewAtIndex(fetchMoreTimer.oldIndex + numItems, ListView.Beginning);
            } else {
                listView.positionViewAtIndex(0, ListView.End);
            }

            if (!listView.model.canFetchMoreBack()) {
                fetchMoreTimer.running = false;
            }
        }
    }

    Timer {
        id: fetchMoreTimer
        interval: 1000
        running: false
        repeat: true

        property bool fetching : false
        property int oldIndex : -1

        onTriggered: {
            if (listView.atYBeginning) {
                if (!fetching && listView.model.canFetchMoreBack()) {
                    fetching = true;
                    oldIndex = 0;
                    listView.model.fetchMoreBack();
                }
            }
        }

        onFetchingChanged: {}
    }

    Item {
        id: content
        anchors.fill: parent
        clip: true

        Item {
            id: header
            width: parent.width
            height: headerBackground.height

            BorderImage {
                id: headerBackground
                border {
                    left: platformStyle.headerBackgroundMarginLeft
                    right: platformStyle.headerBackgroundMarginRight
                    top: platformStyle.headerBackgroundMarginTop
                    bottom: platformStyle.headerBackgroundMarginBottom
                }
                source: platformStyle.headerBackground
                width: header.width
            }

            Item {
                id: headerContent
                anchors.fill: parent

                SheetButton {
                    id: backButton
                    anchors.left: parent.left
                    anchors.leftMargin: root.platformStyle.rejectButtonLeftMargin
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("WidgetChats")
                    onClicked: pageStack.pop()
                }

                Row {
                    id: chatInfoRow
                    anchors.right: parent.right
                    anchors.rightMargin: root.platformStyle.rejectButtonLeftMargin
                    anchors.verticalCenter: parent.verticalCenter

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

                        Label {
                            text: chatInfo.title
                            font.bold: true
                            horizontalAlignment: Text.AlignRight
                            elide: Text.ElideRight
                            width: 230
                        }

                        Label {
                            text: chatInfo.status
                            font {
                                weight: Font.Light
                                pixelSize: 20
                            }
                            horizontalAlignment: Text.AlignRight
                            elide: Text.ElideRight
                            width: 230
                        }
                    }
                }
            }
        }

        Component {
            id: sectionDateDelegate
            Item {
                width: listView.width
                height: 50

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                    color: "black"
                    font.bold: true
                    font.pixelSize: 18
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                    text: section
                    wrapMode: Text.WordWrap
                }
            }
        }

        ListView {
            id: listView
            anchors {
                top: header.bottom
                left: parent.left
                right: parent.right
                bottom: inputPanelHolder.top
            }

            cacheBuffer: 600
            delegate: MessageDelegate {}

            model: myMessageModel

            highlightFollowsCurrentItem: true
            currentIndex: count - 1

            clip: true

            section.property: "section"
            section.criteria: ViewSection.FullString
            section.delegate: sectionDateDelegate

            interactive: contentHeight > height

            ScrollDecorator { flickableItem: listView }
        }

        Column {
            id: inputPanelHolder

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

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
