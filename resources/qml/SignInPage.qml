import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

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
                text: app.locale.getString("YourPhone") + app.locale.emptyString
                font.pixelSize: 40
            }
            Rectangle {
                color: "#b2b2b4"
                width: parent.width
                height: 1
            }

            // Phone
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
                        text: "+" + countryModel.get(selectionDialog.selectedIndex).calling_codes
                        onClicked: selectionDialog.open()
                    }

                    TextField {
                        id: phoneNumber
                        inputMethodHints: Qt.ImhDialableCharactersOnly | Qt.ImhNoPredictiveText
                        placeholderText: app.locale.getString("PhoneNumberSearch") + app.locale.emptyString
                    }
                }

                Label {
                    font.pixelSize: 24
                    text: app.locale.getString("StartText") + app.locale.emptyString
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
                text: app.locale.getString("Next") + app.locale.emptyString
                onClicked: {
                    if (phoneNumber.text.length > 0) {
                        authorization.setPhoneNumber(countryCodeButton.text + phoneNumber.text);
                    }
                }
            }
            ToolButton {
                text: app.locale.getString("Cancel") + app.locale.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    MyListModel {
        id: countryModel
        values: app.countries
    }

    SelectionDialog {
        id: selectionDialog
        selectedIndex: countryModel.defaultIndex
        titleText: app.locale.getString("ChooseCountry") + app.locale.emptyString
        model: countryModel
    }

    Component.onCompleted: { countryModel.setDefaultIndex({ "country_code": "NG"}) }
}
