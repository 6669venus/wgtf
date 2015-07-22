import QtQuick 2.0

Rectangle {
    height: 60
    width: 440
    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#e11931"
        }
        GradientStop {
            position: 1
            color: "#800e1b"
        }
    }

    Image {
        id: logo_wg
        source: "images/logo_WG.png"
        anchors.left: parent.left
        anchors.leftMargin: 12
        anchors.verticalCenter: parent.verticalCenter
    }

    Rectangle{
        id: button_settings
        height: 40
        width: 40
        anchors.right: parent.right
        anchors.rightMargin: 8
        anchors.verticalCenter: parent.verticalCenter
        color: "#730d19"
        radius: 2.5
        opacity: 0

        MouseArea{
            id: button_settings_mouse_area
            anchors.fill: parent
            hoverEnabled: true
            onClicked: content.state = "settings"
            onPressed: parent.color = "red"
            onReleased: parent.color = "#730d19"

            states: State{
                name: "border_fade"; when: button_settings_mouse_area.containsMouse
                PropertyChanges {target: button_settings; opacity: 1}
            }

            transitions: [
                Transition {
                    PropertyAnimation{ property: "opacity"; duration: 150}
                }
            ]
        }
    }

    Image {
        id: icon_app_settings
        source: "images/icon_settings_gear.png"
        anchors.verticalCenter: button_settings.verticalCenter
        anchors.horizontalCenter: button_settings.horizontalCenter
    }
}
