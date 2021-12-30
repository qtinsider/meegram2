import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1

Item {
    id: root

    signal pressAndHold

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
            width: parent.width - bubbleLoader.width
            anchors.verticalCenter: parent.verticalCenter
            font.weight: Font.Light
            font.pixelSize: 22
            color: mouseArea.pressed ? "#797979" : "#505050"
            elide: Text.ElideRight
            text: model.lastMessageContent
        }

        Loader {
            id: bubbleLoader

            anchors { verticalCenter: parent.verticalCenter; right: parent.right }

            sourceComponent: model.unreadCount ? countBubble : model.isPinned ? pinnedBubble : moreIndicator
        }

        Component {
            id: countBubble

            CustomBubble {
                isMuted: model.isMuted
                value: model.unreadCount
            }
        }

        Component {
            id: moreIndicator

            MoreIndicator {}
        }

        Component {
            id: pinnedBubble

            BorderImage {
                border { left: 10; top: 10; right: 10; bottom: 10 }
                source: "image://theme/" + theme.colorString + "meegotouch-new-items-counter-background-combined"

                width: 32
                height: 32

                Text {
                    id: text
                    height: parent.height
                    color: "#ffffff"
                    font.family: icons.fontFamily
                    font.pixelSize: 32
                    anchors.horizontalCenter: parent.horizontalCenter
                    verticalAlignment: Text.AlignVCenter
                    text: icons.pinnedchat
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            pageStack.push(Qt.createComponent("qrc:/qml/MessagePage.qml"), { chat: myChatModel.get(index) })
        }
        onPressAndHold: root.pressAndHold()
    }
}
