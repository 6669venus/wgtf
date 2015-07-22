import QtQuick 2.0

Item{
    id: updates_delegate
    width: 440
    height: 65

    Rectangle{
        id: updates_delegate_header
        anchors.fill: parent
        color: "white"

        Image {
            id: app_icon
            source: "images/logo_WG.png"
            height: 48
            width: 48
            anchors.left: parent.left
            anchors.leftMargin: 16
            anchors.verticalCenter: parent.verticalCenter
        }

        Item{
            height: tool_label.height + version_label.height
            anchors.verticalCenter: parent.verticalCenter
            anchors.left: app_icon.right
            anchors.leftMargin: 16
            Text {
                id: tool_label
                text: "Despair Tool"
                color: "#4d4d4f"
                font.pointSize: 12
            }

            Text {
                id: version_label
                text: "Version 28.00.01.2761"
                anchors.top: tool_label.bottom
                color: "#939598"
                //font.pointSize: 10
            }
        }

        Button_Simple{
            id: button_update
            label: "Update"
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}

