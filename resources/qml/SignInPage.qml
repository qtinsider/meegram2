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
                text: Localization.getString("YourPhone") + Localization.emptyString
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
                    text: Localization.getString("StartText") + Localization.emptyString
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
                text: Localization.getString("Next") + Localization.emptyString
                onClicked: {
                    if (phoneNumber.text.length > 0) {
                        Api.setPhoneNumber(countryCodeButton.text + phoneNumber.text)
                    } else {
                        Api.setPhoneNumber("")
                    }
                }
            }
            ToolButton {
                text: Localization.getString("Cancel") + Localization.emptyString
                onClicked: root.cancelClicked()
            }
        }
    }

    SelectionDialog {
        id: selectionDialog
        titleText: Localization.getString("ChooseCountry") + Localization.emptyString
        selectedIndex: myCountryModel.defaultIndex
        model: CountryModel { id: myCountryModel }
    }
}
