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
                text: locale.getString("YourName") + locale.emptyString
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
                    text: locale.getString("RegisterText2") + locale.emptyString
                }

                Column {
                    spacing: 15
                    width: parent.width

                    TextField {
                        id: firstName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: locale.getString("FirstName") + locale.emptyString
                    }
                    TextField {
                        id: lastName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: locale.getString("LastName") + locale.emptyString
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
                text: locale.getString("Next") + locale.emptyString
                onClicked: authorization.registerUser(firstName.text, lastName.text)
            }
            ToolButton {
                text: locale.getString("Cancel") + locale.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    QueryDialog {
        id: dialog
        titleText: locale.getString("TermsOfService") + locale.emptyString
        message: locale.getString("TermsOfServiceLogin") + locale.emptyString
        rejectButtonText: locale.getString("Close") + locale.emptyString
    }
}
