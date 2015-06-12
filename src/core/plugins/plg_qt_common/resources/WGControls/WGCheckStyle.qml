import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

CheckBoxStyle {

    label: Text {
        text: control.text
        color: control.enabled ? palette.TextColor : palette.DisabledTextColor
        renderType: Text.NativeRendering

        //coloured border around the label when the control has focus
        Rectangle {
            height: parent.height + panelProps.doubleBorder_
            width: parent.width + panelProps.standardMargin_
            anchors.centerIn: parent
            visible: control.activeFocus
            color: "transparent"
            radius: panelProps.halfRadius_
            border.width: panelProps.standardBorder_
            border.color: palette.HighlightShade
        }
    }

    indicator: WGTextBoxFrame {
        id: checkboxFrame
        implicitWidth: 14
        implicitHeight: 14

        color: {
            if (!control.noFrame_ && control.enabled){
                palette.TextBoxColor
            } else {
                "transparent"
            }
        }

        border.color: {
            if (control.enabled && !control.noFrame) {
                palette.DarkestShade
            } else if (control.enabled && control.noFrame){
                "transparent"
            } else if (!control.enabled){
                palette.DarkerShade
            }
        }

        Rectangle {
            visible: control.checkedState != Qt.Unchecked // visible if checked or partially checked
            color: {
                if (control.enabled && control.checkedState == Qt.Checked){
                    palette.HighlightColor
                } else if (control.enabled && control.checkedState != Qt.Checked){
                    palette.HighlightShade
                } else if (!control.enabled){
                    palette.LightShade
                }
            }
            radius: panelProps.halfRadius_
            anchors.fill: parent
            anchors.margins: control.checkedState == Qt.Checked ? 1 : 2 //smaller dull blue square if partially checked

            Text {
                id : tickMark
                color : control.enabled ? palette.HighlightTextColor : palette.LightestShade
                font.family : "Marlett"
                font.pixelSize: checkboxFrame.height + panelProps.standardRadius_
                renderType: Text.NativeRendering
                text : "a"
                visible : control.checkedState == Qt.Checked //invisible if partially checked
                anchors.fill: parent
                anchors.leftMargin: panelProps.halfRadius_
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
