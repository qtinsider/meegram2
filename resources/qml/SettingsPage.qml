import QtQuick 1.1
import com.nokia.meego 1.1
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        text: Localization.getString("SETTINGS") + Localization.emptyString

        MouseArea {
            anchors.fill: parent
        }
    }

    Flickable {
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height + 50
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
}
