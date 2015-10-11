import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGNumberBox.*/

SpinBoxStyle {

    textColor: control.enabled ? palette.TextColor : palette.DisabledTextColor

    selectedTextColor: palette.HighlightTextColor

    selectionColor: palette.HighlightColor

    horizontalAlignment: Qt.AlignLeft

    renderType: Text.NativeRendering

    background: WGTextBoxFrame {

        color: !control.noFrame_ && control.enabled ? palette.TextBoxColor : "transparent"

        border.color: {
            if (control.enabled && control.activeFocus)
            {
                palette.LighterShade
            }
            else if (control.enabled && !control.noFrame && !control.activeFocus)
            {
                palette.DarkestShade
            }
            else if (control.enabled && control.noFrame && !control.activeFocus){
                "transparent"
            }
            else if (!control.enabled)
            {
                palette.DarkerShade
            }
        }
    }

    incrementControl: WGPushButton {
        radius: defaultSpacing.halfRadius
        activeFocusOnTab: false

        //reset to zero if buttons are right clicked, like in Max
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: {
                control.value = control.minimumValue
            }
        }

        Text {
            color : {
                if (parent.enabled && parent.hovered && parent.pressed && !parent.checked)
                {
                    palette.TextColor
                }
                else if (parent.enabled && parent.checked)
                {
                    palette.HighlightText0Color
                }
                else if (parent.enabled)
                {
                    palette.NeutralTextColor
                }
                else if (!parent.enabled)
                {
                    palette.DisabledTextColor
                }
            }
            font.family : "Marlett"
            font.pixelSize: 8
            renderType: Text.NativeRendering
            text : "\uF074"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: defaultSpacing.standardBorderSize
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

    decrementControl: WGPushButton {
        radius: defaultSpacing.halfRadius
        activeFocusOnTab: false

        //reset to zero if buttons are right clicked, like in Max
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.RightButton
            onClicked: {
                control.value = control.minimumValue
            }
        }

        Text {
            color : {
                if (parent.enabled && parent.hovered && parent.pressed && !parent.checked)
                {
                    palette.TextColor
                }
                else if (parent.enabled && parent.checked)
                {
                    palette.HighlightTextColor
                }
                else if (parent.enabled)
                {
                    palette.NeutralTextColor
                }
                else if (!parent.enabled)
                {
                    palette.DisabledTextColor
                }
            }
            font.family : "Marlett"
            font.pixelSize: 8
            renderType: Text.NativeRendering
            text : "\uF075"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: defaultSpacing.standardBorderSize
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
