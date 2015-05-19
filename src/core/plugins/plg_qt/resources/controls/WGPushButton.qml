import QtQuick 2.0
import QtQuick.Controls 1.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

//Raised button with border and label.
//Automatically snaps to the width of the label... but setting the width manually can behave strangely.
//Can have text, text+icon or just icon

Button {
    id: pushButton

    property bool noFrame_: false
    property int radius_: panelProps.standardRadius_

    property string label_: ""

    activeFocusOnTab: enabled

    //Auto-sized widths

    implicitWidth: {
        if (iconSource){
            labelText.paintedWidth + icon.width + panelProps.rowHeight_
        } else {
            labelText.paintedWidth + panelProps.rowHeight_
        }
    }

    implicitHeight: {
        if (panelProps.rowHeight_){
            panelProps.rowHeight_
        } else {
            22
        }
    }

    property alias b_Target: dataBinding.target
    property alias b_Property: dataBinding.property
    property alias b_Value: dataBinding.value

    Binding {
        id: dataBinding

    }

    RowLayout {
        id: labelFrame
        anchors.centerIn: pushButton
        spacing: panelProps.standardMargin_

        anchors {left: parent.left; right: parent.right}

        //Disabled icons are desaturated and faded.
        //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

        //Icon and label are here rather than in ButtonStyle. ImageEffects were too difficult inside the style.

        Item {
            anchors.verticalCenter: parent.verticalCenter
            opacity: pushButton.enabled ? 1 : 0.35

            width: {
                if (pushButton.iconSource){
                    icon.width
                } else {
                    0
                }
            }

            Image{
                id: icon
                anchors.verticalCenter: parent.verticalCenter
                source: pushButton.iconSource
                visible: pushButton.enabled
            }

            Desaturate {
                anchors.fill: icon
                cached: true
                source: icon
                desaturation: pushButton.enabled ? 0 : 1
            }
        }

        Text {
            id: labelText
            text: pushButton.text

            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter

            renderType: Text.NativeRendering

            //icon only buttons are offcentre without this
            visible: {
                if (pushButton.text){
                    true
                } else {
                    false
                }
            }

            color: {
                if (pushButton.enabled && pushButton.checked){
                    palette.HighlightTextColor
                } else if (pushButton.enabled && pushButton.hovered) {
                    palette.TextColor
                } else if (pushButton.enabled && pushButton.pressed) {
                    palette.TextColor
                } else if (pushButton.enabled && !pushButton.hovered && !pushButton.pressed) {
                    palette.NeutralTextColor
                } else if (!pushButton.enabled) {
                    palette.DisabledTextColor
                }
            }
        }
    }

    style: WGButtonStyle{

    }
}
