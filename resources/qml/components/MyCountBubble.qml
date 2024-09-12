import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    id: root

    property bool isMuted: false
    property alias value: label.text

    implicitWidth: label.paintedWidth + 19
    implicitHeight: 32

    BorderImage {
        source: "image://theme/" + theme.colorString + (isMuted ? "meegotouch-new-items-counter-background-combined" : "meegotouch-countbubble-background-large")
        anchors.fill: parent
        border.left: 10
        border.top: 10
        border.right: 10
        border.bottom: 10
    }

    Label {
        id: label
        height: parent.height
        y: 1
        color: "#FFFFFF"
        anchors.horizontalCenter: parent.horizontalCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: 22
    }
}
