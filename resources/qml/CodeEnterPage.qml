import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    property int length
    property int timeout

    // private api
    property alias __title: codeTitle.text
    property alias __subtitle: codeType.text
    property alias __isNextTypeSms: isNextTypeSms.visible
    property alias __nextTypeString: nextTypeLabel.text

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
                text: qsTr("YourCode")
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
                            if(text.length >= length) {
                                tdapi.checkCode(code.text)
                            }
                        }
                    }

                    Label {
                        id: codeType
                        font.pixelSize: 24
                        width: parent.width
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
                            onClicked: tdapi.resendCode()
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
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: qsTr("Next")
                onClicked: tdapi.checkCode(code.text)
            }
            ToolButton {
                text: qsTrId("Cancel")
                onClicked: {
                    pageStack.pop()
                }
            }
        }
    }

    function onCodeRequested(codeInfo) {
        root.length = codeInfo.length
        root.timeout = codeInfo.timeout * 1000
        root.__title = codeInfo.title
        root.__subtitle = codeInfo.subtitle
        root.__nextTypeString = codeInfo.nextTypeString
        root.__isNextTypeSms = codeInfo.isNextTypeSms
    }

    function onPasswordRequested(passwordInfo) {
        var component = Qt.createComponent("PasswordPage.qml")
        if (component.status === Component.Ready) {
            pageStack.replace(component, { passwordHint: passwordInfo.passwordHint })
        } else {
            console.debug("Error loading component:", component.errorString());
        }
    }

    function onRegistrationRequested(termsOfService) {
        var component = Qt.createComponent("SignUpPage.qml")
        if (component.status === Component.Ready) {
            pageStack.replace(component, { termsOfServiceString: termsOfService.text })
        } else {
            console.debug("Error loading component:", component.errorString());
        }
    }

    onTimeoutChanged: {
        codeExpireTimer.start()
        codeTimeText.text = Utils.formatTime(timeout / 1000)
    }

    Component.onCompleted: {
        tdapi.codeRequested.connect(onCodeRequested)
        tdapi.passwordRequested.connect(onPasswordRequested)
        tdapi.registrationRequested.connect(onRegistrationRequested)
        tdapi.error.connect(function(error) { showInfoBanner(error.message) })
    }

    Component.onDestruction: {
        tdapi.codeRequested.disconnect(onCodeRequested)
        tdapi.passwordRequested.disconnect(onPasswordRequested)
        tdapi.registrationRequested.disconnect(onRegistrationRequested)
        tdapi.error.disconnect(function(error) { showInfoBanner(error.message) })
    }
}
