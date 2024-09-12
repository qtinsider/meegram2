import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0

Item {
    id: root

    width: parent ? parent.width : 0
    height: parent ? parent.height : 0

    property Item visualParent
    property int status: DialogStatus.Closed

    property Component authorizationState: getAuthorization(authorization.state)

    property QtObject platformStyle: SheetStyle {}

    visible: status !== DialogStatus.Closed;

    function open() {
        parent = visualParent || __findParent();
        sheet.state = "";
    }

    function close() {
        sheet.state = "closed";
    }

    function __findParent() {
        var next = parent;
        while (next && next.parent
               && next.objectName != "appWindowContent"
               && next.objectName != "windowContent") {
            next = next.parent;
        }
        return next;
    }

    function getButton(name) {
        for (var i = 0; i < buttons.length; ++i) {
            if (buttons[i].objectName === name)
                return buttons[i];
        }

        return undefined;
    }

    MouseArea {
        id: blockMouseInput
        anchors.fill: parent
    }

    Item {
        id: sheet

        //when the sheet is part of a page do nothing
        //when the sheet is a direct child of a PageStackWindow, consider the status bar
        property int statusBarOffset: (typeof __isPage != "undefined") ? 0
                                                                       : (typeof __statusBarHeight == "undefined") ? 0
                                                                                                                   :  __statusBarHeight

        width: parent.width
        height: parent.height - statusBarOffset

        y: statusBarOffset

        clip: true

        property int transitionDurationIn: 300
        property int transitionDurationOut: 450

        state: "closed"

        function transitionStarted() {
            status = (state == "closed") ? DialogStatus.Closing : DialogStatus.Opening;
        }

        function transitionEnded() {
            status = (state == "closed") ? DialogStatus.Closed : DialogStatus.Open;
        }

        states: [
            // Closed state.
            State {
                name: "closed"
                // consider input panel height when input panel is open
                PropertyChanges { target: sheet; y: !inputContext.softwareInputPanelVisible
                                                    ? height : inputContext.softwareInputPanelRect.height + height; }
            }
        ]

        transitions: [
            // Transition between open and closed states.
            Transition {
                from: ""; to: "closed"; reversible: false
                SequentialAnimation {
                    ScriptAction { script: if (sheet.state == "closed") { sheet.transitionStarted(); } else { sheet.transitionEnded(); } }
                    PropertyAnimation { properties: "y"; easing.type: Easing.InOutQuint; duration: sheet.transitionDurationOut }
                    ScriptAction { script: if (sheet.state == "closed") { sheet.transitionEnded(); } else { sheet.transitionStarted(); } }
                }
            },
            Transition {
                from: "closed"; to: ""; reversible: false
                SequentialAnimation {
                    ScriptAction { script: if (sheet.state == "") { sheet.transitionStarted(); } else { sheet.transitionEnded(); } }
                    PropertyAnimation { properties: "y"; easing.type: Easing.OutQuint; duration: sheet.transitionDurationIn }
                    ScriptAction { script: if (sheet.state == "") { sheet.transitionEnded(); } else { sheet.transitionStarted(); } }
                }
            }
        ]

        BorderImage {
            source: platformStyle.background
            width: parent.width
            anchors.top: header.bottom
            anchors.bottom: parent.bottom

            Loader {
                id: contentField
                anchors.fill: parent
                sourceComponent: authorizationState
            }
        }

        Item {
            id: header
            width: parent.width
            height: headerBackground.height
            BorderImage {
                id: headerBackground
                border {
                    left: platformStyle.headerBackgroundMarginLeft
                    right: platformStyle.headerBackgroundMarginRight
                    top: platformStyle.headerBackgroundMarginTop
                    bottom: platformStyle.headerBackgroundMarginBottom
                }
                source: platformStyle.headerBackground
                width: header.width
            }
            Item {
                id: buttonRow
                anchors.fill: parent
                SheetButton {
                    id: rejectButton
                    objectName: "rejectButton"
                    anchors.left: parent.left
                    anchors.leftMargin: root.platformStyle.rejectButtonLeftMargin
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Cancel")

                    // onClicked: reject();
                }
                SheetButton {
                    id: acceptButton
                    objectName: "acceptButton"
                    anchors.right: parent.right
                    anchors.rightMargin: root.platformStyle.acceptButtonRightMargin
                    anchors.verticalCenter: parent.verticalCenter
                    platformStyle: SheetButtonAccentStyle { }
                    text: qsTr("Next")
                }
            }
        }
    }

    Component {
        id: signInPage
        SignInPage {}
    }

    Component {
        id: qrCodePage
        QrCodePage {}
    }

    Component {
        id: codeEnterPage
        CodeEnterPage {}
    }

    Component {
        id: passwordPage
        PasswordPage {}
    }

    Component {
        id: signupPage
        SignUpPage {}
    }

    Connections {
        id: authentication
        target: authorization
        onStateChanged: {
            if (authorization.state === "ready") {
                close()
                pageStack.push(Qt.createComponent("ChatsPage.qml"))
            } else {
                contentField.sourceComponent = authorizationState;
            }
        }
        onError: { appWindow.showInfoBanner(message) }
    }

    function getAuthorization(value) {
        switch (value) {
        case "phone_number":
            return signInPage;
        case "qr_code":
            return qrCodePage;
        case "code":
            return codeEnterPage;
        case "password":
            return passwordPage;
        case "registration":
            return signupPage;
        default:
            return signInPage;
        }
    }

    Component.onDestruction: { authentication.target = null }
}
