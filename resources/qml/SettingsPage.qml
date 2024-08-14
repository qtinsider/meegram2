import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: "SETTINGS"
    }

    ListView {
        id: listView

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: header.bottom
        }

        model: [app.getString("Language") + app.emptyString]
        delegate: DrillDownDelegate {
            text: modelData
            onClicked: {
                switch (index) {
                case 0:
                    appWindow.pageStack.push(Qt.createComponent("LanguageSettingsPage.qml"));
                    break;
                default:
                    break;
                }
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: appWindow.pageStack.pop()
        }
    }
}
