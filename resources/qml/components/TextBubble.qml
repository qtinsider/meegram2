import QtQuick 1.1
import com.nokia.meego 1.1

MessageBubble {

    property string message

    childrenWidth: messageText.paintedWidth

    content: Text {
        id: messageText

        text: message
        color: isServiceMessage ? "gray" : isOutgoing ? "black" : "white"
        width: screen.currentOrientation === Screen.Portrait ? 380 : 754
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        anchors {
            left: parent.left
            leftMargin: isServiceMessage ? 50 : isOutgoing ? 20 : 80
        }
        font.weight: Font.Light
        font.pixelSize: isServiceMessage ? 18 : 23
        horizontalAlignment: isServiceMessage ? Text.AlignHCenter : isOutgoing ? Text.AlignLeft : Text.AlignRight
        onLinkActivated: Qt.openUrlExternally(link)
	}
}
