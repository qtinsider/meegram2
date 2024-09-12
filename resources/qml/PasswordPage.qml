import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0

Item {
    id: root

    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin * 2

    property variant content: authorization.content

    property string passwordHint: content.passwordHint
    property bool hasRecoveryEmailAddress: content.hasRecoveryEmailAddress
    property string recoveryEmailAddressPattern: content.recoveryEmailAddressPattern

    Column {
        id: enterPasswordColumn

        anchors.fill: parent
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

        LottieAnimation {
            id: lottieAnimation
            width: 160
            height: 160
            loopCount: 1
            anchors.horizontalCenter: parent.horizontalCenter
            source: "qrc:/tgs/TwoFactorSetupIntro.tgs"

            onStatusChanged: {
                if (status === LottieAnimation.Ready)
                    lottieAnimation.play();
            }
        }

        Label {
            text: qsTr("YourPassword")
            wrapMode: Text.WordWrap
            font: UiConstants.TitleFont
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }

        Label {
            text: qsTr("LoginPasswordText")
            wrapMode: Text.WordWrap
            font: UiConstants.SubtitleFont
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }

        Item {
            height: 3
        }

        TextField {
            id: password
            width: parent.width
            echoMode: TextInput.Password
            placeholderText:  passwordHint || qsTr("Password")
            platformSipAttributes: SipAttributes {
                actionKeyLabel: qsTr("Next")
                actionKeyHighlighted: true
            }
        }

        Label {
            wrapMode: Text.WordWrap
            font: UiConstants.SubtitleFont
            text: qsTr("ForgotPassword")
            width: parent.width
            color: theme.selectionColor // "#ffcc00"
        }
    }

    Component.onCompleted: {
        password.forceActiveFocus();

        acceptButton.clicked.connect(function () { authorization.checkPassword(password.text); })
     }
}


