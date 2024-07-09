import QtQuick 1.1
import com.nokia.meego 1.1
import "../constants.js" as UI

Item {
    id: listItem

    signal clicked
    property alias pressed: mouseArea.pressed

    property alias title: mainText.text
    property alias subTitle: subText.text

    property int titleSize: UI.LIST_TILE_SIZE
    property int titleWeight: Font.Bold
    property color titleColor: theme.inverted ? UI.LIST_TITLE_COLOR_INVERTED : UI.LIST_TITLE_COLOR

    property int subtitleSize: UI.LIST_SUBTILE_SIZE
    property int subtitleWeight: Font.Light
    property color subtitleColor: theme.inverted ? UI.LIST_SUBTITLE_COLOR_INVERTED : UI.LIST_SUBTITLE_COLOR

    height: UI.LIST_ITEM_HEIGHT
    width: parent.width

    BorderImage {
        id: background
        anchors.fill: parent
        // Fill page porders
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
    }

    Row {
        anchors {
            top: parent.top
            left: parent.left
            leftMargin: UI.MARGIN_XLARGE
            bottom: parent.bottom
            right: icon.left
            rightMargin: UI.MARGIN_XLARGE
        }
        spacing: UI.LIST_ITEM_SPACING

        Column {
            anchors.verticalCenter: parent.verticalCenter

            Label {
                id: mainText
                font.weight: listItem.titleWeight
                font.pixelSize: listItem.titleSize
                color: listItem.titleColor
            }

            Label {
                id: subText
                font.weight: listItem.subtitleWeight
                font.pixelSize: listItem.subtitleSize
                color: listItem.subtitleColor

                visible: text != ""
            }
        }
    }
    Image {
        id: icon

        anchors {
            right: parent.right
//            rightMargin: UI.MARGIN_XLARGE
            verticalCenter: parent.verticalCenter
        }
        height: sourceSize.height
        width: sourceSize.width
        source: "image://theme/meegotouch-combobox-indicator" + platformStyle.__invertedString
    }

    MouseArea {
        id: mouseArea;
        anchors.fill: parent
        onClicked: {
            listItem.clicked();
        }
    }
}
