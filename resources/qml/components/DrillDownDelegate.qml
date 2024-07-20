import QtQuick 1.1
import com.nokia.meego 1.0

ListItem {
    id: root

    property alias text: label.text

    subItemIndicator: true

    Label {
        id: label

        anchors {
            fill: parent
            margins: 12
        }
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        font.pixelSize: 26
        font.bold: true
        text: display
    }
}
