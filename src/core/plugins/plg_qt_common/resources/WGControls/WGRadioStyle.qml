import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

/*!
 \brief Provides custom styling for WGRadioButton
*/

RadioButtonStyle {
    id: baseStyle
    objectName: "WGRadioStyle"

    /*! \internal */
    // helper property for text color so states can all be in the indicator object
    property color __textColor: palette.NeutralTextColor

    label: Text {
        text: control.text
        color: __textColor

        renderType: Text.NativeRendering

        //coloured border around the label when the control has focus
        Rectangle {
            height: parent.height + defaultSpacing.doubleBorderSize
            width: parent.width + defaultSpacing.standardMargin
            anchors.centerIn: parent
            visible: control.activeFocus && control.text != ""
            color: "transparent"
            radius: defaultSpacing.halfRadius
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.LighterShade
        }
    }
    indicator: WGTextBoxFrame {
        id: radioFrame
        implicitWidth: 13
        implicitHeight: 13
        radius: 7

        color: palette.TextBoxColor

        border.color: palette.DarkestShade

        states: [
            State {
                name: "UNCHECKED DISABLED"
                when: !control.checked && !control.enabled
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: radioFrame; color: "transparent"}
                PropertyChanges {target: radioFrame; border.color: palette.DarkerShade}
            },
            State {
                name: "CHECKED"
                when: control.checked && control.enabled
                PropertyChanges {target: dotContainer; visible: true}
            },
            State {
                name: "CHECKED DISABLED"
                when: control.checked && !control.enabled
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: radioFrame; color: "transparent"}
                PropertyChanges {target: radioFrame; border.color: palette.DarkerShade}
                PropertyChanges {target: dotContainer; color: palette.LightShade}
                PropertyChanges {target: dotContainer; visible: true}
            }
        ]

        Rectangle {
            id: dotContainer
            visible: false
            color: palette.HighlightColor
            radius: 7
            anchors.fill: parent
            anchors.margins:2
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.DarkerShade
        }
    }
}
