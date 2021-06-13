import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 0.1
import "components"

PageStackWindow {
    id: appWindow

    property bool isAuthorized: tdapi.authorizationState == TdApi.AuthorizationStateReady
    property variant authorizationStateData: null

    property QtObject icons: Icons {}

    property bool isPortrait: (screen.currentOrientation === Screen.Landscape) ? false : true

    initialPage: MainPage {}

    onOrientationChangeFinished: showStatusBar = isPortrait

    Connections {
        target: tdapi
        onError: {
            banner.text = data.message

            if (banner.text)
                banner.show()
        }
        onUpdateAuthorizationState: {
            authorizationStateData = authorizationState
        }
        onUpdateChatFilters: {
            if (chatFilters === null)
                return;

            chatFilterModel.clear();

            chatFilterModel.append({name: qsTr("FilterAllChats"), value: 1});

            for (var i in chatFilters) {
                chatFilterModel.append({name: chatFilters[i].title, value: chatFilters[i].id})
            }
        }
    }

    ListModel {
        id: chatFilterModel
    }

    InfoBanner {
        id: banner
        y: 36
        z: 100
    }

    Component {
        id: contextMenuComponent

        ContextMenu {
            id: contextMenu

            property string chatId: ""
            property variant chatList: null
            property bool isPinned: false

            content: MenuLayout {
                MenuItem {
                    text: !isPinned ? qsTr("PinToTop") : qsTr("UnpinFromTop")
                    onClicked: {
                        tdapi.toggleChatIsPinned(chatList, chatId, !isPinned)
                        myChatModel.refresh()
                    }
                }
            }
        }
    }

    function createChatContextMenu(chatId, chatList, isPinned) {
        var p = { chatId: chatId, chatList: chatList, isPinned: isPinned };
        var menu = contextMenuComponent.createObject(pageStack.currentPage, p);
        menu.statusChanged.connect(function() {
            if (menu.status === DialogStatus.Closed)
                menu.destroy(250);
        });
        menu.open();
    }

    Rectangle {
        z: 2
        anchors.fill: parent
        visible: pageStack.busy
        color: "#70000000"

        BusyIndicator  {
            anchors {
                horizontalCenter: parent.horizontalCenter
                verticalCenter: parent.verticalCenter
            }
            running: true
            platformStyle: BusyIndicatorStyle { size: "large" }
        }
    }

    Component.onCompleted: tdapi.listen()
}
