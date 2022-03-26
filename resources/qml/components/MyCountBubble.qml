import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: root

    property bool isMuted: false

    property string value: ""

    implicitWidth: text.paintedWidth + 19
    implicitHeight: 32

    BorderImage {
        source: "image://theme/" + theme.colorString + (isMuted ? "meegotouch-new-items-counter-background-combined" : "meegotouch-countbubble-background-large")
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
}
