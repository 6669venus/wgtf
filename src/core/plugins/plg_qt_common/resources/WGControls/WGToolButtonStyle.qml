import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

/*! \brief Provides custom styling for WGToolButton.
    Button border is only visible when pressed or hovered.
    Generally intended NOT to have text but can be used in cases where a frame is not desired*/

WGButtonStyle {
    id: baseStyle
    objectName: "WGToolButtonStyle"

    background: WGButtonFrame{
        id: buttonFrame

        radius: control.radius

        innerBorderColor: "transparent"
        borderColor: "transparent"
        color: "transparent"

        states: [
            State {
                name: "PRESSED"
                when: control.pressed && control.enabled
                PropertyChanges {target: buttonFrame; color: palette.DarkShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkerShade}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; highlightColor: palette.LighterShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.LightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.LighterShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.LightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: "transparent"}
                PropertyChanges {target: buttonFrame; borderColor: "transparent"}
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
            },
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus && !control.checked
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.LightestShade}
            },
            State {
                name: "ACTIVE FOCUS CHECKED"
                when: control.enabled && control.activeFocus && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkerShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.DarkerShade}
            }
        ]
    }
}
