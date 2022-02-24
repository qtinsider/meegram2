import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    property string termsOfServiceString: ""

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
                text: qsTr("YourName")
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
                    text: qsTr("RegisterText2")
                }

                Column {
                    spacing: 15
                    width: parent.width

                    TextField {
                        id: firstName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: qsTr("FirstName")
                    }
                    TextField {
                        id: lastName
                        width: parent.width
                        inputMethodHints: Qt.ImhNoPredictiveText
                        placeholderText: qsTr("LastName")
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
                text: qsTr("Next")
                onClicked: tdapi.registerUser(firstName.text, lastName.text)
            }
            ToolButton {
                text: qsTrId("Cancel")
                onClicked: {
                    pageStack.pop()
                }
            }
        }
    }

    QtObject {
        id: internal
        function showTermsOfService() {
            var dialog = termsOfServiceComponent.createObject(sheet, { termsOfService: termsOfServiceString });
            dialog.open();
        }
    }

    Component {
        id: termsOfServiceComponent

        QueryDialog {
            property string termsOfService

            titleText: "Terms of Service"
            message: termsOfService
            rejectButtonText: "Close"
        }
    }

    Component.onCompleted: tdapi.error.connect(function(error) { showInfoBanner(error.message) })
}
