import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*  TODO:
    "IF YOU CHANGE THIS - Change the version in WGMenuStyle as well."
    Can we get a more thorough explanation of why WGMenuStyle should be changed
 */

/*! \brief Provides custom styling for WGCheckBox.*/

CheckBoxStyle {
    objectName: "WGCheckStyle"

    label: Text {
        text: control.text
        color: control.enabled ? palette.TextColor : palette.DisabledTextColor
        renderType: Text.NativeRendering

        //coloured border around the label when the control has focus
        Rectangle {
            height: parent.height + defaultSpacing.doubleBorderSize
            width: parent.width + defaultSpacing.standardMargin
            anchors.centerIn: parent
            visible: control.activeFocus && control.text != ""
            color: "transparent"
            radius: defaultSpacing.halfRadius
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.HighlightShade
        }
    }

    indicator: WGTextBoxFrame {
        id: checkboxFrame
        implicitWidth: 14
        implicitHeight: 14

        color: !control.noFrame_ && control.enabled ? palette.TextBoxColor : "transparent"

        border.color: {
            if (control.enabled && !control.noFrame)
            {
                palette.DarkestShade
            }
            else if (control.enabled && control.noFrame)
            {
                "transparent"
            }
            else if (!control.enabled)
            {
                palette.DarkerShade
            }
        }

        Rectangle {
            visible: control.checkedState != Qt.Unchecked // visible if checked or partially checked
            color: {
                if (control.enabled && control.checkedState == Qt.Checked)
                {
                    palette.HighlightColor
                }
                else if (control.enabled && control.checkedState != Qt.Checked)
                {
                    palette.HighlightShade
                }
                else if (!control.enabled)
                {
                    palette.LightShade
                }
            }
            radius: defaultSpacing.halfRadius
            anchors.fill: parent
            anchors.margins: control.checkedState == Qt.Checked ? 1 : 2 //smaller dull blue square if partially checked

            Text {
                id : tickMark
                color : control.enabled ? palette.HighlightTextColor : palette.LightestShade
                font.family : "Marlett"
                font.pixelSize: checkboxFrame.height + defaultSpacing.standardRadius
                renderType: Text.NativeRendering
                text : "a"
                visible : control.checkedState == Qt.Checked //invisible if partially checked
                anchors.fill: parent
                anchors.leftMargin: defaultSpacing.halfRadius
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
