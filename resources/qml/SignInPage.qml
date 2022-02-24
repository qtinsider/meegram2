import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    Flickable {
        id: flickable
        anchors.fill: parent
        anchors.margins: 16
        contentHeight: contentColumn.height

        Column {
            id: contentColumn

            width: flickable.width
            height: childrenRect.height

            spacing: 16


            Label {
                id: title
                text: qsTr("YourPhone")
                font.pixelSize: 40
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: flickable.width
            }

            // Phone
            Column {
                id: signInColumn

                width: parent.width
                spacing: 16

                Label {
                    id: countryNameLabel
                    text: myCountryModel.get(selectionDialog.selectedIndex).name || ""
                    width: parent.width
                    font.pixelSize: 48
                    platformSelectable: true
                    color: "#0088cc"
                    wrapMode: Text.WordWrap

                    MouseArea {
                        anchors.fill:  parent
                        onClicked: selectionDialog.open()
                    }
                }

                Row {
                    width: parent.width
                    spacing: 16

                    TumblerButton {
                        id: countryCodeButton
                        width: 160
                        text: "+" + myCountryModel.get(selectionDialog.selectedIndex).code
                        onClicked: selectionDialog.open()
                    }

                    TextField {
                        id: phoneNumber
                        inputMethodHints: Qt.ImhDialableCharactersOnly | Qt.ImhNoPredictiveText
                        placeholderText: "Phone number"
                    }
                }

                Label {
                    font.pixelSize: 24
                    width: parent.width
                    text: qsTr("StartText")
                }

                Row {
                    width: parent.width
                    spacing: 16

                }
            }
        }
    }

    tools: ToolBarLayout {
        ToolButtonRow {
            ToolButton {
                text: qsTr("Next")
                onClicked: {
                    if (phoneNumber.text.length > 0) {
                        tdapi.setPhoneNumber(countryCodeButton.text + phoneNumber.text)
                    } else {
                        tdapi.setPhoneNumber("")
                    }
                }
            }
            ToolButton {
                text: qsTrId("Cancel")
                onClicked: {
                    pageStack.pop()
                }
            }
        }
    }

    function onCodeRequested(codeInfo) {
        var component = Qt.createComponent("CodeEnterPage.qml")
        if (component.status === Component.Ready) {
            pageStack.replace(component, {
                               length: codeInfo.length,
                               timeout: codeInfo.timeout * 1000,
                               __title: codeInfo.title,
                               __subtitle: codeInfo.subtitle,
                               __nextTypeString: codeInfo.nextTypeString,
                               __isNextTypeSms: codeInfo.isNextTypeSms,
                           })
        } else {
            console.debug("Error loading component:", component.errorString());
        }
    }

    SelectionDialog {
        id: selectionDialog
        titleText: qsTr("ChooseCountry")
        selectedIndex: myCountryModel.defaultIndex
        model: CountryModel { id: myCountryModel }
    }

    Component.onCompleted: {
        tdapi.codeRequested.connect(onCodeRequested)
        tdapi.error.connect(function(error) { showInfoBanner(error.message) })
    }

    Component.onDestruction: {
        tdapi.codeRequested.disconnect(onCodeRequested);
        tdapi.error.disconnect(function(error) { showInfoBanner(error.message) });
    }
}
