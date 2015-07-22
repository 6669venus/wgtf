import QtQuick 2.0
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

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

        Item {
            id: tool_status
            width: tool_status_container.width + 6
            height: 20
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 28

            Item{
                id: tool_status_container
                width: icon_tool_status.width + tool_status_label.width + 4
                height: icon_tool_status.height
                anchors.verticalCenter: parent.verticalCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.horizontalCenterOffset: -3

                Image {
                    id: icon_tool_status
                    source: "images/icon_status_pass.png"
                    height: 12
                    width: 12
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    id: tool_status_label
                    text: "Up to date"
                    color: "#4d4d4f"
                    anchors.left: icon_tool_status.right
                    anchors.leftMargin: 4
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }

        ComboBox{
            width: 50
            height: 30
            anchors.right: tool_status.left
            anchors.verticalCenter: parent.verticalCenter

            model: ["Force Update","Uninstall"]

            style: ComboBoxStyle{

                label: Item{
                    id: tool_settings_container
                    width: icon_tool_settings_dropdown.width

                    Image {
                        id: icon_tool_settings_dropdown
                        source: "images/icon_settings_dropdown.png"
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.horizontalCenter: parent.horizontalCenter
                    }
                }

                background: Rectangle{
                    color: "white"
                    anchors.fill: parent
                }
            }
        }
    }
}

