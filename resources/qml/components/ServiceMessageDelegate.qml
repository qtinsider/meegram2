import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0
import "UIConstants.js" as UI

Item {
    id: root

    property variant chat: app.getChat(model.chatId)
    property variant message: myMessageModel.getMessage(model.id)

    property string labelContent: message.getServiceContent(true)

    anchors.horizontalCenter: parent.horizontalCenter
    width: parent.width - UiConstants.DefaultMargin * 2
    height: column.height

    Column {
        id: column
        width: parent.width
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

        Item {
            id: container
            anchors.margins: UiConstants.DefaultMargin
            anchors.horizontalCenter: parent.horizontalCenter
            height: label.implicitHeight + UI.PADDING_LARGE * 4
            width: Math.min(label.implicitWidth + UI.PADDING_XLARGE * 2, parent.width - UiConstants.DefaultMargin * 2)

            BorderImage {
                anchors.fill: parent
                border { left: 22; top: 22; right: 22; bottom: 22 }
                source: "qrc:/images/notification.png"
                smooth: true
            }

            Label {
                id: label
                anchors.centerIn: parent
                horizontalAlignment: Text.AlignHCenter
                color: "gray"
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                font.pixelSize: 18
                x: UI.PADDING_XLARGE
                y: (UI.FIELD_DEFAULT_HEIGHT - font.pixelSize) / 2
                width: root.width - UI.PADDING_XLARGE * 2
                text: labelContent
                onLinkActivated: { console.log(link)}
            }
        }
    }
}
