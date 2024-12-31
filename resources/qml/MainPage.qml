import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Page {
    id: root
    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: "MeeGram"
    }

    Loader {
        id: loader
        anchors.fill: parent
        anchors.topMargin: header.height
        sourceComponent: Item {
            anchors.fill: parent
            BusyIndicator {
                anchors.centerIn: parent
                running: true
                platformStyle: BusyIndicatorStyle { size: "large" }
            }
        }
    }

    Component {
        id: infoComponent
        Item {
            anchors.fill: parent

            Column {
                spacing: 20
                width: parent.width

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    topMargin: 30
                }

                Text {
                    id: infoText
                    text: "Different, Handy, Powerful"
                    wrapMode: Text.Wrap
                    font.pixelSize: 30
                    color: "#777777"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                }

                Button {
                    text: qsTr("StartMessaging")
                    platformStyle: ButtonStyle { inverted: true }
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        var component = Qt.createComponent("AuthenticationPage.qml")
                        if (component.status === Component.Ready) {
                            var authenticationSheet = component.createObject(root);
                            authenticationSheet.open();
                        }
                    }
                }
            }
        }
    }

    AboutDialog { id: aboutDialog }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: Qt.quit()
        }
        ToolIcon {
            anchors.right: parent.right
            iconSource: "qrc:/images/help-icon.png"
            onClicked: aboutDialog.open()
        }
    }

    Connections {
        target: appManager
        onAppInitialized: {
            if (appManager.authorized) {
                pageStack.push(Qt.createComponent("ChatsPage.qml"))
            } else {
                loader.sourceComponent = infoComponent
            }
        }
    }

    Component.onCompleted: appManager.initialize()
}
