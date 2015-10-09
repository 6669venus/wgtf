import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGTextBox.*/

TextFieldStyle {
    id: baseStyle
    objectName: "WGTexBoxStyle"

    textColor: palette.TextColor

    /*! An undocumented (Qt) Style property required to centre the edit text properly */
    padding { top: 2 ; left: 4 ; right: 4 ; bottom: 2 }

    selectedTextColor: palette.HighlightTextColor

    selectionColor: palette.HighlightColor

    placeholderTextColor: palette.PlaceholderTextColor

    background: WGTextBoxFrame {
        id: textFrame
        states: [
            State {
                name: "ACTIVE FOCUS"
                when: control.activeFocus && control.enabled && !control.readOnly
                PropertyChanges {target: textFrame; border.color: palette.LighterShade}
            },
            State {
                name: "ACTIVE FOCUS READ ONLY"
                when: control.activeFocus && control.enabled && control.readOnly
                PropertyChanges {target: textFrame; color: "transparent"}
                PropertyChanges {target: textFrame; border.color: palette.LighterShade}
                PropertyChanges {target: baseStyle; textColor: palette.NeutralTextColor}
            },
            State {
                name: "READ ONLY"
                when: control.enabled && control.readOnly && !control.activeFocus
                PropertyChanges {target: textFrame; color: "transparent"}
                PropertyChanges {target: textFrame; border.color: palette.DarkestShade}
                PropertyChanges {target: baseStyle; textColor: palette.NeutralTextColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled
                PropertyChanges {target: textFrame; border.color: palette.DarkestShade}
                PropertyChanges {target: baseStyle; textColor: palette.DisabledTextColor}
            }
        ]


        color: palette.TextBoxColor

        border.color: palette.DarkestShade
    }
}
