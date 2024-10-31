import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    property variant chatPosition: null

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: "ArchivedChats"
        isArchived: true
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

        delegate: ChatItem {
            onPressAndHold: {
                var chat = app.getChat(model.id)
                chatPosition = myChatModel.getChatPosition(chat, chatList);
                contextMenu.open();
            }
        }

        model: myChatModel
    }

    Column {
        anchors.centerIn: listView
        anchors.margins: UiConstants.DefaultMargin * 2
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

        width: parent.width

        visible: myChatModel.count === 0 && !populateTimer.running && !myChatModel.loading

        LottieAnimation {
            id: lottieAnimation
            anchors.horizontalCenter: parent.horizontalCenter
            width: 160
            height: 160
            source: "qrc:/tgs/Folders_2.tgs"
            onStatusChanged: {
                if (status === LottieAnimation.Ready)
                    lottieAnimation.play();
            }
        }

        Label {
            text: qsTr("FilterNoChatsToDisplay")
            wrapMode: Text.WordWrap
            color: "gray"
            font.pixelSize: 42
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }
    }

    BusyIndicator {
        anchors.centerIn: listView
        running: visible
        visible: populateTimer.running || myChatModel.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }

    ChatModel {
        id: myChatModel

        list: ChatList { type: ChatList.Archive }

        onLoadingChanged: {
            if (!loading)
                populateTimer.restart()
        }
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {
            MenuItem {
                text: chatPosition && chatPosition.isPinned
                      ? qsTr("UnpinFromTop")
                      : qsTr("PinToTop")
                onClicked: {
                    if (chatPosition)
                        myChatModel.toggleChatIsPinned(chatPosition.id, !chatPosition.isPinned)
                }
            }
        }
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

    Timer {
        id: populateTimer
        interval: 200
        repeat: false
        onTriggered: myChatModel.populate()
    }

    Component.onCompleted: { myChatModel.refresh() }
}
