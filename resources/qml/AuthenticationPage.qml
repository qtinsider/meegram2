import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Sheet {
    id: sheet

    property int timeout

    property string passwordHint: ""

    property string termsOfServiceString: ""

    property alias __title: codeTitle.text
    property alias __subtitle: codeType.text
    property alias __isNextTypeSms: isNextTypeSms.visible
    property alias __nextTypeString: nextTypeLabel.text
    property int __length

    rejectButtonText: "Cancel"

    state: "Phone"

    content: Item {
        id: background
        anchors.fill: parent

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
                    color: "#282828"
                    font.pixelSize: 40
                }
                Rectangle {
                    color: "#b2b2b4"
                    height: 1
                    width: flickable.width
                }

                // Phone
                Column {
                    id: signInColumn

                    width: parent.width
                    spacing: 16

                    visible: (sheet.state !== "Phone") ? false : true

                    Label {
                        id: countryNameLabel
                        text: myCountryModel.get(selectionDialog.selectedIndex).name
                        width: parent.width
                        font.pixelSize: 48
                        platformSelectable: true
                        color: "#0088cc"
                        wrapMode: Text.WordWrap

                        MouseArea {
                            anchors.fill:  parent
                            onClicked: selectionDialog.open()
                        }
                    }

                    Row {
                        width: parent.width
                        spacing: 16

                        TumblerButton {
                            id: countryCodeButton
                            width: 160
                            text: "+" + myCountryModel.get(selectionDialog.selectedIndex).code
                            onClicked: selectionDialog.open()
                        }

                        TextField {
                            id: phoneNumber
                            inputMethodHints: Qt.ImhDialableCharactersOnly | Qt.ImhNoPredictiveText
                            placeholderText: "Phone number"
                        }
                    }

                    Label {
                        font.pixelSize: 24
                        width: parent.width
                        text: qsTr("StartText")
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: !tdapi.busy
                        text: qsTr("Next")
                        width: parent.width / 2

                        platformStyle: ButtonStyle { inverted: true }

                        onClicked: {
                            if (phoneNumber.text.length > 0) {
                                tdapi.setPhoneNumber(countryCodeButton.text + phoneNumber.text)
                            }
                        }

                        BusyIndicator {
                            visible: tdapi.busy
                            running: visible
                            anchors.centerIn: parent
                        }
                    }
                }

                // Code
                Column {
                    id: codeEnterColumn

                    width: parent.width
                    spacing: 16

                    visible: (sheet.state !== "Code") ? false : true

                    Label {
                        id: codeTitle
                    }

                    Column {
                        width: parent.width
                        spacing: 20

                        TextField {
                            id: code
                            width: parent.width
                            inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoPredictiveText
                            placeholderText: qsTr("Code")

                            onTextChanged: {
                                if(text.length >= __length) {
                                    tdapi.checkCode(code.text)
                                }
                            }
                        }

                        Label {
                            id: codeType
                            font.pixelSize: 24
                            width: parent.width
                        }

                        Button {
                            anchors.horizontalCenter: parent.horizontalCenter
                            enabled: !tdapi.busy
                            text: qsTr("Next")
                            width: parent.width / 2

                            platformStyle: ButtonStyle { inverted: true }

                            onClicked: {
                                tdapi.checkCode(code.text)
                            }

                            BusyIndicator {
                                visible: tdapi.busy
                                running: visible
                                anchors.centerIn: parent
                            }
                        }

                        Label {
                            id: isNextTypeSms

                            anchors.horizontalCenter: parent.horizontalCenter

                            font.pixelSize: 24
                            font.underline: true

                            color: "#0088cc"
                            text: qsTr("DidNotGetTheCodeSms")

                            visible: false

                            MouseArea {
                                anchors.fill: parent
                                onClicked: {
                                    tdapi.resendCode()
                                }
                            }
                        }

                        Row {
                            id: codeTextRow
                            anchors.horizontalCenter: parent.horizontalCenter
                            spacing: 4
                            Label {
                                id: nextTypeLabel
                                horizontalAlignment: Text.AlignHCenter
                                font.pointSize: 24
                            }
                            Label {
                                id: codeTimeText
                                horizontalAlignment: Text.AlignHCenter
                                font.pointSize: 24
                                color: theme.selectionColor // "#ffcc00"
                            }
                            visible: codeTimeText.text !== ""
                        }

                        Timer {
                            id: codeExpireTimer
                            interval: 1000
                            repeat: true
                            onTriggered: {
                                timeout = timeout - 1000;
                                codeTimeText.text = Utils.formatTime(timeout / 1000);
                                if (timeout === 0) {
                                    codeExpireTimer.stop()
                                    codeTextRow.visible = false;
                                    tdapi.resendCode()
                                }
                            }
                        }
                    }
                }

                // Password
                Column {
                    id: enterPasswordColumn

                    width: parent.width
                    spacing: 10

                    visible: (sheet.state !== "Password") ? false : true


                    Label {
                        width: parent.width
                        text: qsTr("LoginPasswordText")
                    }

                    Item {
                        height: 3
                    }

                    Label {
                        text: qsTr("YourPassword")
                    }

                    TextField {
                        id: password
                        width: parent.width
                        echoMode: TextInput.Password
                        placeholderText: qsTr("Password")
                    }

                    Label {
                        id: hint
                        width: parent.width
                        font.pixelSize: 24
                        text: "<b>Password hint:</b> <span style=\"color: #999\">(%1)</span>".arg(passwordHint)
                        visible: passwordHint !== ""
                    }

                    Item {
                        height: 3
                        width: parent.width
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: !tdapi.busy
                        text: qsTr("Next")
                        width: parent.width / 2

                        platformStyle: ButtonStyle { inverted: true }

                        onClicked: {
                            if (password.text.length > 0) {
                                tdapi.checkPassword(password.text)
                            }
                        }

                        BusyIndicator {
                            visible: tdapi.busy
                            running: visible
                            anchors.centerIn: parent
                        }
                    }

                }

                // Registration
                Column {
                    id: signUpColumn

                    width: parent.width
                    spacing: 10

                    visible: (sheet.state !== "Registration") ? false : true

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

                    Item {
                        height: 3
                        width: parent.width
                    }

                    Button {
                        anchors.horizontalCenter: parent.horizontalCenter
                        enabled: !tdapi.busy
                        width: parent.width / 2
                        text: qsTr("Done")

                        platformStyle: ButtonStyle { inverted: true }

                        onClicked: {
                            tdapi.registerUser(firstName.text, lastName.text)
                        }

                        BusyIndicator {
                            visible: tdapi.busy
                            running: visible
                            anchors.centerIn: parent
                        }
                    }
                }

            }
        }
    }

    states: [
        State {
            name: "Phone"
            PropertyChanges { target: title; text: qsTr("YourPhone") }
        },
        State {
            name: "Code"
            PropertyChanges { target: title; text: qsTr("YourCode") }
        },
        State {
            name: "Password"
            PropertyChanges { target: title; text: qsTr("TwoStepVerification") }
        },
        State {
            name: "Registration"
            PropertyChanges { target: title; text: qsTr("YourName") }
        }
    ]

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

    SelectionDialog {
        id: selectionDialog
        titleText: qsTr("ChooseCountry")
        selectedIndex: myCountryModel.defaultIndex
        model: CountryModel { id: myCountryModel }
    }

    Connections {
        target: tdapi

        onCodeRequested: {
            state = "Code"

            timeout = codeInfo.timeout * 1000

            __title = codeInfo.title
            __subtitle = codeInfo.subtitle
            __nextTypeString = codeInfo.nextTypeString
            __isNextTypeSms = codeInfo.isNextTypeSms

            __length = codeInfo.length
        }

        onRegistrationRequested: {
            state = "Registration";
            termsOfServiceString = termsOfService.text
        }

        onPasswordRequested: {
            state = "Password"
            passwordHint = passwordInfo.passwordHint
        }

        onIsAuthorizedChanged: {
            sheet.accept()
            codeExpireTimer.stop()
        }
    }

    onTimeoutChanged: {
        codeExpireTimer.start()
        codeTimeText.text = Utils.formatTime(timeout / 1000)
    }

    onRejected: tdapi.busy = false
}
