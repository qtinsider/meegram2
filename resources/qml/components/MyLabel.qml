import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0

Label {
    id: myLabel

    property alias text: textFormatter.text
    property alias formattedText: textFormatter.formattedText

    text: textFormatter.text

    TextFormatter {
        id: textFormatter
        font: myLabel.font
    }
}
