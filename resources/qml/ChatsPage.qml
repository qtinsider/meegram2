import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    property variant chatFolderModel: chatManager.folderModel
    property variant mainChatModel: chatManager.mainModel
    property variant folderChatModels: chatManager.folderModels

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: "MeeGram"
    }

    Loader {
        id: layoutLoader
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        sourceComponent: folderChatModels.count === 0 ? chatLayoutComponent : chatTabsLayoutComponent
    }

    Component {
        id: chatLayoutComponent

        Item {
            anchors.fill: parent

            ChatListView { model: mainChatModel }
        }
    }

    Component {
        id: chatTabsLayoutComponent

        Item {
            id: chatTabsLayout

            property alias tabFlickable: tabRowFlickable

            anchors.fill: parent
            Flickable {
                id: tabRowFlickable
                contentWidth: tabButtonRow.width
                height: 48
                flickableDirection: Flickable.HorizontalFlick
                clip: true
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                Row {
                    id: tabButtonRow
                    property int buttonMargin: 1
                    anchors.verticalCenter: parent.verticalCenter

                    TabButton {
                        id: allChatsTabButton
                        text: qsTr("FilterAllChats")
                        tab: allChatsTab
                        checked: tabGroup.currentTab === allChatsTab
                        onClicked: {
                            tabGroup.currentTab = allChatsTab;
                            root.ensureVisible(allChatsTabButton);
                        }
                        width: allChatsTabText.width + tabButtonRow.buttonMargin * 2

                        Text {
                            id: allChatsTabText
                            text: parent.text
                            visible: false // Invisible, used only for width calculation
                        }
                    }

                    Repeater {
                        model: chatFolderModel

                        TabButton {
                            id: folderTabButton
                            text: model.name
                            checked: tabGroup.currentTab === tabGroup.children[index + 1]
                            onClicked: {
                                tabGroup.currentTab = tabGroup.children[index + 1];
                                root.ensureVisible(folderTabButton);
                            }
                            width: folderTabText.width + tabButtonRow.buttonMargin * 2

                            Text {
                                id: folderTabText
                                text: parent.text
                                visible: false // Invisible, used only for width calculation
                            }
                        }
                    }
                }

                function smoothScrollTo(targetX) {
                    contentXAnimation.to = targetX;
                    contentXAnimation.running = true;
                }

                NumberAnimation {
                    id: contentXAnimation
                    target: tabRowFlickable
                    property: "contentX"
                    duration: 300
                    easing.type: Easing.InOutQuad
                }
            }

            TabGroup {
                id: tabGroup
                currentTab: allChatsTab
                anchors {
                    top: tabRowFlickable.bottom
                    left: parent.left
                    right: parent.right
                    bottom: parent.bottom
                }

                Item {
                    id: allChatsTab
                    anchors.fill: parent

                    ChatListView {
                        model: mainChatModel
                        clip: true
                    }
                }

                Repeater {
                    model: folderChatModels

                    Item {
                        id: folderTabPage
                        anchors.fill: parent

                        ChatListView {
                            model: modelData
                            clip: true
                        }
                    }
                }

                onCurrentTabChanged: {

                }
            }
        }
    }

    Menu {
        id: myMenu

        MenuLayout {
            MenuItem {
                text: qsTr("SavedMessages")
                onClicked: appWindow.openChat(storageManager.myId())
            }
            MenuItem {
                text: qsTr("ArchivedChats")
                onClicked: pageStack.push(Qt.createComponent("ArchivedChatPage.qml"), { model: chatManager.archivedModel })
            }
            MenuItem {
                text: qsTr("SETTINGS")
                onClicked: pageStack.push(Qt.createComponent("SettingsPage.qml"))
            }
            MenuItem {
                text: "About"
                onClicked: aboutDialog.open()
            }
        }
    }

    AboutDialog {
        id: aboutDialog
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-view-menu"
            anchors.right: (parent !== undefined) ? parent.right : undefined
            onClicked: (myMenu.status === DialogStatus.Closed) ? myMenu.open() : myMenu.close()
        }
    }

    function ensureVisible(item) {
        if (layoutLoader.item && layoutLoader.item.tabFlickable) {
            var flickable = layoutLoader.item.tabFlickable;
            var targetContentX = item.x + item.width / 2 - flickable.width / 2;
            var clampedContentX = Math.max(0, Math.min(targetContentX, flickable.contentWidth - flickable.width));
            flickable.smoothScrollTo(clampedContentX);
        }
    }
}
