import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
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
        anchors.fill: parent
        anchors.topMargin: header.height
        sourceComponent: BusyIndicator {
            anchors.centerIn: parent
            running: true
            platformStyle: BusyIndicatorStyle { size: "large" }
        }
    }

    Component {
        id: infoComponent
        Item {
            anchors.fill: parent

            Column {
                spacing: 20
                width: parent.width

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    topMargin: 60
                }

                SvgIcon {
                    source: "qrc:/icons/loginlogomobile.svg"
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: 220
                    height: 220
                    color: theme.selectionColor
                }

                Item {
                    width: parent.width
                    height: 20
                }

                Text {
                    id: infoText
                    text: "Different, Handy, Powerful"
                    wrapMode: Text.WordWrap
                    font.pixelSize: 30
                    color: "#777777"
                    horizontalAlignment: Text.AlignHCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Button {
                    text: app.getString("StartMessaging") + app.emptyString
                    platformStyle: ButtonStyle { inverted: true }
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked: {
                        authentication.target = authorization
                        pageStack.push(signInPage)
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
        id: authentication
        target: null
        onCodeRequested: internal.changePage(codeEnterPage, {
                                                 phoneNumber: phoneNumber,
                                                 type: type,
                                                 nextType: nextType,
                                                 timeout: timeout * 1000
                                             })
        onPasswordRequested: internal.changePage(passwordPage, {
                                                     passwordHint: passwordHint,
                                                     hasRecoveryEmailAddress: hasRecoveryEmailAddress,
                                                     recoveryEmailAddressPattern: recoveryEmailAddressPattern
                                                 })
        onRegistrationRequested: internal.changePage(signUpPage, {
                                                         text: text,
                                                         minUserAge: minUserAge,
                                                         showPopup: showPopup
                                                     })
        onError: appWindow.showBanner(message)
        onReady: pageStack.push(Qt.createComponent("ChatsPage.qml"))
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

    Connections {
        target: app
        onAppInitialized: {
            if (app.authorized) {
                pageStack.push(Qt.createComponent("ChatsPage.qml"))
            } else {
                loader.sourceComponent = infoComponent
            }
        }
        onLanguageChanged: {
            console.log("Language changed, updating UI...")
        }
    }

    QtObject {
        id: internal
        function changePage(page, prop) {
            if (pageStack.depth > 1) {
                pageStack.replace(page, prop)
            } else {
                pageStack.push(page, prop)
            }
        }
    }
}
