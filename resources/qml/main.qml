import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
import "components"

PageStackWindow {
    id: appWindow

    property QtObject icons: Icons {}

    property bool isPortrait: (screen.currentOrientation === Screen.Landscape) ? false : true

    initialPage: MainPage {}

    onOrientationChangeFinished: showStatusBar = isPortrait

    Connections {
        target: tdapi
        onError: {
            banner.text = data.message

            if (data.code !== "404") {
                banner.show()
                console.log(data.message, data.code)
            }
        }
    }

    InfoBanner {
        id: banner
        y: 36
        z: 100
    }


    Component.onCompleted: tdapi.listen()
}
