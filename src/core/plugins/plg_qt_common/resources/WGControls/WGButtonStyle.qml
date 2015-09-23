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
            if(control.hovered && !control.noInteraction_)
            {
                true
            }
            else if (control.checked && !control.textCheckedHighlight && !control.noInteraction_)
            {
                true
            }
            else if (control.activeFocus && !control.noInteraction_ && control.activeFocusOnPress)
            {
                true
            }
            else if (control.pressed && !control.noInteraction_)
            {
                true
            }
            else if (!control.noFrame_)
            {
                true
            }
            else
            {
                false
            }
        }

        color: {
            if(!control.textCheckedHighlight && control.checked && !control.noInteraction_)
            {
                palette.HighlightColor
            }
            else if (control.hovered && !control.noInteraction_)
            {
                palette.LightShade
            }
            else if (control.pressed && !control.noInteraction_)
            {
                palette.DarkShade
            }
            else
            {
                "transparent"
            }
        }

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
                "transparent"
            }
            else if (control.enabled && !control.pressed && control.activeFocus && control.checked)
            {
                palette.DarkShade
            }
            else if (control.enabled && !control.pressed && control.activeFocus && !control.checked && !control.hovered)
            {
                palette.LighterShade
            }
            else if (control.enabled && !control.pressed && !control.activeFocus)
            {
                palette.LightShade
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
