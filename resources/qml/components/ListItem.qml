import QtQuick 1.1
import com.nokia.meego 1.1
import "UIConstants.js" as UI

Item {
    id: root

    property bool subItemIndicator: false
    property bool isSelected: false

    signal clicked
    signal pressAndHold

    height: 64 + UI.PADDING_DOUBLE * 2
    width: parent ? parent.width : screen.displayWidth
    opacity: enabled ? UI.OPACITY_ENABLED : UI.OPACITY_DISABLED

    Loader {
        anchors {
            fill: parent
            topMargin: 1
            bottomMargin: 1
        }
        sourceComponent: (mouseArea.pressed) || (root.isSelected) ? highlight : undefined
    }

    Component {
        id: highlight

        Rectangle {
            anchors.fill: parent
            color: root.isSelected ? Settings.activeColor : UI.COLOR_INVERTED_SECONDARY_FOREGROUND
            opacity: 0.5
        }
    }

    Loader {
        anchors {
            right: parent.right
            rightMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }
        sourceComponent: root.subItemIndicator ? indicator : undefined
    }

    Component {
        id: indicator

        Image {
            source: "image://theme/icon-m-common-drilldown-arrow"
        }
    }

    MouseArea {
        id: mouseArea

        z: 1
        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
        onPressAndHold: root.pressAndHold()
    }
}
