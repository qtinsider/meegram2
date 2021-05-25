import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import com.strawberry.meegram 1.0

Sheet {
    id: root

    property string termsOfServiceString: ""

    rejectButtonText: "Cancel"

    content: Item {
        id: background
        anchors.fill: parent

        Flickable {
            id: flickable
            anchors.fill: parent
            anchors.margins: 16
            contentHeight: contentColumn.height

            Column {
                id: contentColumn
                spacing: 16
                Item {
                    id: contentItem
                    width: flickable.width
                    height: childrenRect.height

                    Loader {
                        id: componentLoader

                        width: parent.width
                    }
                }
            }

            Behavior on contentHeight { NumberAnimation { easing.type: Easing.Linear} }
        }

    }

    Component {
        id: signInComponent

        Column {
            width: parent.width
            spacing: 16

            Label {
                color: "#282828"
                font.pixelSize: 40
                text: qsTr("YourPhone")
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: parent.width
            }


            Label {
                id: countryNameLabel
                text: myCountryModel.get(selectionDialog.selectedIndex).name
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
                    text: myCountryModel.get(selectionDialog.selectedIndex).code
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

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Next")
                width: parent.width / 2
                onClicked: {
                    if (phoneNumber.text.length > 0)
                        tdapi.setPhoneNumber(countryCodeButton.text + phoneNumber.text)
                }
            }
        }
    }

    Component {
        id: codeEnterComponent


        Column {
            id: codeEnterColumn

            property alias codeTitle: title.text
            property alias codeTypeString: codeType.text
            property alias isNextTypeSms: isNextTypeSms.visible
            property alias nextTypeString: nextTypeLabel.text

            property int codeLength

            property string phoneNumber: ""
            property int timeout: 0

            width: parent.width
            spacing: 16

            Label {
                color: "#282828"
                font.pixelSize: 40
                text: qsTr("YourCode")
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: parent.width
            }


            Label {
                id: title
            }

            Column {
                width: parent.width
                spacing: 20

                TextField {
                    id: code
                    width: parent.width
                    inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoPredictiveText
                    placeholderText: qsTr("Code")
                    onTextChanged: {
                        if(text.length >= codeLength)
                            tdapi.checkCode(code.text)
                    }
                }

                Label {
                    id: codeType
                    font.pixelSize: 24
                    width: parent.width
                }

                Button {
                    anchors.horizontalCenter: parent.horizontalCenter
                    text: qsTr("Next")
                    width: parent.width / 2
                    onClicked: {
                        tdapi.checkCode(code.text)
                    }
                }

                Label {
                    id: isNextTypeSms

                    anchors.horizontalCenter: parent.horizontalCenter

                    font.pixelSize: 24
                    font.underline: true

                    color: "#0088cc"
                    text: qsTr("DidNotGetTheCodeSms")

                    visible: false

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            tdapi.resendCode()
                        }
                    }
                }

                Row {
                    id: codeTextRow
                    anchors.horizontalCenter: parent.horizontalCenter
                    spacing: 4
                    Label {
                        id: nextTypeLabel
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: 24
                    }
                    Label {
                        id: codeTimeText
                        horizontalAlignment: Text.AlignHCenter
                        font.pointSize: 24
                        color: theme.selectionColor // "#ffcc00"
                    }
                    visible: codeTimeText.text !== ""
                }

                Timer {
                    id: codeExpireTimer
                    interval: 1000
                    repeat: true
                    onTriggered: {

                        timeout = timeout - 1000
                        codeTimeText.text = Utils.formatTime(timeout / 1000)
                        if (timeout === 0) {
                            codeExpireTimer.stop()
                            codeTextRow.visible = false
                            tdapi.resendCode()
                        }
                    }
                }
            }

            onTimeoutChanged: {
                codeExpireTimer.start()
                codeTimeText.text = Utils.formatTime(timeout / 1000)
            }
        }
    }

    Component {
        id: enterPasswordComponent

        Column {
            id: enterPasswordColumn

            property string passwordHint;
            property bool hasEecoveryEmailAddress;
            property string recoveryEmailAddressPattern;

            width: parent.width
            spacing: 10

            Label {
                color: "#282828"
                font.pixelSize: 40
                text: qsTr("TwoStepVerification")
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: parent.width
            }

            Label {
                width: parent.width
                text: qsTr("LoginPasswordText")
            }

            Item {
                width: parent.width
                height: 10 / 2
            }
            Label {
                text: qsTr("YourPassword")
            }

            TextField {
                id: password
                width: parent.width
                echoMode: TextInput.Password
                placeholderText: qsTr("Password")
            }

            Label {
                id: hint
                width: parent.width
                font.pixelSize: 24
                text: "<b>Password hint:</b> <span style=\"color: #999\">(%1)</span>".arg(passwordHint)
                visible: passwordHint !== ""
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Next")
                width: parent.width / 2
                onClicked: {
                    if (password.text.length > 0) {
                        tdapi.checkPassword(password.text)
                        password.focus = false
                        password.text = ""
                    }
                }
            }

        }
    }

    Component {
        id: signUpComponent

        Column {
            id: signUpColumn

            width: parent.width
            spacing: 10

            Label {
                color: "#282828"
                font.pixelSize: 40
                text: qsTr("YourName")
            }
            Rectangle {
                color: "#b2b2b4"
                height: 1
                width: parent.width
            }

            Label {
                text: qsTr("RegisterText2")
            }

            Column {
                spacing: 15
                width: parent.width

                TextField {
                    id: firstName
                    width: parent.width
                    inputMethodHints: Qt.ImhNoPredictiveText
                    placeholderText: qsTr("FirstName")
                }
                TextField {
                    id: lastName
                    width: parent.width
                    inputMethodHints: Qt.ImhNoPredictiveText
                    placeholderText: qsTr("LastName")
                }
            }

            Label {
                width: parent.width
                text: "<style type=text/css> a { text-decoration: underline; color: #0088cc } </style>By signing up,<br>you agree to the <a href='http://www.telegram.com'>Terms of Service.</a>"
                font.pixelSize: 24
                onLinkActivated: internal.showTermsOfService()
                horizontalAlignment: Text.AlignHCenter
            }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Done")
                onClicked: {
                    tdapi.registerUser(firstName.text, lastName.text)
                }
            }
        }
    }

    SelectionDialog {
        id: selectionDialog
        titleText: qsTr("ChooseCountry")
        selectedIndex: 148
        model: myCountryModel
    }
    
    Component {
        id: termsOfServiceComponent

        QueryDialog {
            id: dialog

            property string termsOfService: ""
            
            titleText: qsTr("TermsOfService")
            message: termsOfService
            
            onLinkActivated: Qt.openUrlExternally(link)

            acceptButtonText: "OK"
            onAccepted: { dialog.destroy(1000) }
        }
    }

    ListModel {
        id: myCountryModel
        ListElement{name: "Afghanistan"; iso2: "AF"; code: "+93"}
        ListElement{name: "Albania"; iso2: "AL"; code: "+355"}
        ListElement{name: "Algeria"; iso2: "DZ"; code: "+213"}
        ListElement{name: "American Samoa"; iso2: "AS"; code: "+1684"}
        ListElement{name: "Andorra"; iso2: "AD"; code: "+376"}
        ListElement{name: "Angola"; iso2: "AO"; code: "+244"}
        ListElement{name: "Anguilla"; iso2: "AI"; code: "+1264"}
        ListElement{name: "Antigua & Barbuda"; iso2: "AG"; code: "+1268"}
        ListElement{name: "Argentina"; iso2: "AR"; code: "+54"}
        ListElement{name: "Armenia"; iso2: "AM"; code: "+374"}
        ListElement{name: "Aruba"; iso2: "AW"; code: "+297"}
        ListElement{name: "Australia"; iso2: "AU"; code: "+61"}
        ListElement{name: "Austria"; iso2: "AT"; code: "+43"}
        ListElement{name: "Azerbaijan"; iso2: "AZ"; code: "+994"}
        ListElement{name: "Bahamas"; iso2: "BS"; code: "+1242"}
        ListElement{name: "Bahrain"; iso2: "BH"; code: "+973"}
        ListElement{name: "Bangladesh"; iso2: "BD"; code: "+880"}
        ListElement{name: "Barbados"; iso2: "BB"; code: "+1246"}
        ListElement{name: "Belarus"; iso2: "BY"; code: "+375"}
        ListElement{name: "Belgium"; iso2: "BE"; code: "+32"}
        ListElement{name: "Belize"; iso2: "BZ"; code: "+501"}
        ListElement{name: "Benin"; iso2: "BJ"; code: "+229"}
        ListElement{name: "Bermuda"; iso2: "BM"; code: "+1441"}
        ListElement{name: "Bhutan"; iso2: "BT"; code: "+975"}
        ListElement{name: "Bolivia"; iso2: "BO"; code: "+591"}
        ListElement{name: "Bonaire, Sint Eustatius & Saba"; iso2: "BQ"; code: "+599"}
        ListElement{name: "Bosnia & Herzegovina"; iso2: "BA"; code: "+387"}
        ListElement{name: "Botswana"; iso2: "BW"; code: "+267"}
        ListElement{name: "Brazil"; iso2: "BR"; code: "+55"}
        ListElement{name: "British Virgin Islands"; iso2: "VG"; code: "+1284"}
        ListElement{name: "Brunei Darussalam"; iso2: "BN"; code: "+673"}
        ListElement{name: "Bulgaria"; iso2: "BG"; code: "+359"}
        ListElement{name: "Burkina Faso"; iso2: "BF"; code: "+226"}
        ListElement{name: "Burundi"; iso2: "BI"; code: "+257"}
        ListElement{name: "Cambodia"; iso2: "KH"; code: "+855"}
        ListElement{name: "Cameroon"; iso2: "CM"; code: "+237"}
        ListElement{name: "Canada"; iso2: "CA"; code: "+1"}
        ListElement{name: "Cape Verde"; iso2: "CV"; code: "+238"}
        ListElement{name: "Cayman Islands"; iso2: "KY"; code: "+1345"}
        ListElement{name: "Central African Republic"; iso2: "CF"; code: "+236"}
        ListElement{name: "Chad"; iso2: "TD"; code: "+235"}
        ListElement{name: "Chile"; iso2: "CL"; code: "+56"}
        ListElement{name: "China"; iso2: "CN"; code: "+86"}
        ListElement{name: "Colombia"; iso2: "CO"; code: "+57"}
        ListElement{name: "Comoros"; iso2: "KM"; code: "+269"}
        ListElement{name: "Congo"; iso2: "CG"; code: "+242"}
        ListElement{name: "Congo, Democratic Republic"; iso2: "CD"; code: "+243"}
        ListElement{name: "Cook Islands"; iso2: "CK"; code: "+682"}
        ListElement{name: "Costa Rica"; iso2: "CR"; code: "+506"}
        ListElement{name: "Croatia"; iso2: "HR"; code: "+385"}
        ListElement{name: "Cuba"; iso2: "CU"; code: "+53"}
        ListElement{name: "Curaçao"; iso2: "CW"; code: "+599"}
        ListElement{name: "Cyprus"; iso2: "CY"; code: "+357"}
        ListElement{name: "Czech Republic"; iso2: "CZ"; code: "+420"}
        ListElement{name: "Côte d`Ivoire"; iso2: "CI"; code: "+225"}
        ListElement{name: "Denmark"; iso2: "DK"; code: "+45"}
        ListElement{name: "Diego Garcia"; iso2: "IO"; code: "+246"}
        ListElement{name: "Djibouti"; iso2: "DJ"; code: "+253"}
        ListElement{name: "Dominica"; iso2: "DM"; code: "+1767"}
        ListElement{name: "Dominican Republic"; iso2: "DO"; code: "+1"}
        ListElement{name: "Ecuador"; iso2: "EC"; code: "+593"}
        ListElement{name: "Egypt"; iso2: "EG"; code: "+20"}
        ListElement{name: "El Salvador"; iso2: "SV"; code: "+503"}
        ListElement{name: "Equatorial Guinea"; iso2: "GQ"; code: "+240"}
        ListElement{name: "Eritrea"; iso2: "ER"; code: "+291"}
        ListElement{name: "Estonia"; iso2: "EE"; code: "+372"}
        ListElement{name: "Ethiopia"; iso2: "ET"; code: "+251"}
        ListElement{name: "Falkland Islands"; iso2: "FK"; code: "+500"}
        ListElement{name: "Faroe Islands"; iso2: "FO"; code: "+298"}
        ListElement{name: "Fiji"; iso2: "FJ"; code: "+679"}
        ListElement{name: "Finland"; iso2: "FI"; code: "+358"}
        ListElement{name: "France"; iso2: "FR"; code: "+33"}
        ListElement{name: "French Guiana"; iso2: "GF"; code: "+594"}
        ListElement{name: "French Polynesia"; iso2: "PF"; code: "+689"}
        ListElement{name: "Gabon"; iso2: "GA"; code: "+241"}
        ListElement{name: "Gambia"; iso2: "GM"; code: "+220"}
        ListElement{name: "Georgia"; iso2: "GE"; code: "+995"}
        ListElement{name: "Germany"; iso2: "DE"; code: "+49"}
        ListElement{name: "Ghana"; iso2: "GH"; code: "+233"}
        ListElement{name: "Gibraltar"; iso2: "GI"; code: "+350"}
        ListElement{name: "Greece"; iso2: "GR"; code: "+30"}
        ListElement{name: "Greenland"; iso2: "GL"; code: "+299"}
        ListElement{name: "Grenada"; iso2: "GD"; code: "+1473"}
        ListElement{name: "Guadeloupe"; iso2: "GP"; code: "+590"}
        ListElement{name: "Guam"; iso2: "GU"; code: "+1671"}
        ListElement{name: "Guatemala"; iso2: "GT"; code: "+502"}
        ListElement{name: "Guinea"; iso2: "GN"; code: "+224"}
        ListElement{name: "Guinea-Bissau"; iso2: "GW"; code: "+245"}
        ListElement{name: "Guyana"; iso2: "GY"; code: "+592"}
        ListElement{name: "Haiti"; iso2: "HT"; code: "+509"}
        ListElement{name: "Honduras"; iso2: "HN"; code: "+504"}
        ListElement{name: "Hong Kong"; iso2: "HK"; code: "+852"}
        ListElement{name: "Hungary"; iso2: "HU"; code: "+36"}
        ListElement{name: "Iceland"; iso2: "IS"; code: "+354"}
        ListElement{name: "India"; iso2: "IN"; code: "+91"}
        ListElement{name: "Indonesia"; iso2: "ID"; code: "+62"}
        ListElement{name: "Iran"; iso2: "IR"; code: "+98"}
        ListElement{name: "Iraq"; iso2: "IQ"; code: "+964"}
        ListElement{name: "Ireland"; iso2: "IE"; code: "+353"}
        ListElement{name: "Israel"; iso2: "IL"; code: "+972"}
        ListElement{name: "Italy"; iso2: "IT"; code: "+39"}
        ListElement{name: "Jamaica"; iso2: "JM"; code: "+1876"}
        ListElement{name: "Japan"; iso2: "JP"; code: "+81"}
        ListElement{name: "Jordan"; iso2: "JO"; code: "+962"}
        ListElement{name: "Kazakhstan"; iso2: "KZ"; code: "+7"}
        ListElement{name: "Kenya"; iso2: "KE"; code: "+254"}
        ListElement{name: "Kiribati"; iso2: "KI"; code: "+686"}
        ListElement{name: "Kuwait"; iso2: "KW"; code: "+965"}
        ListElement{name: "Kyrgyzstan"; iso2: "KG"; code: "+996"}
        ListElement{name: "Laos"; iso2: "LA"; code: "+856"}
        ListElement{name: "Latvia"; iso2: "LV"; code: "+371"}
        ListElement{name: "Lebanon"; iso2: "LB"; code: "+961"}
        ListElement{name: "Lesotho"; iso2: "LS"; code: "+266"}
        ListElement{name: "Liberia"; iso2: "LR"; code: "+231"}
        ListElement{name: "Libya"; iso2: "LY"; code: "+218"}
        ListElement{name: "Liechtenstein"; iso2: "LI"; code: "+423"}
        ListElement{name: "Lithuania"; iso2: "LT"; code: "+370"}
        ListElement{name: "Luxembourg"; iso2: "LU"; code: "+352"}
        ListElement{name: "Macau"; iso2: "MO"; code: "+853"}
        ListElement{name: "Macedonia"; iso2: "MK"; code: "+389"}
        ListElement{name: "Madagascar"; iso2: "MG"; code: "+261"}
        ListElement{name: "Malawi"; iso2: "MW"; code: "+265"}
        ListElement{name: "Malaysia"; iso2: "MY"; code: "+60"}
        ListElement{name: "Maldives"; iso2: "MV"; code: "+960"}
        ListElement{name: "Mali"; iso2: "ML"; code: "+223"}
        ListElement{name: "Malta"; iso2: "MT"; code: "+356"}
        ListElement{name: "Marshall Islands"; iso2: "MH"; code: "+692"}
        ListElement{name: "Martinique"; iso2: "MQ"; code: "+596"}
        ListElement{name: "Mauritania"; iso2: "MR"; code: "+222"}
        ListElement{name: "Mauritius"; iso2: "MU"; code: "+230"}
        ListElement{name: "Mexico"; iso2: "MX"; code: "+52"}
        ListElement{name: "Micronesia"; iso2: "FM"; code: "+691"}
        ListElement{name: "Moldova"; iso2: "MD"; code: "+373"}
        ListElement{name: "Monaco"; iso2: "MC"; code: "+377"}
        ListElement{name: "Mongolia"; iso2: "MN"; code: "+976"}
        ListElement{name: "Montenegro"; iso2: "ME"; code: "+382"}
        ListElement{name: "Montserrat"; iso2: "MS"; code: "+1664"}
        ListElement{name: "Morocco"; iso2: "MA"; code: "+212"}
        ListElement{name: "Mozambique"; iso2: "MZ"; code: "+258"}
        ListElement{name: "Myanmar"; iso2: "MM"; code: "+95"}
        ListElement{name: "Namibia"; iso2: "NA"; code: "+264"}
        ListElement{name: "Nauru"; iso2: "NR"; code: "+674"}
        ListElement{name: "Nepal"; iso2: "NP"; code: "+977"}
        ListElement{name: "Netherlands"; iso2: "NL"; code: "+31"}
        ListElement{name: "New Caledonia"; iso2: "NC"; code: "+687"}
        ListElement{name: "New Zealand"; iso2: "NZ"; code: "+64"}
        ListElement{name: "Nicaragua"; iso2: "NI"; code: "+505"}
        ListElement{name: "Niger"; iso2: "NE"; code: "+227"}
        ListElement{name: "Nigeria"; iso2: "NG"; code: "+234"}
        ListElement{name: "Niue"; iso2: "NU"; code: "+683"}
        ListElement{name: "Norfolk Island"; iso2: "NF"; code: "+672"}
        ListElement{name: "North Korea"; iso2: "KP"; code: "+850"}
        ListElement{name: "Northern Mariana Islands"; iso2: "MP"; code: "+1670"}
        ListElement{name: "Norway"; iso2: "NO"; code: "+47"}
        ListElement{name: "Oman"; iso2: "OM"; code: "+968"}
        ListElement{name: "Pakistan"; iso2: "PK"; code: "+92"}
        ListElement{name: "Palau"; iso2: "PW"; code: "+680"}
        ListElement{name: "Palestine"; iso2: "PS"; code: "+970"}
        ListElement{name: "Panama"; iso2: "PA"; code: "+507"}
        ListElement{name: "Papua New Guinea"; iso2: "PG"; code: "+675"}
        ListElement{name: "Paraguay"; iso2: "PY"; code: "+595"}
        ListElement{name: "Peru"; iso2: "PE"; code: "+51"}
        ListElement{name: "Philippines"; iso2: "PH"; code: "+63"}
        ListElement{name: "Poland"; iso2: "PL"; code: "+48"}
        ListElement{name: "Portugal"; iso2: "PT"; code: "+351"}
        ListElement{name: "Puerto Rico"; iso2: "PR"; code: "+1"}
        ListElement{name: "Qatar"; iso2: "QA"; code: "+974"}
        ListElement{name: "Romania"; iso2: "RO"; code: "+40"}
        ListElement{name: "Russian Federation"; iso2: "RU"; code: "+7"}
        ListElement{name: "Rwanda"; iso2: "RW"; code: "+250"}
        ListElement{name: "Réunion"; iso2: "RE"; code: "+262"}
        ListElement{name: "Saint Helena"; iso2: "SH"; code: "+247"}
        ListElement{name: "Saint Helena"; iso2: "SH2"; code: "+290"}
        ListElement{name: "Saint Kitts & Nevis"; iso2: "KN"; code: "+1869"}
        ListElement{name: "Saint Lucia"; iso2: "LC"; code: "+1758"}
        ListElement{name: "Saint Pierre & Miquelon"; iso2: "PM"; code: "+508"}
        ListElement{name: "Saint Vincent & the Grenadines"; iso2: "VC"; code: "+1784"}
        ListElement{name: "Samoa"; iso2: "WS"; code: "+685"}
        ListElement{name: "San Marino"; iso2: "SM"; code: "+378"}
        ListElement{name: "Saudi Arabia"; iso2: "SA"; code: "+966"}
        ListElement{name: "Senegal"; iso2: "SN"; code: "+221"}
        ListElement{name: "Serbia"; iso2: "RS"; code: "+381"}
        ListElement{name: "Seychelles"; iso2: "SC"; code: "+248"}
        ListElement{name: "Sierra Leone"; iso2: "SL"; code: "+232"}
        ListElement{name: "Singapore"; iso2: "SG"; code: "+65"}
        ListElement{name: "Sint Maarten"; iso2: "SX"; code: "+1721"}
        ListElement{name: "Slovakia"; iso2: "SK"; code: "+421"}
        ListElement{name: "Slovenia"; iso2: "SI"; code: "+386"}
        ListElement{name: "Solomon Islands"; iso2: "SB"; code: "+677"}
        ListElement{name: "Somalia"; iso2: "SO"; code: "+252"}
        ListElement{name: "South Africa"; iso2: "ZA"; code: "+27"}
        ListElement{name: "South Korea"; iso2: "KR"; code: "+82"}
        ListElement{name: "South Sudan"; iso2: "SS"; code: "+211"}
        ListElement{name: "Spain"; iso2: "ES"; code: "+34"}
        ListElement{name: "Sri Lanka"; iso2: "LK"; code: "+94"}
        ListElement{name: "Sudan"; iso2: "SD"; code: "+249"}
        ListElement{name: "Suriname"; iso2: "SR"; code: "+597"}
        ListElement{name: "Swaziland"; iso2: "SZ"; code: "+268"}
        ListElement{name: "Sweden"; iso2: "SE"; code: "+46"}
        ListElement{name: "Switzerland"; iso2: "CH"; code: "+41"}
        ListElement{name: "Syrian Arab Republic"; iso2: "SY"; code: "+963"}
        ListElement{name: "São Tomé & Príncipe"; iso2: "ST"; code: "+239"}
        ListElement{name: "Taiwan"; iso2: "TW"; code: "+886"}
        ListElement{name: "Tajikistan"; iso2: "TJ"; code: "+992"}
        ListElement{name: "Tanzania"; iso2: "TZ"; code: "+255"}
        ListElement{name: "Thailand"; iso2: "TH"; code: "+66"}
        ListElement{name: "Timor-Leste"; iso2: "TL"; code: "+670"}
        ListElement{name: "Togo"; iso2: "TG"; code: "+228"}
        ListElement{name: "Tokelau"; iso2: "TK"; code: "+690"}
        ListElement{name: "Tonga"; iso2: "TO"; code: "+676"}
        ListElement{name: "Trinidad & Tobago"; iso2: "TT"; code: "+1868"}
        ListElement{name: "Tunisia"; iso2: "TN"; code: "+216"}
        ListElement{name: "Turkey"; iso2: "TR"; code: "+90"}
        ListElement{name: "Turkmenistan"; iso2: "TM"; code: "+993"}
        ListElement{name: "Turks & Caicos Islands"; iso2: "TC"; code: "+1649"}
        ListElement{name: "Tuvalu"; iso2: "TV"; code: "+688"}
        ListElement{name: "US Virgin Islands"; iso2: "VI"; code: "+1340"}
        ListElement{name: "United States of America"; iso2: "US"; code: "+1"}
        ListElement{name: "Uganda"; iso2: "UG"; code: "+256"}
        ListElement{name: "Ukraine"; iso2: "UA"; code: "+380"}
        ListElement{name: "United Arab Emirates"; iso2: "AE"; code: "+971"}
        ListElement{name: "United Kingdom"; iso2: "GB"; code: "+44"}
        ListElement{name: "Uruguay"; iso2: "UY"; code: "+598"}
        ListElement{name: "Uzbekistan"; iso2: "UZ"; code: "+998"}
        ListElement{name: "Vanuatu"; iso2: "VU"; code: "+678"}
        ListElement{name: "Venezuela"; iso2: "VE"; code: "+58"}
        ListElement{name: "Vietnam"; iso2: "VN"; code: "+84"}
        ListElement{name: "Wallis & Futuna"; iso2: "WF"; code: "+681"}
        ListElement{name: "Yemen"; iso2: "YE"; code: "+967"}
        ListElement{name: "Zambia"; iso2: "ZM"; code: "+260"}
        ListElement{name: "Zimbabwe"; iso2: "ZW"; code: "+263"}
    }

    QtObject {
        id: internal

        function isNextTypeSms(codeInfo) {
            if (!codeInfo) return false;
            if (!codeInfo.next_type) return false;

            if (codeInfo.next_type['@type'] === "authenticationCodeTypeSms")
                return true;

            return false;
        }

        function getNextTypeString(codeInfo) {
            if (!codeInfo) return "";
            if (!codeInfo.next_type) return "";

            switch (codeInfo.next_type['@type']) {
            case 'authenticationCodeTypeCall':
                return qsTr("CallText");
            case 'authenticationCodeTypeSms':
                return qsTr("SmsText");
            }
            return "";
        }

        function getTitle(codeInfo) {
            if (!codeInfo) return "";
            if (!codeInfo.type) return "";

            switch (codeInfo.type['@type']) {
            case 'authenticationCodeTypeTelegramMessage': {
                return qsTr("SentAppCodeTitle");
            }
            case 'authenticationCodeTypeCall':
            case 'authenticationCodeTypeSms': {
                return qsTr("SentSmsCodeTitle");
            }
            }

            return "Title";
        }

        function getCodeLength(codeInfo) {
            if (!codeInfo) return 0;
            if (!codeInfo.type) return 0;

            switch (codeInfo.type['@type']) {
            case 'authenticationCodeTypeCall': {
                return codeInfo.type.length;
            }
            case 'authenticationCodeTypeFlashCall': {
                return 0;
            }
            case 'authenticationCodeTypeSms': {
                return codeInfo.type.length;
            }
            case 'authenticationCodeTypeTelegramMessage': {
                return codeInfo.type.length;
            }
            }

            return 0;
        }

        function getSubtitle(codeInfo) {
            if (!codeInfo) return "";
            if (!codeInfo.type) return "";

            var phoneNumber = codeInfo.phone_number
            switch (codeInfo.type['@type']) {
            case 'authenticationCodeTypeCall': {
                return qsTr("SentCallCode").arg(phoneNumber);
            }
            case 'authenticationCodeTypeFlashCall': {
                return qsTr("SentCallOnly").arg(phoneNumber);
            }
            case 'authenticationCodeTypeSms': {
                return qsTr("SentSmsCode").arg(phoneNumber);
            }
            case 'authenticationCodeTypeTelegramMessage': {
                return qsTr("SentAppCode");
            }
            }

            return "";
        }
        
        function showTermsOfService() {
            var dialog = termsOfServiceComponent.createObject(root, { termsOfService: termsOfServiceString });
            dialog.open();
        }
    }

    Connections {
        target: tdapi
        onUpdateAuthorizationState: {
            switch (authorizationState['@type']) {
            case 'authorizationStateWaitPhoneNumber': {
                componentLoader.sourceComponent = signInComponent
                break;
            }
            case 'authorizationStateWaitCode': {
                componentLoader.sourceComponent = codeEnterComponent

                var codeInfo = authorizationState.code_info

                componentLoader.item.codeTitle = internal.getTitle(codeInfo);
                componentLoader.item.codeTypeString = internal.getSubtitle(codeInfo);
                componentLoader.item.isNextTypeSms = internal.isNextTypeSms(codeInfo);
                componentLoader.item.nextTypeString = internal.getNextTypeString(codeInfo);

                componentLoader.item.phoneNumber = codeInfo.phone_number;
                componentLoader.item.timeout = codeInfo.timeout * 1000;
                componentLoader.item.codeLength = internal.getCodeLength(codeInfo);
                break;
            }
            case 'authorizationStateWaitRegistration': {
                componentLoader.sourceComponent = signUpComponent

                termsOfServiceString = internal.getTermsOfServiceString(authorizationState);
                break;
            }
            case 'authorizationStateWaitPassword': {
                componentLoader.sourceComponent = enterPasswordComponent

                componentLoader.item.passwordHint = authorizationState.password_hint;
                break;
            }
            }
        }
        onAuthorizationStateReady:  root.accept()
    }

    Component.onCompleted: {
        switch (tdapi.authorizationState) {
        case TdApi.AuthorizationStateWaitPhoneNumber: {
            componentLoader.sourceComponent = signInComponent
            break;
        }
        case TdApi.AuthorizationStateWaitCode: {
            componentLoader.sourceComponent = codeEnterComponent;

            var codeInfo = authorizationStateData.code_info;

            componentLoader.item.codeTitle = internal.getTitle(codeInfo);
            componentLoader.item.codeTypeString = internal.getSubtitle(codeInfo);
            componentLoader.item.isNextTypeSms = internal.isNextTypeSms(codeInfo);
            componentLoader.item.nextTypeString = internal.getNextTypeString(codeInfo);

            componentLoader.item.phoneNumber = codeInfo.phone_number;
            componentLoader.item.timeout = codeInfo.timeout * 1000;
            componentLoader.item.codeLength = internal.getCodeLength(codeInfo);

            break;
        }
        case TdApi.AuthorizationStateWaitRegistration: {
            componentLoader.sourceComponent = signUpComponent

            termsOfServiceString = Utils.getFormattedText(authorizationStateData.text);

            break;
        }
        case TdApi.AuthorizationStateWaitPassword: {
            componentLoader.sourceComponent = enterPasswordComponent;

            componentLoader.item.passwordHint = authorizationStateData.password_hint;
            break;
        }
        }
    }

}
