import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    property string passwordHint

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.margins: 16
        contentHeight: contentColumn.height

        Column {
            id: contentColumn

            width: flickable.width
            height: childrenRect.height

            spacing: 16


            Label {
                id: title
                text: qsTr("TwoStepVerification")
                font.pixelSize: 40
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: flickable.width
            }

            // Password
            Column {
                id: enterPasswordColumn

                width: parent.width
                spacing: 10

                Label {
                    width: parent.width
                    text: qsTr("LoginPasswordText")
                }

                Item {
                    height: 3
                }

                Label {
                    text: qsTr("YourPassword")
                }

                TextField {
                    id: password
                    width: parent.width
                    echoMode: TextInput.Password
                    placeholderText: qsTr("Password")
                }

                Label {
                    id: hint
                    width: parent.width
                    font.pixelSize: 24
                    text: "<b>Password hint:</b> <span style=\"color: #999\">(%1)</span>".arg(passwordHint)
                    visible: passwordHint !== ""
                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: qsTr("Next")
                onClicked: tdapi.checkPassword(password.text)
            }
            ToolButton {
                text: qsTrId("Cancel")
                onClicked: {
                    pageStack.pop()
                }
            }
        }
    }

    Component.onCompleted: tdapi.error.connect(function(error) { showInfoBanner(error.message) })
    Component.onDestruction: tdapi.error.disconnect(function(error) { showInfoBanner(error.message) })
}
