import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0

Label {
    id: root

    property alias formattedText: content.formattedText

    text: content.text

    TextFormatter {
        id: content
        // Use the same font as the label
        font: root.font
    }
}
