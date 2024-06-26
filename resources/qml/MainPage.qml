import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: "MeeGram"
    }

    Loader {
        id: loader
        anchors { fill: parent; topMargin: header.height; }
        sourceComponent: Item {
            anchors.fill: parent
            BusyIndicator {
                anchors.centerIn: parent
                running: true
                platformStyle: BusyIndicatorStyle { size: "large" }
            }
        }
    }

    Component {
        id: infoComponent
        Item {
            anchors.fill: parent

            Column {
                anchors{
                    top: parent.top
                    topMargin: 30
                    left: parent.left
                    right: parent.right
                }

                spacing: 16

                Text {
                    anchors{
                        left: parent.left
                        right: parent.right
                        leftMargin: 24
                        rightMargin: 24
                    }

                    text: "Different, Handy, Powerful"
                    wrapMode: Text.WordWrap

                    font.pixelSize: 30
                    color: "#777777"

                    horizontalAlignment: Text.AlignHCenter
                }

                Column {
                    anchors.horizontalCenter: parent.horizontalCenter

                    spacing: 16

                    Button {
                        text: locale.getString("StartMessaging") + locale.emptyString

                        platformStyle: ButtonStyle { inverted: true }

                        onClicked: pageStack.push(signInPage)
                    }
                }
            }
        }
    }

    Component {
        id: signInPage
        SignInPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Component {
        id: codeEnterPage
        CodeEnterPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Component {
        id: passwordPage
        PasswordPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Component {
        id: signUpPage
        SignUpPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Connections {
        target: authorization
        onCodeRequested: {
            internal.changePage(codeEnterPage, {
                                    phoneNumber: phoneNumber,
                                    type: type,
                                    nextType: nextType,
                                    timeout: timeout * 1000})
        }

        onPasswordRequested: {
            internal.changePage(passwordPage, {
                                    passwordHint: passwordHint,
                                    hasRecoveryEmailAddress: hasRecoveryEmailAddress,
                                    recoveryEmailAddressPattern: recoveryEmailAddressPattern})
        }

        onRegistrationRequested: {
            internal.changePage(signUpPage, {
                                    text: text,
                                    minUserAge:minUserAge,
                                    showPopup: showPopup})
        }

        onError: appWindow.showBanner(errorString)
    }

    AboutDialog {
        id: aboutDialog
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: Qt.quit()
        }

        ToolIcon {
            anchors.right: parent.right
            iconSource: "qrc:/images/help-icon.png"

            onClicked: aboutDialog.open()
        }
    }

    Timer {
        id: delayInfoTimer

        interval: 2000
        repeat: false
        onTriggered: loader.sourceComponent = infoComponent;
    }

    QtObject {
        id:internal

        function changePage(page, prop) {
            if (pageStack.depth > 1)
                pageStack.replace(page, prop)
            else
                pageStack.push(page, prop)
        }
    }

    Component.onCompleted: delayInfoTimer.restart()
}
