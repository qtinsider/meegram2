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
                text: Localization.getString("TwoStepVerification") + Localization.emptyString
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
                    text: Localization.getString("LoginPasswordText") + Localization.emptyString
                }

                Item {
                    height: 3
                }

                Label {
                    text: Localization.getString("YourPassword") + Localization.emptyString
                }

                TextField {
                    id: password
                    width: parent.width
                    echoMode: TextInput.Password
                    placeholderText: Localization.getString("Password") + Localization.emptyString
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
                text: Localization.getString("Next") + Localization.emptyString
                onClicked: Api.checkPassword(password.text)
            }
            ToolButton {
                text: Localization.getString("Cancel") + Localization.emptyString
                onClicked: {
                    pageStack.pop()
                }
            }
        }
    }

    Component.onCompleted: Api.error.connect(function(error) { showInfoBanner(error.message) })
    Component.onDestruction: Api.error.disconnect(function(error) { showInfoBanner(error.message) })
}
