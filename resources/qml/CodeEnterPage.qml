import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0

Item {
    id: root

    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin * 2

    property variant content: authorization.content

    property string phoneNumber: content.phoneNumber
    property variant type: content.type
    property variant nextType: content.nextType
    property int timeout: content.timeout

    Column {
        id: codeEnterColumn

        anchors.fill: parent
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

        LottieAnimation {
            id: lottieAnimation
            width: 160
            height: 160
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc:/tgs/AuthorizationStateWaitCode.tgs"

            onStatusChanged: {
                if (status === LottieAnimation.Ready)
                    lottieAnimation.play();
            }
        }

        Label {
            id: codeTitle
            text: getCodeTitle()
            wrapMode: Text.WordWrap
            font: UiConstants.TitleFont
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }

        Label {
            id: codeType
            text: getCodeSubtitle()
            wrapMode: Text.WordWrap
            font: UiConstants.SubtitleFont
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }

        TextField {
            id: code
            width: parent.width
            inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoPredictiveText
            placeholderText: qsTr("Code")
            platformSipAttributes: SipAttributes {
                actionKeyLabel: qsTr("Next")
                actionKeyHighlighted: true
            }

            onTextChanged: {
                if(text.length >= getCodeLength())
                    accept();
            }

        }

        Label {
            id: isNextTypeSms

            wrapMode: Text.WordWrap
            font.pixelSize: 24
            font.underline: true
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width

            color: "#0088cc"
            text: qsTr("DidNotGetTheCodeSms")

            visible: nextType.type === "Sms"

            MouseArea {
                anchors.fill: parent
                onClicked: authorization.resendCode()
            }
        }

        Row {
            id: codeTextRow
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 4

            Label {
                id: nextTypeLabel
                wrapMode: Text.WordWrap
                font: UiConstants.SubtitleFont
                width: parent.width
                text: getCodeNextTypeString()
            }

            Label {
                id: codeTimeText

                wrapMode: Text.WordWrap
                font: UiConstants.SubtitleFont
                width: parent.width
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
                codeTimeText.text = utils.formatTime(timeout / 1000);
                if (timeout === 0) {
                    codeExpireTimer.stop()
                    codeTextRow.visible = false;
                    authorization.resendCode()
                }
            }
        }
    }

    function getCodeTitle() {
        switch (type.type) {
        case "TelegramMessage":
            return qsTr("SentAppCodeTitle");
        case "Call":
        case "Sms":
            return qsTr("SentSmsCodeTitle");
        default:
            return qsTr("Title");
        }
    }

    function getCodeSubtitle() {
        switch (type.type) {
        case "Call":
            return qsTr("SentCallCode").arg(phoneNumber);
        case "FlashCall":
            return qsTr("SentCallOnly").arg(phoneNumber);
        case "Sms":
            return qsTr("SentSmsCode").arg(phoneNumber);
        case "TelegramMessage":
            return qsTr("SentAppCode");
        default:
            return "";
        }
    }

    function getCodeNextTypeString() {
        switch (nextType.type) {
        case "Call":
            return qsTr("CallText");
        case "Sms":
            return qsTr("SmsText");
        default:
            return "";
        }
    }

    function getCodeLength() {
        return type.length || 0;
    }

    onTimeoutChanged: {
        codeExpireTimer.start()
        codeTimeText.text = utils.formatTime(timeout / 1000)
    }

    QueryDialog {
        id: dialog
        titleText: qsTr("EditNumber")
        message: qsTr("EditNumberInfo").arg("+" + phoneNumber)
        acceptButtonText: qsTr("Edit")
        rejectButtonText: qsTr("Close")

        onAccepted: {
            sheet.state = "closed";
            authorization._destroy()
        }
    }

    Component.onCompleted: {
        code.forceActiveFocus();

        acceptButton.clicked.connect(function () { authorization.checkCode(code.text); })
        rejectButton.clicked.connect(function () { dialog.open(); })
    }
}
