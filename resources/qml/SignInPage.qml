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
                text: app.getString("YourPhone") + app.emptyString
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
                    text: countryModel.get(selectionDialog.selectedIndex).name
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

                    TumblerButton {
                        id: countryCodeButton
                        width: 160
                        text: "+" + countryModel.get(selectionDialog.selectedIndex).code
                        onClicked: selectionDialog.open()
                    }

                    TextField {
                        id: phoneNumber
                        inputMethodHints: Qt.ImhDialableCharactersOnly | Qt.ImhNoPredictiveText
                        placeholderText: app.getString("PhoneNumberSearch") + app.emptyString
                    }
                }

                Label {
                    font.pixelSize: 24
                    text: app.getString("StartText") + app.emptyString
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
                text: app.getString("Next") + app.emptyString
                onClicked: {
                    if (phoneNumber.text.length > 0) {
                        authorization.setPhoneNumber(countryCodeButton.text + phoneNumber.text);
                    }
                }
            }

            ToolButton {
                text: app.getString("Cancel") + app.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    ListModel {
        id: countryModel
    }

    SelectionDialog {
        id: selectionDialog
        titleText: app.getString("ChooseCountry") + app.emptyString
        selectedIndex: countryModel.defaultIndex
        model: countryModel
    }
}
