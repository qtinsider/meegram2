import QtQuick 1.1
import com.nokia.meego 1.1
import MyComponent 1.0
import "components"

Page {
    id: root

    orientationLock: PageOrientation.LockPortrait

    property variant model

    TopBar {
        id: header
        title: "ArchivedChats"
        isArchived: true
    }

    Item {
        anchors { bottom: parent.bottom; left: parent.left;
                  right: parent.right; top: header.bottom }

        ChatListView { model: root.model }
    }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
}
