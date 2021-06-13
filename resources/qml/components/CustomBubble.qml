import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: root

    property bool isMute: false

    property int value: 0

    implicitWidth: internal.getBubbleWidth()
    implicitHeight: 32

    BorderImage {
        source: "image://theme/" + theme.colorString + (isMute ? "meegotouch-new-items-counter-background-combined" : "meegotouch-countbubble-background-large")
        anchors.fill: parent
        border { left: 10; top: 10; right: 10; bottom: 10 }
    }

    Text {
        id: text
        height: parent.height
        y:1
        color: "#FFFFFF"
        font.family: "Nokia Pure Text"
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 22
        text: root.value
    }

    QtObject {
        id: internal

        function getBubbleWidth() {
            if (root.value < 10)
                return 32;
            else if (root.value < 100)
                return 40;
            else if (root.value < 1000)
                return 52;
            else
                return text.paintedWidth + 19

        }
    }
}
