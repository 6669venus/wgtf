import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGTextBox.*/

TextFieldStyle {
    id: baseStyle
    objectName: "WGTexBoxStyle"

    textColor: palette.OverlayTextColor

    /*! An undocumented (Qt) Style property required to centre the edit text properly */
    padding { top: 2 ; left: 4 ; right: 4 ; bottom: 2 }

    selectedTextColor: palette.HighlightTextColor

    selectionColor: palette.HighlightShade

    placeholderTextColor: palette.OverlayDarkShade

    background: WGTextBoxFrame {
        id: textFrame
        states: [
            State {
                name: "ACTIVE FOCUS"
                when: control.activeFocus && control.enabled && !control.readOnly
                PropertyChanges {target: textFrame; border.color: palette.HighlightShade}
            },
            State {
                name: "ACTIVE FOCUS READ ONLY"
                when: control.activeFocus && control.enabled && control.readOnly
                PropertyChanges {target: textFrame; color: palette.LightestShade}
                PropertyChanges {target: textFrame; border.color: palette.HighlightShade}
                PropertyChanges {target: baseStyle; textColor: palette.OverlayDarkerShade}
            },
            State {
                name: "READ ONLY"
                when: control.enabled && control.readOnly && !control.activeFocus
                PropertyChanges {target: textFrame; color: palette.LightestShade}
                PropertyChanges {target: textFrame; border.color: palette.OverlayDarkShade}
                PropertyChanges {target: baseStyle; textColor: palette.OverlayDarkerShade}
            },
            State {
                name: "DISABLED"
                when: !control.enabled
                PropertyChanges {target: textFrame; color: palette.LightShade}
                PropertyChanges {target: textFrame; border.color: palette.DarkestShade}
                PropertyChanges {target: baseStyle; textColor: palette.DarkestShade}
            }
        ]


        color: palette.OverlayLightShade

        border.color: palette.OverlayDarkerShade
    }
}
