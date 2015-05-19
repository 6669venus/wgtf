import QtQuick 2.0
import QtQuick.Controls.Styles 1.2

TextFieldStyle {

    textColor: {
        if(control.enabled && !control.readOnly){
            palette.TextColor
        }else if(control.enabled && control.readOnly){
            palette.NeutralTextColor
        } else {
            palette.DisabledTextColor
        }
    }

    selectedTextColor: palette.HighlightTextColor

    selectionColor: palette.HighlightColor

    placeholderTextColor: palette.PlaceholderTextColor

    background: WGTextBoxFrame {

        //fix for input text being too high. There is a matching +1 in WGTextBox
        y: -1

        color: {
            if (!control.noFrame_ && control.enabled && !control.readOnly){
                palette.TextBoxColor
            } else {
                "transparent"
            }
        }

        border.color: {
            if (control.enabled && control.activeFocus && !control.readOnly){
                palette.HighlightColor
            } else if (control.enabled && !control.noFrame_ && !control.activeFocus && !control.readOnly) {
                palette.DarkestShade
            } else if (!control.noFrame_){
                palette.DarkerShade
            } else {
                "transparent"
            }
        }
    }
}
