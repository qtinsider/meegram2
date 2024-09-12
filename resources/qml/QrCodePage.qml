import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Item {
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin * 2

    property variant content: authorization.content

    Column {
        id: qrCodeLoginColumn
        anchors.fill: parent
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

        QrCode {
            id: qrCode
            anchors.horizontalCenter: parent.horizontalCenter
            text: content.link || ""
            width: 300
            height: 300
            size: 500
            foreground:  theme.inverted ? "#ffffff" : "#191919"

            BusyIndicator {
                anchors.centerIn: qrCode
                visible: running
                running: qrCode.text === ""
                platformStyle: BusyIndicatorStyle { size: "large" }
            }
        }

        Label {
            text: qsTr("Log in to Telegram by QR Code")
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            font: UiConstants.TitleFont
            width: parent.width
        }

        Column {
            spacing: UiConstants.IndentDefault
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width

            Label {
                text: qsTr("Open Telegram on your phone")
                font: UiConstants.SubtitleFont
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }

            Label {
                text: qsTr("Go to <b>Settings</b> > <b>Devices</b> > <b>Link Desktop Device</b>")
                font: UiConstants.SubtitleFont
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }

            Label {
                text: qsTr("Point your phone at this screen to confirm login")
                font: UiConstants.SubtitleFont
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                width: parent.width
            }
        }

        Button {
            text: qsTr("Log in by phone Number")
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: { authorization.state = "phone_number" }
        }
    }

    Component.onCompleted: {
        authorization.requestQrCode()
        rejectButton.clicked.connect(function() { sheet.state = "closed"; })
    }
}
