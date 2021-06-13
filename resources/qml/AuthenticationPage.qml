import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 0.1

Sheet {
    id: sheet

    property string phoneNumber: ""
    property int timeout: 0

    property string passwordHint;
    property bool hasRecoveryEmailAddress;
    property string recoveryEmailAddressPattern;

    property bool loading: false

    property alias __codeTitle: codeTitle.text
    property alias __codeTypeString: codeType.text
    property alias __isNextTypeSms: isNextTypeSms.visible
    property alias __nextTypeString: nextTypeLabel.text

    property int __codeLength

    rejectButtonText: "Cancel"

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

                    visible: (sheet.state != "Phone") ? false : true

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
                            enabled: !loading
                            opacity: enabled ? 1.0 : 0.5
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
                        enabled: !loading
                        text: qsTr("Next")
                        width: parent.width / 2
                        onClicked: {
                            if (phoneNumber.text.length > 0) {
                                tdapi.setPhoneNumber(countryCodeButton.text + phoneNumber.text)
                                loading = true
                            }
                        }

                        BusyIndicator {
                            visible: loading
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

                    visible: (sheet.state != "Code") ? false : true

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
                                if(text.length >= __codeLength) {
                                    tdapi.checkCode(code.text)

                                    code.focus = false
                                    code.text = ""

                                    loading = true
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
                            enabled: !loading
                            text: qsTr("Next")
                            width: parent.width / 2
                            onClicked: {
                                tdapi.checkCode(code.text)

                                code.focus = false
                                code.text = ""
                                loading = true
                            }

                            BusyIndicator {
                                visible: loading
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

                                    loading = true
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

                                timeout = timeout - 1000
                                codeTimeText.text = Utils.formatTime(timeout / 1000)
                                if (timeout === 0) {
                                    codeExpireTimer.stop()
                                    codeTextRow.visible = false
                                    tdapi.resendCode()

                                    loading = true
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

                    visible: (sheet.state != "Password") ? false : true


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
                        enabled: !loading
                        text: qsTr("Next")
                        width: parent.width / 2
                        onClicked: {
                            if (password.text.length > 0) {
                                tdapi.checkPassword(password.text)
                                password.focus = false
                                password.text = ""

                                loading = true
                            }
                        }

                        BusyIndicator {
                            visible: loading
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

                    visible: (sheet.state != "Registration") ? false : true

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
                        enabled: !loading
                        width: parent.width / 2
                        text: qsTr("Done")
                        onClicked: {
                            tdapi.registerUser(firstName.text, lastName.text)

                            firstName.focus = false
                            lastName.focus = false

                            loading = true
                        }

                        BusyIndicator {
                            visible: loading
                            running: visible
                            anchors.centerIn: parent
                        }
                    }
                }

            }
        }
    }

    SelectionDialog {
        id: selectionDialog
        titleText: qsTr("ChooseCountry")
        selectedIndex: myCountryModel.defaultIndex
        model: myCountryModel
    }

    Connections {
        target: tdapi
        onUpdateAuthorizationState: {
            switch (authorizationState['@type']) {
            case 'authorizationStateWaitPhoneNumber': {
                state = "Phone"

                loading = false
                break;
            }
            case 'authorizationStateWaitCode': {
                state = "Code"

                loading = false
                var codeInfo = authorizationState.code_info

                __codeTitle = internal.getTitle(codeInfo);
                __codeTypeString = internal.getSubtitle(codeInfo);
                __isNextTypeSms = internal.isNextTypeSms(codeInfo);
                __nextTypeString = internal.getNextTypeString(codeInfo);

                __codeLength = internal.getCodeLength(codeInfo);

                phoneNumber = codeInfo.phone_number;
                timeout = codeInfo.timeout * 1000;

                loading = false
                break;
            }
            case 'authorizationStateWaitRegistration': {
                state = "Registration"

                loading = false
                break;
            }
            case 'authorizationStateWaitPassword': {
                state = "Password"

                passwordHint = authorizationState.password_hint;
                hasRecoveryEmailAddress = authorizationState.has_recovery_email_address;
                recoveryEmailAddressPattern = authorizationState.recovery_email_address_pattern;


                loading = false
                break;
            }
            }
        }
        onAuthorizationStateReady:  sheet.accept()
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

    onTimeoutChanged: {
        codeExpireTimer.start()
        codeTimeText.text = Utils.formatTime(timeout / 1000)
    }

    QtObject {
        id: internal

        function isNextTypeSms(codeInfo) {
            if (!codeInfo) return false;
            if (!codeInfo.next_type) return false;

            if (codeInfo.next_type['@type'] === "authenticationCodeTypeSms")
                return true;

            return false;
        }

        function getNextTypeString(codeInfo) {
            if (!codeInfo) return "";
            if (!codeInfo.next_type) return "";

            switch (codeInfo.next_type['@type']) {
            case 'authenticationCodeTypeCall':
                return qsTr("CallText");
            case 'authenticationCodeTypeSms':
                return qsTr("SmsText");
            }
            return "";
        }

        function getTitle(codeInfo) {
            if (!codeInfo) return "";
            if (!codeInfo.type) return "";

            switch (codeInfo.type['@type']) {
            case 'authenticationCodeTypeTelegramMessage': {
                return qsTr("SentAppCodeTitle");
            }
            case 'authenticationCodeTypeCall':
            case 'authenticationCodeTypeSms': {
                return qsTr("SentSmsCodeTitle");
            }
            }

            return "Title";
        }

        function getCodeLength(codeInfo) {
            if (!codeInfo) return 0;
            if (!codeInfo.type) return 0;

            switch (codeInfo.type['@type']) {
            case 'authenticationCodeTypeCall': {
                return codeInfo.type.length;
            }
            case 'authenticationCodeTypeFlashCall': {
                return 0;
            }
            case 'authenticationCodeTypeSms': {
                return codeInfo.type.length;
            }
            case 'authenticationCodeTypeTelegramMessage': {
                return codeInfo.type.length;
            }
            }

            return 0;
        }

        function getSubtitle(codeInfo) {
            if (!codeInfo) return "";
            if (!codeInfo.type) return "";

            var phoneNumber = codeInfo.phone_number
            switch (codeInfo.type['@type']) {
            case 'authenticationCodeTypeCall': {
                return qsTr("SentCallCode").arg(phoneNumber);
            }
            case 'authenticationCodeTypeFlashCall': {
                return qsTr("SentCallOnly").arg(phoneNumber);
            }
            case 'authenticationCodeTypeSms': {
                return qsTr("SentSmsCode").arg(phoneNumber);
            }
            case 'authenticationCodeTypeTelegramMessage': {
                return qsTr("SentAppCode");
            }
            }

            return "";
        }
        
        function showTermsOfService() {
            var dialog = termsOfServiceComponent.createObject(sheet, { termsOfService: termsOfServiceString });
            dialog.open();
        }
    }

    Component.onCompleted: {
        switch (tdapi.authorizationState) {
        case TdApi.AuthorizationStateWaitPhoneNumber: {
            state = "Phone"

            break;
        }
        case TdApi.AuthorizationStateWaitCode: {
            state = "Code"


            var codeInfo = authorizationStateData.code_info;

            __codeTitle = internal.getTitle(codeInfo);
            __codeTypeString = internal.getSubtitle(codeInfo);
            __isNextTypeSms = internal.isNextTypeSms(codeInfo);
            __nextTypeString = internal.getNextTypeString(codeInfo);

            phoneNumber = codeInfo.phone_number;
            timeout = codeInfo.timeout * 1000;

            __codeLength = internal.getCodeLength(codeInfo);

            loading = false

            break;
        }
        case TdApi.AuthorizationStateWaitRegistration: {
            state = "Registration"

            loading = false

            break;
        }
        case TdApi.AuthorizationStateWaitPassword: {
            state = "Password"

            passwordHint = authorizationStateData.password_hint;
            hasRecoveryEmailAddress = authorizationStateData.has_recovery_email_address;
            recoveryEmailAddressPattern = authorizationStateData.recovery_email_address_pattern;

            loading = false

            break;
        }
        }
    }

}
