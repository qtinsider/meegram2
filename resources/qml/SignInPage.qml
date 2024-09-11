import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import MyComponent 1.0

Page {
    id: root

    anchors.margins: 16

    signal cancelClicked

    Flickable {
        id: flickable
        anchors.fill: parent
        contentHeight: contentColumn.height
        contentWidth: contentColumn.width
        flickableDirection: Flickable.VerticalFlick

        Column {
            id: contentColumn
            width: flickable.width
            spacing: 16

            Label {
                id: title
                text: qsTr("YourPhone")
                font.pixelSize: 40
            }

            Rectangle {
                color: "#b2b2b4"
                width: parent.width
                height: 1
            }

            Column {
                id: signInColumn
                spacing: 16

                Label {
                    id: countryNameLabel
                    text: proxyModel.get(selectionDialog.selectedIndex, "name")
                    width: parent.width
                    font.pixelSize: 48
                    platformSelectable: true
                    color: "#0088cc"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: selectionDialog.open()
                    }
                }

                Row {
                    spacing: 16

                    // Future Upgrade Alert: TumblerButton will soon be swapped for a TextField. 🚀
                    // Because typing is the new black! Stay tuned for a stylish input makeover. 😉
                    TumblerButton {
                        id: countryCodeButton
                        width: 160
                        text: "+" + proxyModel.get(selectionDialog.selectedIndex, "code")
                        onClicked: selectionDialog.open()
                    }


                    TextField {
                        id: phoneNumber
                        inputMethodHints: Qt.ImhDialableCharactersOnly | Qt.ImhNoPredictiveText
                        placeholderText: qsTr("PhoneNumberSearch")
                    }
                }

                Label {
                    font.pixelSize: 24
                    text: qsTr("StartText")
                }

                Row {
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
                        authorization.loading = true
                        authorization.setPhoneNumber(countryCodeButton.text + phoneNumber.text);
                    }
                }
            }

            ToolButton {
                text: qsTr("Cancel")
                onClicked: {
                    authorization.loading = false
                    root.cancelClicked()
                }
            }
        }
    }

    CountryModel {
        id: countryModel
    }

    SortFilterProxyModel {
        id: proxyModel
        sourceModel: countryModel
        filterRole: "name"
    }

    SelectionDialog {
        id: selectionDialog
        titleText: qsTr("ChooseCountry")
        selectedIndex: countryModel.defaultIndex
        model: proxyModel
    }

    BusyIndicator {
        anchors.centerIn: parent
        running: authorization.loading
        visible: authorization.loading
        platformStyle: BusyIndicatorStyle { size: "large" }
    }
}
