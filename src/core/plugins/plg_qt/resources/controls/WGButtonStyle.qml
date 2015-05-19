import QtQuick 2.0
import QtQuick.Controls.Styles 1.2

ButtonStyle {

    //invisible label as handled in button control.
    label: Text{
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: false
    }

    background: WGButtonFrame{
        visible: {
            if (control.noFrame_ && !control.hovered && !control.pressed && !control.checked && !control.activeFocus) {
                false
            } else {
                true
            }
        }

        color: {
            if(control.checked){
                palette.HighlightColor
            } else {
                palette.LightShade
            }
        }

        borderColor_: {
            if (control.enabled && control.checked){
                palette.HighlightColor
            } else if (control.enabled && !control.checked){
                palette.DarkerShade
            } else if (!control.enabled){
                palette.DarkShade
            }
        }

        innerBorderColor_: {
            if (control.enabled && control.pressed){
                palette.DarkerShade
            } else if (control.enabled && !control.pressed && control.activeFocus && control.checked){
                palette.DarkShade
            } else if (control.enabled && !control.pressed && control.activeFocus && !control.checked){
                palette.HighlightShade
            } else if (control.enabled && !control.pressed && !control.activeFocus){
                palette.LighterShade
            } else if (!control.enabled){
                "transparent"
            }
        }

        highlightColor_: {
            if (control.pressed){
                palette.DarkerShade
            } else if (control.hovered && !control.pressed && !palette.GlowStyle){
                palette.LighterShade
            } else if (control.hovered && control.checked &&!control.pressed){
                palette.LightestShade
            } else {
                "transparent"
            }
        }

        hovered_: control.hovered

        radius_: control.radius_
    }
}
