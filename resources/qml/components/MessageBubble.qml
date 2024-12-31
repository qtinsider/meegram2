import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    id: root

    property alias content: contentItem.children

    property int childrenWidth

    signal clicked
    signal pressAndHold

    height: contentItem.children[0].height + messageDate.height + (senderLabel.text !== "" ? senderLabel.height : 0) + (model.isOutgoing ? 30 : 28);
    width: parent.width

    BorderImage {
        height: parent.height + (isOutgoing ? 0 : 2)
        width: Math.max(childrenWidth, messageDate.paintedWidth + (model.isOutgoing ? 0 : 28),  senderLabel.paintedWidth) + 26
        anchors {
            left: parent.left
            leftMargin: model.isOutgoing ? parent.width - width - 10 : 10
            top: parent.top
            topMargin: model.isOutgoing ? 1 : 8
        }

        source: internal.getBubbleImage();

        border { left: 22; right: 22; bottom: 22; top: 22; }

        opacity: 1.0

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onClicked:  {
                console.log(model.id)
                console.log(utils.replaceEmoji(model.content.formattedText))
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
        anchors {
            left: parent.left
            leftMargin: isOutgoing ? 80 : 20
        }
        color: model.isOutgoing ? "white" : "black"
        text: utils.replaceEmoji(model.sender)
        font.pixelSize: 20
        font.bold: true
        wrapMode: Text.WrapAnywhere
        maximumLineCount: 1
        horizontalAlignment: model.isOutgoing ? Text.AlignRight : Text.AlignLeft
        visible: text !== ""
    }

    Item {
        id: contentItem

        height: contentItem.children[0].height
        anchors {
            top: parent.top
            topMargin: senderLabel.text === "" ? 16 : 46
        }
    }

    Label {
        id: messageDate

        width: parent.width -100
        anchors {
            left: parent.left
            leftMargin: isOutgoing ? 80 : 20
            top: contentItem.bottom
            topMargin: 4
        }
        text: model.date
        color: model.isOutgoing ? "white" : "black"
        font.pixelSize: 16
        font.weight: Font.Light
        horizontalAlignment: model.isOutgoing ? Text.AlignRight : Text.AlignLeft
    }

    QtObject {
        id: internal

        function getBubbleImage() {
            var imageSrc = "qrc:/images/";

            imageSrc += model.isOutgoing ? "outgoing" : "incoming"
            imageSrc += mouseArea.pressed ? "-pressed" : "-normal"

            return imageSrc + ".png";
        }
    }
}
