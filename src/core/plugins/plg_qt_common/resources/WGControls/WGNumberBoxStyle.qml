import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

SpinBoxStyle {

    textColor: {
        if(control.enabled){
            palette.TextColor
        } else {
            palette.DisabledTextColor
        }
    }

    selectedTextColor: palette.HighlightTextColor

    selectionColor: palette.HighlightColor

    horizontalAlignment: Qt.AlignLeft

    renderType: Text.NativeRendering

    background: WGTextBoxFrame {

        color: {
            if (!control.noFrame_ && control.enabled){
                palette.TextBoxColor
            } else {
                "transparent"
            }
        }

        border.color: {
            if (control.enabled && control.activeFocus){
                palette.HighlightColor
            } else if (control.enabled && !control.noFrame && !control.activeFocus) {
                palette.DarkestShade
            } else if (control.enabled && control.noFrame && !control.activeFocus){
                "transparent"
            } else if (!control.enabled){
                palette.DarkerShade
            }
        }
    }

    incrementControl: WGPushButton {
        radius_: defaultSpacing.halfRadius
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
                if (parent.enabled && parent.hovered && parent.pressed && !parent.checked){
                    palette.TextColor
                } else if (parent.enabled && parent.checked) {
                    palette.HighlightText0Color
                } else if (parent.enabled) {
                    palette.NeutralTextColor
                } else if (!parent.enabled) {
                    palette.DisabledTextColor
                }
            }
            font.family : "Marlett"
            font.pixelSize: 8
            renderType: Text.NativeRendering
            text : "t"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: defaultSpacing.standardBorderSize
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

    decrementControl: WGPushButton {
        radius_: defaultSpacing.halfRadius
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
                if (parent.enabled && parent.hovered && parent.pressed && !parent.checked){
                    palette.TextColor
                } else if (parent.enabled && parent.checked) {
                    palette.HighlightTextColor
                } else if (parent.enabled) {
                    palette.NeutralTextColor
                } else if (!parent.enabled) {
                    palette.DisabledTextColor
                }
            }
            font.family : "Marlett"
            font.pixelSize: 8
            renderType: Text.NativeRendering
            text : "u"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: defaultSpacing.standardBorderSize
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
}
