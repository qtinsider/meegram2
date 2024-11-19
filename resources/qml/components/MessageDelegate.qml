import QtQuick 1.1
import com.nokia.meego 1.1

Item {
    height: loader.y + loader.height
    width: listView.width

    Loader {
        id: loader
        width: listView.width
        sourceComponent: model.isService ? serviceMessageComponent  : deleglateChooser.get(model.contentType)
    }

    Component {
        id: serviceMessageComponent

        ServiceMessageDelegate {}
    }

    Component {
        id: textMessageComponent

        MessageBubble {
            childrenWidth: messageText.paintedWidth

            content: Label {
                id: messageText
                text: model.content.formattedText
                textFormat: Text.RichText
                color: model.isOutgoing ? "white" : "black"
                width: isPortrait ? 380 : 754
                wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                anchors {
                    left: parent.left
                    leftMargin: model.isOutgoing ? 80 : 20
                }
                font.pixelSize: 23
                horizontalAlignment: model.isOutgoing ? Text.AlignRight : Text.AlignLeft
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }

    Component {
        id: notSupportedMessageComponent

        MessageBubble {
            childrenWidth: notSupportedMessage.paintedWidth

            content: Label {
                id: notSupportedMessage
                anchors {
                    left: parent.left
                    leftMargin: model.isOutgoing ? 80 : 20
                }
                width: isPortrait ? 380 : 754
                font {
                    bold: true
                    pixelSize: 23
                }
                horizontalAlignment: model.isOutgoing ? Text.AlignRight : Text.AlignLeft
                wrapMode: Text.Wrap
                color: model.isOutgoing ? "white" : "black"
                text: "The message is not supported on MeeGram yet"
            }
        }
    }

    QtObject {
        id: deleglateChooser
        function get(contentType) {
            switch (contentType) {
            case "messageText":
                return textMessageComponent;
            default:
                return notSupportedMessageComponent;
            }
        }
    }
}
