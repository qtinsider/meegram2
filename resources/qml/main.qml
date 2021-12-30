import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
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

            if (data.code !== "404")
                banner.show()
        }
        onUpdateAuthorizationState: {
            authorizationStateData = authorizationState
        }
    }

    InfoBanner {
        id: banner
        y: 36
        z: 100
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
