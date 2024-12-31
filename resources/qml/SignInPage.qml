import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import MyComponent 1.0
import "components"

Item {
    id: signInItem
    anchors.fill: parent
    anchors.margins: UiConstants.DefaultMargin * 2

    property QtObject countryModel: null
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
            onClicked: selectionDialog.open()
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
                font: phoneNumberField.font
                visible: phoneNumberField.text !== ""
            }

            onTextChanged: updatePhoneNumber()
        }

        Item {
            height: UiConstants.IndentDefault
            width: parent.width
        }

        Button {
            text: "Log in by QR Code"
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: authorization.state = "qr_code"
        }
    }

    CountryPickerDialog {
        id: selectionDialog
        titleText: qsTr("ChooseCountry")
        selectedIndex: countryModel.selectedIndex
        onSelectedIndexChanged: handleCountrySelection(selectedIndex)
    }

    QueryDialog {
        id: dialog
        titleText: qsTr("ConfirmCorrectNumber")
        message: "+" + countryModel.callingCode + " " + countryModel.formattedPhoneNumber
        acceptButtonText: qsTr("OK")
        rejectButtonText: qsTr("Edit")

        onAccepted: {
            var cleanedPhoneNumber = phoneNumberField.text.replace(/\D/g, "");
            authorization.setPhoneNumber(cleanedPhoneNumber);
        }
    }

    Connections {
        id: countryModelConnection
        target: countryModel
        ignoreUnknownSignals: true
        onSelectedIndexChanged: updateSelectedIndex()
    }

    Component.onCompleted: initializeComponent()

    function initializeComponent() {
        phoneNumberField.forceActiveFocus();
        authorization.initializeCountryModel();
        countryModel = authorization.countryModel();
        selectionDialog.model = countryModel;

        acceptButton.clicked.connect(function () { dialog.open(); })
        rejectButton.clicked.connect(function () { sheet.state = "closed"; })
    }

    function updatePhoneNumber() {
        countryModel.phoneNumberPrefix = phoneNumberField.text;
        phoneNumberField.text = countryModel.callingCode + countryModel.formattedPhoneNumber;

        var match = phoneNumberField.text.match(/(\d)(?!.*\d)/);
        var lastDigitPosition = match ? phoneNumberField.text.lastIndexOf(match[0]) : -1;

        phoneNumberField.cursorPosition = lastDigitPosition + 1;
    }

    function handleCountrySelection(index) {
        if (index !== -1) {
            phoneNumberField.text = countryModel.get(index).code;
            phoneNumberField.forceActiveFocus();
        }
    }

    function updateSelectedIndex() {
        if (selectionDialog.selectedIndex !== countryModel.selectedIndex) {
            selectionDialog.selectedIndex = countryModel.selectedIndex;
        }
    }
}
