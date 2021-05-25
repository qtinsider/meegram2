import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    id: root

    property variant content
    property bool isOutgoing
    property string date
    property string sender

    property bool isServiceMessage
    property string serviceMessage

    property string __notSupportedString: "The message is not supported on MeeGram yet"
    property string __textMessage: content.text === undefined ? __notSupportedString : Utils.getFormattedText(content.text, false)

    signal clicked
    signal pressAndHold

    anchors.right: isOutgoing ? this.right: parent.right
    anchors.left: !isOutgoing ? this.left: parent.left

    width: bubbleLoader.width
    height: bubbleLoader.height

    Loader {
        id: bubbleLoader
        sourceComponent: isServiceMessage ? serviceMessageComponent : messageTextComponent
    }

    Component {
        id: serviceMessageComponent

        TextBubble {
            message: root.serviceMessage
            isServiceMessage: root.isServiceMessage

            onClicked: root.clicked()
            onPressAndHold: root.pressAndHold()
        }
    }

    Component {
        id: messageTextComponent

        TextBubble {
            message: root.__textMessage
            date: root.date
            isOutgoing: root.isOutgoing
            sender: root.sender

            onClicked: root.clicked()
            onPressAndHold: root.pressAndHold()
        }
    }
}
