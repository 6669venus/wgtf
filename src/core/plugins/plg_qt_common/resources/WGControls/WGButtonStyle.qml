import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGNumberBox.*/

ButtonStyle {
    objectName: "WGButtonStyle"

    //invisible label as handled in button control.
    label: Text{
        text: control.text
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        visible: false
    }

    background: WGButtonFrame{
        visible: {
            if (control.noInteraction_)
            {
                false
            }
            else if (control.noFrame_ && !control.hovered && !control.pressed && !control.checked && !control.activeFocus)
            {
                false
            }
            else
            {
                true
            }
        }

        color: control.checked && !control.noInteraction_ ? palette.HighlightColor : palette.LightShade

        borderColor_: {
            if (control.enabled)
            {
                palette.DarkerShade
            }
            else if (!control.enabled)
            {
                palette.DarkShade
            }
        }

        innerBorderColor_: {
            if (control.enabled && control.pressed)
            {
                palette.DarkerShade
            }
            else if (control.enabled && !control.pressed && control.activeFocus && control.checked)
            {
                palette.DarkShade
            }
            else if (control.enabled && !control.pressed && control.activeFocus && !control.checked)
            {
                palette.HighlightShade
            }
            else if (control.enabled && !control.pressed && !control.activeFocus)
            {
                palette.LighterShade
            }
            else if (!control.enabled)
            {
                "transparent"
            }
        }

        highlightColor_: {
            if (control.pressed && !control.noInteraction_)
            {
                palette.DarkerShade
            }
            else if (control.hovered && !control.pressed && !palette.GlowStyle && !control.noInteraction_)
            {
                palette.LighterShade
            }
            else if (control.hovered && control.checked && !control.pressed && !control.noInteraction_)
            {
                palette.LightestShade
            }
            else {
                "transparent"
            }
        }

        hovered_: control.hovered

        radius_: control.radius_
    }
}
