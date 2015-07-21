import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Dialogs 1.2

//Raised solid color button

WGPushButton {
    id: colorButton
    property color color_: "#999999"

    //Auto-sized widths

    implicitWidth: 40

    implicitHeight: {
        if (defaultSpacing.minimumRowHeight){
            defaultSpacing.minimumRowHeight
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
        anchors.margins: defaultSpacing.rowSpacing

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
