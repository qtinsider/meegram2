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

    MaskedItem {
        id: maskedItem

        anchors {
            left: parent.left
            leftMargin: 12
            verticalCenter: parent.verticalCenter
        }

        width: 64
        height: 64

        mask: Image {
            sourceSize.width: maskedItem.width
            sourceSize.height: maskedItem.height
            width: maskedItem.width
            height: maskedItem.height
            source: "qrc:/images/avatar-image-mask.png"
        }

        Image {
            id: profilePhotoImage
            anchors.fill: parent
            cache:  false
            smooth: true
            fillMode: Image.PreserveAspectCrop
            clip: true
            source: model.photo ? "image://chatPhoto/" + model.photo : "image://theme/icon-l-content-avatar-placeholder"
        }
    }

    Item {
        id: row1

        width: parent.width - maskedItem.width - 44
        height: 45
        anchors {
            left: maskedItem.right
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
        width: parent.width - maskedItem.width - 44
        anchors { left: row1.left; top: row1.bottom }

        Label {
            id: lastMessage
            width: parent.width - mentionLoader.width - bubbleLoader.width
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

            sourceComponent: model.unreadCount > 0 ? countBubble : model.isPinned ? pinnedBubble : undefined
        }

        Loader {
            id: mentionLoader

            anchors {
                leftMargin: 8
                right: bubbleLoader.left
                rightMargin: 8
                verticalCenter: parent.verticalCenter
            }

            sourceComponent: model.unreadMentionCount > 0 ? mentionBubble : undefined
        }

        Component {
            id: countBubble

            CustomBubble {
                isMuted: model.isMuted
                value: model.unreadCount
            }
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

        Component {
            id: mentionBubble

            BorderImage {
                border { left: 10; top: 10; right: 10; bottom: 10 }
                source: "image://theme/" + theme.colorString + "meegotouch-countbubble-background-large"

                width: 32
                height: 32

                Text {
                    id: text
                    height: parent.height
                    color: "#ffffff"
                    font.family: icons.fontFamily
                    font.pixelSize: 22
                    anchors.horizontalCenter: parent.horizontalCenter
                    verticalAlignment: Text.AlignVCenter
                    text: icons.username
                }
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: {
            appWindow.openChat(myChatModel.get(index).id);
        }
        onPressAndHold: root.pressAndHold()
    }
}
