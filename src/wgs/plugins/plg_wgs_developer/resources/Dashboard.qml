import QtQuick 2.3


Item {
    width: 440
    height: separator_top.height + title_bar.height + streams.height + separator_bottom.height

    Separator{
        id: separator_top
    }

    //setting up the streams title bar
    Rectangle{
        id: title_bar
        color: "#ebedf2"
        height: 35
        width: parent.width
        anchors.top: separator_top.bottom

        Text {
            id: label_active_stream
            text: "Active Stream"
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            color: "#939598"
        }

        Text {
            id: label_last_built
            text: "Last Built"
            anchors.left: parent.left
            anchors.leftMargin: 240
            anchors.verticalCenter: parent.verticalCenter
            color: "#939598"
        }

        Text {
            id: label_status
            text: "Status"
            anchors.right: parent.right
            anchors.rightMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            color: "#939598"
        }
    }

    Rectangle {
        id: streams
        width: parent.width
        height: 70
        anchors.top: title_bar.bottom
        color: "white"

        Text {
            id: active_stream
            text: "wgs_jvln_main"
            anchors.left: parent.left
            anchors.leftMargin: 24
            anchors.verticalCenter: parent.verticalCenter
            color: "#4d4d4f"
            font.pointSize: 18
        }

        Text {
            id: last_built
            text: "11/19/2014 - 14:43"
            anchors.left: parent.left
            anchors.leftMargin: 240
            anchors.verticalCenter: parent.verticalCenter
            color: "#4d4d4f"
        }

        Image {
            id: status
            source: "images/icon_status_pass.png"
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Separator {
        id: separator_bottom
        anchors.top: streams.bottom
    }
}
