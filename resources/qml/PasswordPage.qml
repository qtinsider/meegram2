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
                text: locale.getString("TwoStepVerification") + locale.emptyString
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
                    text: locale.getString("LoginPasswordText") + locale.emptyString
                }

                Item {
                    height: 3
                }

                Label {
                    text: locale.getString("YourPassword") + locale.emptyString
                }

                TextField {
                    id: password
                    width: parent.width
                    echoMode: TextInput.Password
                    placeholderText: locale.getString("Password") + locale.emptyString
                }

                Label {
                    id: hint
                    width: parent.width
                    font.pixelSize: 24
                    text: "<b>%1:</b> <span style=\"color: #999\">(%2)</span>".arg(locale.getString("PasswordHint")).arg(passwordHint)
                    visible: passwordHint !== ""
                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: locale.getString("Next") + locale.emptyString
                onClicked: authorization.checkPassword(password.text)
            }
            ToolButton {
                text: locale.getString("Cancel") + locale.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }
}
