import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    id: root

    property alias content: contentItem.children

    property int childrenWidth

    signal clicked
    signal pressAndHold

    height: model.isServiceMessage ? contentItem.children[0].height + 30 : contentItem.children[0].height + messageDate.height + (senderLabel.text !== "" ? senderLabel.height : 0) + (model.isOutgoing ? 28 : 30);
    width: parent.width

    BorderImage {
        height: parent.height + (isOutgoing ? 2 : 0)
        width: Math.max(childrenWidth, messageDate.paintedWidth + (model.isOutgoing ? 28 : 0),  senderLabel.paintedWidth) + 26
        anchors {
            left: parent.left
            leftMargin: model.isServiceMessage ? (parent.width - width) / 2 : model.isOutgoing ? 10 : parent.width - width - 10
            top: parent.top
            topMargin: model.isServiceMessage ? 2 : model.isOutgoing ? 8 : 1
        }

        source: internal.getBubbleImage();

        border { left: 22; right: 22; bottom: 22; top: 22; }

        opacity: 1.0

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onClicked:  {
                console.log(model.id)
                root.clicked()
            }
            onPressAndHold: {
                // if (myMessageModel.copyToClipboard(model.content)) {
                //     banner.text = "Copy text to clipbord";
                //     banner.show()
                // }
            }
        }
    }

    Label {
        id: senderLabel
        y: 18
        width: parent.width -100
        anchors { left: parent.left; leftMargin: 80 }
        color: "white"
        text: model.sender
        font.pixelSize: 20
        font.bold: true
        wrapMode: Text.WrapAnywhere
        maximumLineCount: 1
        horizontalAlignment: Text.AlignRight
        visible: text !== "" && !isServiceMessage
    }

    Item {
        id: contentItem

        height: contentItem.children[0].height
        anchors {
            top: parent.top
            topMargin: model.isServiceMessage ? 15 : senderLabel.text === "" ? 16 : 46
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
        text: model.date
        color: model.isOutgoing ? "black" : "white"
        font.pixelSize: 16
        font.weight: Font.Light
        horizontalAlignment: model.isServiceMessage ? Text.AlignHCenter : model.isOutgoing ? Text.AlignLeft : Text.AlignRight
        visible: !model.isServiceMessage
    }

    QtObject {
        id: internal

        function getBubbleImage() {
            var imageSrc = "qrc:/images/";
            if (model.isServiceMessage) {
                imageSrc += "notification"
            } else {
                imageSrc += model.isOutgoing ? "outgoing" : "incoming"
                imageSrc += mouseArea.pressed ? "-pressed" : "-normal"
            }

            return imageSrc + ".png";
        }
    }
}
