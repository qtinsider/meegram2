import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    TopBar {
        id: header
        title: app.getString("Language") + app.emptyString
    }

    ListView {
        id: listView
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
            top: header.bottom
        }

        model: LanguagePackInfoModel {
            id: languagePackModel
        }

        delegate: ListItem {
            Column {
                id: column
                anchors {
                    left: parent.left
                    leftMargin: 12
                    right: icon.left
                    rightMargin: 12
                    verticalCenter: parent.verticalCenter
                }

                Label {
                    width: parent.width
                    font.pixelSize: 26
                    font.bold: true
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    text: model.name
                }

                Label {
                    width: parent.width
                    font.pixelSize: 20
                    font.family: "Nokia Pure Light"
                    verticalAlignment: Text.AlignVCenter
                    elide: Text.ElideRight
                    text: model.nativeName
                }
            }

            Image {
                id: icon
                anchors {
                    right: parent.right
                    rightMargin: 12
                    verticalCenter: parent.verticalCenter
                }
                source: settings.languagePackId !== model.id
                    ? "image://theme/meegotouch-button-checkbox-background"
                    : "image://theme/meegotouch-button-radiobutton-background-selected"
            }

            onClicked: {
                settings.languagePackId = model.id
                settings.languagePluralId = model.pluralCode

                appWindow.pageStack.pop()
            }
        }
    }

    ScrollDecorator {
        flickableItem: listView
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: appWindow.pageStack.pop()
        }
    }
}
