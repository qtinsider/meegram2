import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

PageStackWindow {
    id: appWindow

    property bool isPortrait: screen.currentOrientation !== Screen.Landscape

    initialPage: Component { MainPage {} }

    onOrientationChangeFinished: showStatusBar = isPortrait

    Icons { id: icons }
    UIStyles { id: uiStyles }
    Authorization { id: authorization }

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

    function openChat(chat) {
        var component = Qt.createComponent("ChatPage.qml");

        if (component.status === Component.Ready) {
            pageStack.push(component, { chat: chat });
        } else {
            console.debug("Error loading component:", component.errorString());
        }
    }

    Component.onCompleted: theme.inverted = false /*settings.invertedTheme*/
}
