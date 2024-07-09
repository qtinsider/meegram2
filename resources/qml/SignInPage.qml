import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Page {
    id: root

    signal cancelClicked

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
                text: app.locale.getString("YourPhone") + app.locale.emptyString
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
                        placeholderText: app.locale.getString("PhoneNumberSearch") + app.locale.emptyString
                    }
                }

                Label {
                    font.pixelSize: 24
                    width: parent.width
                    text: app.locale.getString("StartText") + app.locale.emptyString
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
                text: app.locale.getString("Next") + app.locale.emptyString
                onClicked: {
                    if (phoneNumber.text.length > 0) {
                        authorization.setPhoneNumber(countryCodeButton.text + phoneNumber.text)
                    } else {
                        authorization.setPhoneNumber("")
                    }
                }
            }
            ToolButton {
                text: app.locale.getString("Cancel") + app.locale.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    CountryModel {
        id: myCountryModel
        countries:  app.countries
    }

    SelectionDialog {
        id: selectionDialog
        titleText: app.locale.getString("ChooseCountry") + app.locale.emptyString
        selectedIndex: myCountryModel.defaultIndex
        model: myCountryModel
    }
}
