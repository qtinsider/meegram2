import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    property string phoneNumber: ""
    property variant type
    property variant nextType: null
    property int timeout: 0

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
                text: locale.getString("YourCode") + locale.emptyString
                font.pixelSize: 40
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: flickable.width
            }

            // Code
            Column {
                id: codeEnterColumn

                width: parent.width
                spacing: 16

                Label {
                    id: codeTitle
                    text: getCodeTitle()
                }

                Column {
                    width: parent.width
                    spacing: 20

                    TextField {
                        id: code
                        width: parent.width
                        inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoPredictiveText
                        placeholderText: locale.getString("Code") + locale.emptyString

                        onTextChanged: {
                            if(text.length >= getCodeLength()) {
                                authorization.checkCode(code.text)
                            }
                        }
                    }

                    Label {
                        id: codeType
                        font.pixelSize: 24
                        width: parent.width
                        text: getCodeSubtitle()
                    }

                    Label {
                        id: isNextTypeSms

                        anchors.horizontalCenter: parent.horizontalCenter

                        font.pixelSize: 24
                        font.underline: true

                        color: "#0088cc"
                        text: locale.getString("DidNotGetTheCodeSms") + locale.emptyString

                        visible: nextType.type === "authenticationCodeTypeSms"

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
                            horizontalAlignment: Text.AlignHCenter
                            font.pointSize: 24
                            text: getCodeNextTypeString()
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
                            codeTimeText.text = authorization.formatTime(timeout / 1000);
                            if (timeout === 0) {
                                codeExpireTimer.stop()
                                codeTextRow.visible = false;
                                authorization.resendCode()
                            }
                        }
                    }
                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: locale.getString("Next") + locale.emptyString
                onClicked: authorization.checkCode(code.text)
            }
            ToolButton {
                text: locale.getString("Cancel") + locale.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    function getCodeTitle() {
        if (type.type === "authenticationCodeTypeTelegramMessage") {
            return locale.getString("SentAppCodeTitle");
        }
        if (type.type === "authenticationCodeTypeCall" || type.type === "authenticationCodeTypeSms") {
            return locale.getString("SentSmsCodeTitle");
        }

        return locale.getString("Title");
    }

    function getCodeSubtitle() {
        if (type.type === "authenticationCodeTypeCall") {
            return locale.getString("SentCallCode").arg(phoneNumber);
        }
        if (type.type === "authenticationCodeTypeFlashCall") {
            return locale.getString("SentCallOnly").arg(phoneNumber);
        }
        if (type.type === "authenticationCodeTypeSms") {
            return locale.getString("SentSmsCode").arg(phoneNumber);
        }
        if (type.type === "authenticationCodeTypeTelegramMessage") {
            return locale.getString("SentAppCode");
        }

        return "";
    }

    function getCodeNextTypeString() {
        if (nextType.type === "authenticationCodeTypeCall") {
            return locale.getString("CallText");
        }
        if (nextType.type === "authenticationCodeTypeSms") {
            return locale.getString("SmsText");
        }

        return "";
    }

    function getCodeLength() {
        if (type.type === "authenticationCodeTypeCall") {
            return type.length;
        }
        if (type.type === "authenticationCodeTypeSms") {
            return type.length;
        }
        if (type.type === "authenticationCodeTypeTelegramMessage") {
            return type.length;
        }

        return 0;
    }

    onTimeoutChanged: {
        codeExpireTimer.start()
        codeTimeText.text = authorization.formatTime(timeout / 1000)
    }
}
