import QtQuick 1.1

MouseArea {
    id: ripple

    anchors.fill: parent
    clip: true

    property color rippleEffectColor: theme.inverted ? Qt.rgba(200, 200, 200, 0.7) : Qt.rgba(0, 0, 0, 0.4)

    Rectangle {
        id: rippleEffect
        width: 0
        height: 0
        radius: rippleEffect.width / 2
        color: rippleEffectColor
        opacity: 0
        anchors.centerIn: parent
    }

    SequentialAnimation {
        id: rippleAnimation

        ParallelAnimation {
            PropertyAnimation { target: rippleEffect; properties: "width,height"; to: ripple.width * 2; duration: 300 }
            PropertyAnimation { target: rippleEffect; property: "radius"; to: ripple.width; duration: 300 }
        }

        PropertyAnimation { target: rippleEffect; property: "opacity"; to: 0; duration: 300 }
    }

    onPressed: {
        startRippleAnimation(mouse.x, mouse.y)
    }

    onPressAndHold: {
        rippleEffect.opacity = 0.5
    }

    onReleased: {
        rippleAnimation.running = true
    }

    function startRippleAnimation(x, y) {
        rippleEffect.width = 0
        rippleEffect.height = 0
        rippleEffect.opacity = 0.5

        rippleEffect.x = x - rippleEffect.width / 2
        rippleEffect.y = y - rippleEffect.height / 2

        rippleAnimation.running = true
    }
}
