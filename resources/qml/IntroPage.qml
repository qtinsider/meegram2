import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        text: "MeeGram"

        MouseArea { anchors.fill: parent }
    }

    Item {
        id: signInInfo

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        clip: true

        Column {
            id: signInInfoColumn

            anchors{
                top: parent.top
                topMargin: 30
                left: parent.left
                right: parent.right
            }

            spacing: 16

            Text {
                anchors{
                    left: parent.left
                    right: parent.right
                    leftMargin: 24
                    rightMargin: 24
                }

                text: "Different, Handy, Powerful"
                wrapMode: Text.WordWrap

                font.pixelSize: 30
                color: "#777777"

                horizontalAlignment: Text.AlignHCenter
            }

            Column {
                anchors.horizontalCenter: parent.horizontalCenter

                spacing: 16

                Button {
                    text: Localization.getString("StartMessaging") + Localization.emptyString

                    platformStyle: ButtonStyle { inverted: true }

                    onClicked: {
                        var component = Qt.createComponent("SignInPage.qml")
                        if (component.status === Component.Ready) {
                            pageStack.push(component)
                        }
                    }
                }
            }
        }
    }

    function onIsAuthorizedChanged() {
        if (Api.isAuthorized) {}
        console.log("Introoooooooooooooooooooooooooooooooooooo")
    }

    AboutDialog {
        id: aboutDialog
    }

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

    Component.onCompleted: Api.isAuthorizedChanged.connect(onIsAuthorizedChanged)
}
