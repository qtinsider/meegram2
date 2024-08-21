import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

PageStackWindow {
    id: appWindow

    property QtObject icons: Icons {}
    property bool isPortrait: screen.currentOrientation !== Screen.Landscape

    initialPage: Component { MainPage {} }

    onOrientationChangeFinished: showStatusBar = isPortrait

    InfoBanner {
        id: banner
        y: 36
        z: 100
    }

    Authorization {
        id: authorization
    }

    function showInfoBanner(message) {
        banner.text = message
        banner.show()
    }

    function openChat(chatId) {
        var component = Qt.createComponent("ChatPage.qml");

        if (component.status === Component.Ready) {
            pageStack.push(component, { chatId: chatId });
        } else {
            console.debug("Error loading component:", component.errorString());
        }
    }

    Component.onCompleted: app.initialize()
}
