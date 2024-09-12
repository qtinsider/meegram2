import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Item {
    id: signInItem

    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin * 2

    property variant content: authorization.content

    Column {
        id: phoneLoginColumn
        anchors.fill: parent
        spacing: UiConstants.HeaderDefaultTopSpacingPortrait

        Label {
            text: qsTr("YourPhone")
            font: UiConstants.TitleFont
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WordWrap
            width: parent.width
        }

        Label {
            text: qsTr("StartText")
            wrapMode: Text.WordWrap
            font: UiConstants.SubtitleFont
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
        }

        TumblerButton {
            text: countryModel.get(selectionDialog.selectedIndex).name || qsTr("Country")
            height: UiConstants.HeaderDefaultHeightPortrait
            width: parent.width
            onClicked: { selectionDialog.open() }
        }

        TextField {
            id: phoneNumberField
            width: parent.width
            placeholderText: qsTr("PhoneNumberSearch")
            inputMethodHints: Qt.ImhDialableCharactersOnly | Qt.ImhNoPredictiveText
            anchors.horizontalCenter: parent.horizontalCenter
            errorHighlight: text ? !acceptableInput : false
            platformStyle: TextFieldStyle { paddingLeft: plusLabel.width * 2 }
            platformSipAttributes: SipAttributes {
                actionKeyLabel: qsTr("Next")
                actionKeyHighlighted: true
            }

            Label {
                id: plusLabel
                anchors.left: parent.left
                anchors.verticalCenter: parent.verticalCenter
                anchors.leftMargin: 12
                text: "+"
                color: "gray"
                visible: phoneNumberField.text !== ""
            }

            onTextChanged: {
                countryModel.phoneNumberPrefix = text
                phoneNumberField.text = countryModel.countryCallingCode + countryModel.formattedPhoneNumber

                var match = text.match(/(\d)(?!.*\d)/);
                var lastDigitPosition = match ? text.lastIndexOf(match[0]) : -1;

                phoneNumberField.cursorPosition = lastDigitPosition + 1;
            }

            Keys.onReturnPressed: {}
        }

        Item {
            height: UiConstants.IndentDefault
            width: parent.width
        }

        Button {
            text: "Log in by QR Code"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: { authorization.state = "qr_code" }
        }
    }

    CountryModel {
        id: countryModel

        onSelectedIndexChanged: {
            if (selectionDialog.selectedIndex !== selectedIndex)
                selectionDialog.selectedIndex = selectedIndex
        }
    }

    CountryPickerDialog {
        id: selectionDialog
        titleText: qsTr("ChooseCountry")
        selectedIndex: countryModel.selectedIndex
        model: countryModel
        onSelectedIndexChanged: {
            if (selectedIndex !== -1) {
                phoneNumberField.text = countryModel.get(selectedIndex).code
                phoneNumberField.forceActiveFocus();
            }
        }
    }

    function error(code, message) {
        console.log(message)
    }

    QueryDialog {
        id: dialog
        titleText: qsTr("ConfirmCorrectNumber")
        message: "+" + countryModel.countryCallingCode + " " + countryModel.formattedPhoneNumber
        acceptButtonText: qsTr("OK")
        rejectButtonText: qsTr("Edit")

        onAccepted: {
            var cleanedPhoneNumber = phoneNumberField.text.replace(/\D/g, "");
            authorization.setPhoneNumber(cleanedPhoneNumber);
        }
    }

    Component.onCompleted: {
        phoneNumberField.forceActiveFocus();

        acceptButton.clicked.connect(function () { dialog.open(); })
        rejectButton.clicked.connect(function () { sheet.state = "closed"; })
    }
}
