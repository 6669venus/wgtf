import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Dialogs 1.2

//Raised solid color button

WGPushButton {
    id: colorButton
    property color color_: "#999999"

    //Auto-sized widths

    implicitWidth: 40

    implicitHeight: {
        if (panelProps.rowHeight_){
            panelProps.rowHeight_
        } else {
            22
        }
    }

    onClicked: {
        colorDialog.visible = true
    }

    //colour square over the top of the standard button frame.
    Rectangle {
        id: colorSquare
        anchors.fill: parent
        anchors.margins: panelProps.rowSpacing_

        opacity: enabled ? 1 : 0.4

        color: parent.color_
    }

    ColorDialog {
        id: colorDialog
        title: "Choose a Color"
        visible: false
        onAccepted: {
            color_ = colorDialog.color
        }

    }
}
