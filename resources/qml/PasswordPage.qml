import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import MyComponent 1.0

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
                text: app.getString("TwoStepVerification") + app.emptyString
                font.pixelSize: 40
            }

            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: flickable.width
            }

            // Password Section
            Column {
                id: enterPasswordColumn
                width: parent.width
                spacing: 10

                Label {
                    width: parent.width
                    text: app.getString("LoginPasswordText") + app.emptyString
                }

                Item {
                    height: 3
                }

                Label {
                    text: app.getString("YourPassword") + app.emptyString
                }

                TextField {
                    id: password
                    width: parent.width
                    echoMode: TextInput.Password
                    placeholderText: app.getString("Password") + app.emptyString
                }

                Label {
                    id: hint
                    width: parent.width
                    font.pixelSize: 24
                    text: "<b>%1:</b> <span style=\"color: #999\">(%2)</span>".arg(app.getString("PasswordHint")).arg(passwordHint)
                    visible: passwordHint !== ""
                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: app.getString("Next") + app.emptyString
                onClicked: {
                    authorization.loading = true;
                    authorization.checkPassword(password.text)
                }
            }
            ToolButton {
                text: app.getString("Cancel") + app.emptyString
                onClicked: {
                    authorization.loading = false;
                    root.cancelClicked()
                }
            }
        }
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: authorization.loading
        visible: authorization.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }
}
