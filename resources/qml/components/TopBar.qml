import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    id: root
    z: 100

    property alias text: titleLabel.text
    property color barColor: theme.selectionColor

    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }

    height: screen.currentOrientation === Screen.Portrait ? 72 : 64

    Rectangle {
        id: background
        anchors.fill: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: barColor }
            GradientStop { position: 1.0; color: Qt.darker(barColor) }
        }
    }

    Label {
        id: titleLabel
        anchors {
            top: parent.top
            bottom: parent.bottom
            topMargin: screen.currentOrientation == Screen.Portrait ? 16 : 12
            bottomMargin: screen.currentOrientation == Screen.Portrait ? 24 : 20
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
}
