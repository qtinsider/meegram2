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
                text: Localization.getString("YourName") + Localization.emptyString
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
                    text: Localization.getString("RegisterText2") + Localization.emptyString
                }

                Column {
                    spacing: 15
                    width: parent.width

                    TextField {
                        id: firstName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: Localization.getString("FirstName") + Localization.emptyString
                    }
                    TextField {
                        id: lastName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: Localization.getString("LastName") + Localization.emptyString
                    }
                }

                Label {
                    width: parent.width
                    text: "<style type=text/css> a { text-decoration: none; color: #0088cc } </style>By signing up,<br>you agree to the <a href='http://www.telegram.com'>Terms of Service.</a>"
                    font.pixelSize: 24
                    onLinkActivated: internal.showTermsOfService()
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: Localization.getString("Next") + Localization.emptyString
                onClicked: Api.registerUser(firstName.text, lastName.text)
            }
            ToolButton {
                text: Localization.getString("Cancel") + Localization.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    QueryDialog {
        id: dialog
        titleText: "Terms of Service"
        message: termsOfService
        rejectButtonText: Localization.getString("Close") + Localization.emptyString
    }
}
