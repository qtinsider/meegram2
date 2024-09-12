import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0

Item {
    id: root

    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin * 2

    property variant content: authorization.content

    property string text: content.text
    property int minUserAge: content.minUserAge
    property bool showPopup: content.showPopup

    // Registration
    Column {
        id: signUpColumn

        anchors.fill: parent
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

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

    QueryDialog {
        id: dialog
        titleText: qsTr("TermsOfService")
        message: qsTr("TermsOfServiceLogin")
        rejectButtonText: qsTr("Close")
    }

    Component.onCompleted: {
        firstName.forceActiveFocus();

        acceptButton.clicked.connect(function () { authorization.registerUser(firstName.text, lastName.text) })
    }
}
