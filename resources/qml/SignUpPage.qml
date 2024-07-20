import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    property string text
    property int minUserAge
    property bool showPopup

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
                text: app.getString("YourName") + app.emptyString
                font.pixelSize: 40
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: flickable.width
            }

            // Registration
            Column {
                id: signUpColumn

                width: parent.width
                spacing: 10

                Label {
                    text: app.getString("RegisterText2") + app.emptyString
                }

                Column {
                    spacing: 15
                    width: parent.width

                    TextField {
                        id: firstName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: app.getString("FirstName") + app.emptyString
                    }
                    TextField {
                        id: lastName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: app.getString("LastName") + app.emptyString
                    }
                }

                // TODO(strawberry): refactor
                Label {
                    width: parent.width
                    text: "<style type=text/css> a { text-decoration: none; color: #0088cc } </style>By signing up,<br>you agree to the <a href='http://www.telegram.com'>Terms of Service.</a>"
                    font.pixelSize: 24
                    onLinkActivated: dialog.open()
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: app.getString("Next") + app.emptyString
                onClicked: authorization.registerUser(firstName.text, lastName.text)
            }
            ToolButton {
                text: app.getString("Cancel") + app.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    QueryDialog {
        id: dialog
        titleText: app.getString("TermsOfService") + app.emptyString
        message: app.getString("TermsOfServiceLogin") + app.emptyString
        rejectButtonText: app.getString("Close") + app.emptyString
    }
}
