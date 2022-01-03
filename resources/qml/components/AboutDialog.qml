import QtQuick 1.1
import com.nokia.meego 1.1

Dialog {
    id: root

    width: parent.width
    content: Item {
        height: column.height
        anchors {
            left: parent.left
            right: parent.right
            margins: 16
        }

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
            spacing: 16

            Label {
                id: titleLabel

                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                font.bold: true
                font.pixelSize: 32
                color: "white"
                text: "About MeeGram"
            }
            
            Label {
                id: versionLabel

                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                color: "#757575"
                text: "Version " + AppVersion
            }

            Label {
                id: aboutLabel

                width: parent.width
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: "white"
                text: "Telegram client for Nokia N9 based on <font color=\"#d7ccc8\">TDLib</font>"
            }

            Label {
                width: parent.width
                wrapMode: Text.WordWrap
                horizontalAlignment: Text.AlignHCenter
                color: "white"
                text: "Copyright \u00a9 2022 Chukwudi Nwutobo<br/>Contact: <a href='mailto:nwutobo@outlook.com?subject=Meegram'>nwutobo@outlook.com</a>"
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
}
