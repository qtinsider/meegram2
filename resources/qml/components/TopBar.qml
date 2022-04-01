import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    id: root
    z: 100

    property alias title: label.text
    property bool isArchived: false

    property color color: isArchived ? "#424345" :  theme.selectionColor

    signal clicked

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }

    height: isPortrait ? 72 : 64

    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: root.color }
            GradientStop { position: 1.0; color: Qt.darker(root.color) }
        }
    }

    Label {
        id: label
        anchors {
            top: parent.top
            bottom: parent.bottom
            topMargin: isPortrait ? 16 : 12
            bottomMargin: isPortrait ? 24 : 20
            left: parent.left
            right: parent.right
            leftMargin: 16
            rightMargin: 16
        }
        height: 32

        font.family: "Nokia Pure Text Light"
        font.pixelSize: 32
        color: "#ffffff"
        elide: Text.ElideRight
    }

    MouseArea {
        anchors.fill: parent
        onClicked: root.clicked()
    }
}
