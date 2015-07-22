import QtQuick 2.0

Rectangle{
    id: button
    height: 30
    width: 100
    border.color: "#4d4d4f"
    border.width: 1
    radius: 2.5
    color: "white"

    //signal for clicking the button
    signal buttonClick()

    //property alias to set the button label text
    property alias label: button_label.text

    Text {
        id: button_label
        text: "Label"
        color: "#4d4d4f"
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
    }

    MouseArea{
        id: button_mouse_area
        anchors.fill: parent
        hoverEnabled: true
        onEntered: [parent.color = "#adadb2", button_label.color = "#4d4d4f"]
        onExited: [parent.color = "white", button_label.color = "#4d4d4f"]
        onPressed: [parent.color = "#4d4d4f", button_label.color = "#adadb2"]
        onReleased: button_mouse_area.containsMouse ? [parent.color = "#adadb2", button_label.color = "#4d4d4f"] : [parent.color = "white", button_label.color = "#4d4d4f"]
        onClicked: buttonClick()
    }
}


