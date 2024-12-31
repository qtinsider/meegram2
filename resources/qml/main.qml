import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

PageStackWindow {
    id: appWindow

    property variant client: appManager.client
    property variant authorization: appManager.authorization
    property variant locale: appManager.locale
    property variant settings: appManager.settings
    property variant storageManager: appManager.storageManager
    property variant chatManager: appManager.chatManager

    property bool isPortrait: screen.currentOrientation !== Screen.Landscape

    initialPage: Component { MainPage {} }

    onOrientationChangeFinished: showStatusBar = isPortrait

    Icons { id: icons }

    InfoBanner {
        id: banner
        y: 36
        z: 100
    }

    Connections {
        target: settings
        onInvertedThemeChanged: theme.inverted = settings.invertedTheme
    }

    function showInfoBanner(message) {
        banner.text = message
        banner.show()
    }

    function openChat(chatId) {
        var component = Qt.createComponent("ChatPage.qml");
        if (component.status === Component.Ready) {
            chatManager.openChat(chatId);
            pageStack.push(component);
        } else {
            console.debug("Error loading component:", component.errorString());
        }
    }

    Component.onCompleted: theme.inverted = settings.invertedTheme
}
