import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    property string passwordHint
    property bool hasRecoveryEmailAddress
    property string recoveryEmailAddressPattern

    signal cancelClicked

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
                text: app.locale.getString("TwoStepVerification") + app.locale.emptyString
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
                    text: app.locale.getString("LoginPasswordText") + app.locale.emptyString
                }

                Item {
                    height: 3
                }

                Label {
                    text: app.locale.getString("YourPassword") + app.locale.emptyString
                }

                TextField {
                    id: password
                    width: parent.width
                    echoMode: TextInput.Password
                    placeholderText: app.locale.getString("Password") + app.locale.emptyString
                }

                Label {
                    id: hint
                    width: parent.width
                    font.pixelSize: 24
                    text: "<b>%1:</b> <span style=\"color: #999\">(%2)</span>".arg(app.locale.getString("PasswordHint")).arg(passwordHint)
                    visible: passwordHint !== ""
                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: app.locale.getString("Next") + app.locale.emptyString
                onClicked: authorization.checkPassword(password.text)
            }
            ToolButton {
                text: app.locale.getString("Cancel") + app.locale.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }
}
