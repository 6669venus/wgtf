import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGTextBox.*/

TextFieldStyle {
    objectName: "WGTexBoxStyle"

    textColor: {
        if (control.enabled && !control.readOnly)
        {
            palette.TextColor
        }
        else if (control.enabled && control.readOnly)
        {
            palette.NeutralTextColor
        }
        else
        {
            palette.DisabledTextColor
        }
    }

    /*! An undocumented (Qt) Style property required to centre the edit text properly */
    padding { top: 2 ; left: 4 ; right: 4 ; bottom: 2 }

    selectedTextColor: palette.HighlightTextColor

    selectionColor: palette.HighlightColor

    placeholderTextColor: palette.PlaceholderTextColor

    background: WGTextBoxFrame {
        color: !control.noFrame_ && control.enabled && !control.readOnly ? palette.TextBoxColor : "transparent"

        border.color: {
            if (control.enabled && control.activeFocus && !control.readOnly)
            {
                palette.LighterShade
            }
            else if (control.enabled && !control.noFrame_ && !control.activeFocus && !control.readOnly)
            {
                palette.DarkestShade
            }
            else if (!control.noFrame_)
            {
                palette.DarkerShade
            }
            else
            {
                "transparent"
            }
        }
    }
}
