import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    TopBar {
        id: header
        text: "Archived Chats"
        barColor: "#424345"

        MouseArea {
            anchors.fill: parent
        }
    }

    ListView {
        id: listView

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: header.bottom
        }

        cacheBuffer: listView.height * 2

        delegate: Item {
            height: 88; width: parent.width
            
            BorderImage {
                id: background
                anchors.fill: parent
                visible: mouseArea.pressed
                source: "image://theme/meegotouch-panel-background-pressed"
            }

            Image {
                id: photo

                width: 64
                height: 64
                anchors {
                    left: parent.left
                    leftMargin: 16
                    verticalCenter: parent.verticalCenter
                }
                source: model.photo ? "image://telegram/" + model.photo : "image://theme/icon-l-content-avatar-placeholder"
            }

            Item {
                id: row1

                width: parent.width - photo.width - 44
                height: 45
                anchors {
                    left: photo.right
                    leftMargin: 16
                    rightMargin: 16
                }

                Label {
                    id: title
                    width: parent.width - date.width
                    anchors.verticalCenter: parent.verticalCenter
                    font.bold: true
                    font.pixelSize: 26
                    color: mouseArea.pressed ? "#797979" : "#282828"
                    elide: Text.ElideRight
                    text: model.title
                }

                Label {
                    id: date

                    anchors {
                        right: parent.right
                        verticalCenter: parent.verticalCenter
                    }
                    font.weight: Font.Light
                    font.pixelSize: 20
                    color: mouseArea.pressed ? "#797979" : "#505050"
                    text: model.lastMessageDate
                }
            }

            Item {
                height: 30
                width: parent.width - photo.width - 44
                anchors { left: row1.left; top: row1.bottom }
                
                Label {
                    id: lastMessage
                    width: parent.width - loader.width
                    anchors.verticalCenter: parent.verticalCenter
                    font.weight: Font.Light
                    font.pixelSize: 22
                    color: mouseArea.pressed ? "#797979" : "#505050"
                    elide: Text.ElideRight
                    text: model.lastMessageContent
                }

                Loader {
                    id: loader

                    anchors { verticalCenter: parent.verticalCenter; right: parent.right }

                    sourceComponent: model.unreadCount ? countBubble : moreIndicator
                }

                Component {
                    id: countBubble

                    CountBubble {
                        largeSized: true
                        value: model.unreadCount
                    }
                }

                Component {
                    id: moreIndicator

                    MoreIndicator {}
                }
            }

            MouseArea {
                id: mouseArea
                anchors.fill: parent

                onClicked: {
                    pageStack.push(Qt.createComponent("MessagePage.qml"), { chat: ChatStore.get(model.id) })
                }
                onPressAndHold: contextMenu.open()
            }
        }

        model: myChatModel
    }

    ScrollDecorator {
        flickableItem: listView
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }

    Component.onDestruction: {
        myChatModel.chatList = TdApi.ChatListMain
    }
}
