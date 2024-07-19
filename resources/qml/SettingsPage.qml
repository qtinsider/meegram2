import QtQuick 1.1
import com.nokia.meego 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: app.locale.getString("SETTINGS") + app.locale.emptyString
    }

    ListView {
        id: listView

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: header.bottom
        }

        model: [app.locale.getString("Language") + app.locale.emptyString]
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
