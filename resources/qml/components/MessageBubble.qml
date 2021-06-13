import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    id: root

    property string sender: model.sender
    property string date: model.date
    property bool isOutgoing: model.isOutgoing

    property alias content: contentItem.children

    property int childrenWidth

    signal clicked
    signal pressAndHold

    height: contentItem.children[0].height + messageDate.height + (senderName.text !== "" ? senderName.height : 0) + (isOutgoing ? 28 : 30);
    width: isPortrait ? 480 : 854

    BorderImage {
        height: parent.height + (isOutgoing ? 2 : 0)
        width: Math.max(childrenWidth, messageDate.paintedWidth + (isOutgoing ? 28 : 0),  senderName.paintedWidth) + 26
        anchors {
            left: parent.left
            leftMargin: isOutgoing ? 10 : parent.width - width - 10
            top: parent.top
            topMargin: isOutgoing ? 8 : 1
        }

        source: internal.getBubbleImage();

        border { left: 22; right: 22; bottom: 22; top: 22; }

        opacity: 1.0

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onClicked: root.clicked()
            onPressAndHold: root.pressAndHold()
        }
    }

    Label {
        id: senderName
        y: 18
        width: parent.width -100
        anchors { left: parent.left; leftMargin: 80 }
        color: "white"
        text: sender
        font.pixelSize: 20
        font.bold: true
        wrapMode: Text.WrapAnywhere
        maximumLineCount: 1
        horizontalAlignment: Text.AlignRight
        visible: senderName.text !== ""
    }

    Item {
        id: contentItem

        height: contentItem.children[0].height
        anchors {
            top: parent.top
            topMargin: senderName.text === "" ? 16 : 46
        }
    }

    Label {
        id: messageDate

        width: parent.width -100
        anchors {
            left: parent.left
            leftMargin: isOutgoing ? 20 : 80
            top: contentItem.bottom
            topMargin: 4
        }
        text: date
        color: isOutgoing ? "black" : "white"
        font.pixelSize: 16
        font.weight: Font.Light
        horizontalAlignment: isOutgoing ? Text.AlignLeft : Text.AlignRight
    }

    QtObject {
        id: internal

        function getBubbleImage() {
            var imageSrc = "../images/";

            imageSrc += isOutgoing ? "outgoing" : "incoming"
            imageSrc += mouseArea.pressed ? "-pressed" : "-normal"

            imageSrc += ".png"

            return imageSrc;
        }
    }

}
