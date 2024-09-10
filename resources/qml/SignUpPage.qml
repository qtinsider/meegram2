import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import MyComponent 1.0

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
                text: qsTr("Next")
                onClicked: {
                    authorization.loading = true;
                    authorization.registerUser(firstName.text, lastName.text);
                }
            }
            ToolButton {
                text: qsTr("Cancel")
                onClicked: {
                    authorization.loading = false;
                    root.cancelClicked()
                }
            }
        }
    }

    QueryDialog {
        id: dialog
        titleText: qsTr("TermsOfService")
        message: qsTr("TermsOfServiceLogin")
        rejectButtonText: qsTr("Close")
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: authorization.loading
        visible: authorization.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }
}
