import QtQuick 1.1
import com.nokia.meego 1.1

SelectionDialog {
    id: selectionDialog

    delegate: Component {
        Item {
            id: delegateItem

            property bool selected: index === selectionDialog.selectedIndex
            property Style platformStyle: SelectionDialogStyle {}

            height: platformStyle.itemHeight
            anchors.left: parent.left
            anchors.right: parent.right

            MouseArea {
                id: delegateMouseArea
                anchors.fill: parent
                onPressed: {
                    selectionDialog.selectedIndex = index
                }
                onClicked: {
                    selectionDialog.accept()
                }
            }

            Rectangle {
                id: backgroundRect
                anchors.fill: parent
                color: delegateItem.selected ? platformStyle.itemSelectedBackgroundColor : platformStyle.itemBackgroundColor
            }

            BorderImage {
                id: background
                anchors.fill: parent
                border { left: 22; top: 22; right: 22; bottom: 22 }
                source: delegateMouseArea.pressed ? platformStyle.itemPressedBackground :
                                                    delegateItem.selected ? platformStyle.itemSelectedBackground :
                                                                            platformStyle.itemBackground
            }

            Text {
                id: countryName
                anchors {
                    left: parent.left
                    leftMargin: platformStyle.itemLeftMargin
                }
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width - platformStyle.itemLeftMargin - platformStyle.itemRightMargin - countryCode.width
                font: platformStyle.itemFont
                color: delegateItem.selected ? platformStyle.itemSelectedTextColor : platformStyle.itemTextColor
                elide: Text.ElideRight
                horizontalAlignment: Qt.AlignLeft
                text: model.name
            }

            Text {
                id: countryCode
                anchors {
                    right: parent.right
                    rightMargin: platformStyle.itemRightMargin
                }
                anchors.verticalCenter: parent.verticalCenter
                font: platformStyle.itemFont
                color: theme.selectionColor
                text: "+" + model.code
                horizontalAlignment: Qt.AlignRight
            }
        }
    }
}
