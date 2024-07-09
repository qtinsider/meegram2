import QtQuick 1.1
import com.nokia.meego 1.1
import "../constants.js" as UI

Item {
    id: root

    property alias title: titleLabel.text
    property alias description: descriptionLabel.text
    property alias checked: toggle.checked

    height: childrenRect.height
    anchors {
        left: parent.left
        right: parent.right
        leftMargin: UI.MARGIN_XLARGE
        rightMargin: UI.MARGIN_XLARGE
    }

    Column {
        id: labels

        anchors {
            left: parent.left
            right: toggle.left
            rightMargin: 8 /*UI.PADDING_LARGE*/
        }

        Label {
            id: titleLabel
            color: theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED
                                  : UI.LIST_TITLE_COLOR
            font {
                weight: Font.Bold
                pixelSize: UI.LIST_TILE_SIZE
            }
            anchors {
                left: parent.left
                right: parent.right
            }
        }
        Label {
            id: descriptionLabel
            color: theme.inverted ? UI.LIST_SUBTITLE_COLOR_INVERTED
                                  : UI.LIST_SUBTITLE_COLOR
            font {
                weight: UI.LIST_SUBTILE_SIZE
                pixelSize: Font.Light
            }
            anchors {
                left: parent.left
                right: parent.right
            }
        }
    }
    Switch {
        id: toggle

        anchors {
            right: parent.right
            verticalCenter: labels.verticalCenter
        }
    }
}
