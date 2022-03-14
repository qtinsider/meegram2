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
        text: "MeeGram"

        MouseArea { anchors.fill: parent }
    }

    Item {
        id: signInInfo

        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        clip: true

        Column {
            id: signInInfoColumn

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
                    text: Localization.getString("StartMessaging") + Localization.emptyString

                    platformStyle: ButtonStyle { inverted: true }

                    onClicked: pageStack.push(signInPageComponent)
                }
            }
        }
    }

    Component {
        id: signInPageComponent
        SignInPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Component {
        id: codeEnterPageComponent
        CodeEnterPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Component {
        id: passwordPageComponent
        PasswordPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Component {
        id: signUpPageComponent
        SignUpPage {
            onCancelClicked: pageStack.pop()
        }
    }

    Connections {
        target: Api
        onCodeRequested: {
            changePage(codeEnterPageComponent, {
                                  phoneNumber: phoneNumber,
                                  type: type,
                                  nextType: nextType,
                                  timeout: timeout * 1000})
        }

        onPasswordRequested: {
            changePage(passwordPageComponent, {
                                  passwordHint: passwordHint,
                                  hasRecoveryEmailAddress: hasRecoveryEmailAddress,
                                  recoveryEmailAddressPattern: recoveryEmailAddressPattern})
        }

        onRegistrationRequested: {
            changePage(signUpPageComponent, {
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

    function changePage(page, prop) {
        if (pageStack.depth > 1)
            pageStack.replace(page, prop)
        else
            pageStack.push(page, prop)
    }

}
