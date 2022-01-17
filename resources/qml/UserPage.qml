import QtQuick 1.1
import com.nokia.meego 1.1

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    property variant chat: null
    property variant user: null

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }

    ButtonStyle {
        id: buttonStyleTop
        property string __invertedString: theme.inverted ? "-inverted" : ""
        pressedBackground: "image://theme/color3-meegotouch-button-background-pressed-vertical-top"
        checkedBackground: "image://theme/color3-meegotouch-button-background-selected-vertical-top"
        disabledBackground: "image://theme/color3-meegotouch-button" + __invertedString + "-background-disabled-vertical-top"
        checkedDisabledBackground: "image://theme/color3-meegotouch-button" + __invertedString + "-background-disabled-selected-vertical-top"
    }

    ButtonStyle {
        id: buttonStyleCenter
        property string __invertedString: theme.inverted ? "-inverted" : ""
        pressedBackground: "image://theme/color3-meegotouch-button-background-pressed-vertical-center"
        checkedBackground: "image://theme/color3-meegotouch-button-background-selected-vertical-center"
        disabledBackground: "image://theme/color3-meegotouch-button" + __invertedString + "-background-disabled-vertical-center"
        checkedDisabledBackground: "image://theme/color3-meegotouch-button" + __invertedString + "-background-disabled-selected-vertical-center"
    }

    ButtonStyle {
        id: buttonStyleBottom
        property string __invertedString: theme.inverted ? "-inverted" : ""
        pressedBackground: "image://theme/color3-meegotouch-button-background-pressed-vertical-bottom"
        checkedBackground: "image://theme/color3-meegotouch-button-background-selected-vertical-bottom"
        disabledBackground: "image://theme/color3-meegotouch-button" + __invertedString + "-background-disabled-vertical-bottom"
        checkedDisabledBackground: "image://theme/color3-meegotouch-button" + __invertedString + "-background-disabled-selected-vertical-bottom"
    }

    Flickable {
        flickableDirection: Flickable.VerticalFlick
        anchors.fill: parent
        contentHeight: contentItem.childrenRect.height + 50

        width: parent.width


        Column {
                id: column1
                width: parent.width -32
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 16
                anchors.rightMargin: 16
                anchors.topMargin: 12
                spacing: 12

                Row {
                    width: parent.width
                    height: column2.heigth + 48
                    spacing: 10

                    MaskedItem {
                        id: maskedItem

                        anchors.verticalCenter: parent.verticalCenter
                        height: 80
                        width: 80

                        mask: Image {
                            sourceSize.width: maskedItem.width
                            sourceSize.height: maskedItem.height
                            width: maskedItem.width
                            height: maskedItem.height
                            source: "qrc:/images/avatar-image-mask.png"
                        }

                        Image {
                            id: profilePhotoImage
                            anchors.fill: parent
                            cache:  false
                            smooth: true
                            fillMode: Image.PreserveAspectCrop
                            clip: true
                            source: "image://theme/icon-l-content-avatar-placeholder"
                        }

                        MouseArea {
                            anchors.fill: parent

                            onClicked: console.log("Photo clicked ...")
                        }
                    }

                    Column {
                        id: column2

                        anchors.verticalCenter: parent.verticalCenter

                        width: parent.width - maskedItem.size -10

                        Label {
                            text: "Todo"
                            font.bold: true
                            font.pixelSize: 26
                            width: parent.width
                            anchors.horizontalCenter: parent.horizontalCenter
                            wrapMode: Text.WordWrap
                            maximumLineCount: 2
                        }

                        Label {
                            anchors.horizontalCenter: parent.horizontalCenter
                            width: parent.width
                            text: "online"
                            wrapMode: Text.WordWrap
                            font.pixelSize: 22
                            color: "gray"
                        }
                    }
                }

                Rectangle {
                    width: parent.width
                    height: 2

                    color:"transparent"

                    Rectangle {
                        id: sortingDivisionLine
                        anchors.verticalCenter: parent.verticalCenter
                        width: parent.width
                        height: 1
                        color: "gray"
                        opacity: 0.3
                    }

                    Rectangle {
                        anchors.top: sortingDivisionLine.bottom
                        width: parent.width
                        height: 1
                        color: "white"
                        opacity: 0.5
                    }

                }
            }
    }
}
