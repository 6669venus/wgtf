import QtQuick 2.1
import QtQuick.Layouts 1.0
import QtQuick.Dialogs 1.0
import BWControls 1.0

// TODO:
// * Labels should probably be set via C++/data somehow
// * Fix magic width numbers & alignments
// * Assign proper border/rectangle colours from palette
// * Vertically align label text with number text boxes.
// * Number text boxes should be sized for 3-digit values but clip decimal places unless editing
// * Fix tab focus

RowLayout{
	id: color4Layout
	anchors.left: parent.left
	anchors.right: parent.right
    spacing: 3
    height: parent.height

    // properties to change the field label.

    property string xCaption: "R"
    property string yCaption: "G"
    property string zCaption: "B"
    property string wCaption: "A"

    ColorDialog {
        id: colorDialog
        title: "Please choose a color"
        showAlphaChannel: true
        modality: Qt.ApplicationModal
        color: getColor( itemData_.Value )

        onAccepted: {
            var vector = getVector(  colorDialog.color );
            itemData_.Value = vector;
            endUndoFrame();
        }
        onRejected: {
            abortUndoFrame();
        }
    }

    function getColor( vectorColor ){
        return Qt.rgba(
         vectorColor.x / 255, vectorColor.y / 255,
         vectorColor.z / 255, vectorColor.w / 255 )
    }

    function getVector( color ){
        return Qt.vector4d(
         color.r * 255, color.g * 255,
         color.b * 255, color.a * 255 )
    }

    Rectangle{
        id: colField
        height: parent.height
        Layout.minimumWidth: 32
        Layout.maximumWidth: 32
        color: colorDialog.color

        anchors.topMargin: 1
        anchors.bottomMargin: 1

        border.width: 1
        border.color: "#40000000"
        radius: 2

        MouseArea{
            anchors.fill: parent
            onPressed:{
                beginUndoFrame()
                colorDialog.open();
            }
        }
    }

    Text {
        id: colorLabel
        height: parent.height
        Layout.minimumWidth: 40
        Layout.maximumWidth: 40
        horizontalAlignment: Text.AlignRight
        text: xCaption + yCaption + zCaption + wCaption +":"
        color: "#FFFFFF"
        font.bold: true
    }

    Rectangle {
        id: colorText
        height: parent.height
        color: "#20000000"
        Layout.minimumWidth: childrenRect.width
        Layout.maximumWidth: childrenRect.width
        border.width: 1
        border.color: "#40000000"
        radius: 2

        RowLayout {
            Layout.minimumWidth: childrenRect.width
            Layout.maximumWidth: childrenRect.width
            spacing: 1

            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                width: 1
                color: "transparent"
            }

            BWTextField{
                id: xTextField
                Layout.minimumWidth: 32
                Layout.maximumWidth: 32
                text: itemData_.Value.x
            }

            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                width: 1
                color: "#20FFFFFF"
            }

            BWTextField{
                id: yTextField
                Layout.minimumWidth: 32
                Layout.maximumWidth: 32
                text: itemData_.Value.y
            }

            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                width: 1
                color: "#20FFFFFF"
            }

            BWTextField{
                id: zTextField
                Layout.minimumWidth: 32
                Layout.maximumWidth: 32
                text: itemData_.Value.z
            }

            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                width: 1
                color: "#20FFFFFF"
            }

            BWTextField{
                id: wTextField
                Layout.minimumWidth: 32
                Layout.maximumWidth: 32
                text: itemData_.Value.w
            }

            Rectangle {
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.topMargin: 1
                anchors.bottomMargin: 1
                width: 1
                color: "transparent"
            }

        }
    }

    Rectangle {
        color: "transparent"
        Layout.fillWidth: true
    }
}
