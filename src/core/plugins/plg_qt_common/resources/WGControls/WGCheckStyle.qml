import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*  TODO:
    "IF YOU CHANGE THIS - Change the version in WGMenuStyle as well."
    Can we get a more thorough explanation of why WGMenuStyle should be changed
 */

/*! \brief Provides custom styling for WGCheckBox.*/

WGCheckBoxStyle {
    id: baseStyle
    objectName: "WGCheckStyle"

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
        id: checkboxFrame
        implicitWidth: 14
        implicitHeight: 14

        color: palette.TextBoxColor

        border.color: palette.DarkestShade

        states: [
            State {
                name: "UNCHECKED DISABLED"
                when: control.checkedState == Qt.Unchecked && !control.enabled
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: checkboxFrame; color: "transparent"}
                PropertyChanges {target: checkboxFrame; border.color: palette.DarkerShade}
            },
            State {
                name: "CHECKED"
                when: control.checkedState == Qt.Checked && control.enabled
                PropertyChanges {target: checkContainer; visible: true}
                PropertyChanges {target: tickMark; visible: true}
            },
            State {
                name: "CHECKED DISABLED"
                when: control.checkedState == Qt.Checked && !control.enabled
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: checkboxFrame; color: "transparent"}
                PropertyChanges {target: checkboxFrame; border.color: palette.DarkerShade}
                PropertyChanges {target: checkContainer; color: palette.LightShade}
                PropertyChanges {target: checkContainer; visible: true}
                PropertyChanges {target: tickMark; color: palette.LightestShade}
                PropertyChanges {target: tickMark; visible: true}
            },
            State {
                name: "PART CHECKED"
                when: control.checkedState == Qt.PartiallyChecked && control.enabled
                PropertyChanges {target: checkContainer; visible: true}
                PropertyChanges {target: checkContainer; anchors.margins: 2}
                PropertyChanges {target: checkContainer; color: palette.HighlightShade}
            },
            State {
                name: "PART CHECKED DISABLED"
                when: control.checkedState == Qt.PartiallyChecked && !control.enabled
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: checkboxFrame; color: "transparent"}
                PropertyChanges {target: checkboxFrame; border.color: palette.DarkerShade}
                PropertyChanges {target: checkContainer; visible: true}
                PropertyChanges {target: checkContainer; anchors.margins: 2}
                PropertyChanges {target: checkContainer; color: palette.LightShade}
            }
        ]

        Rectangle {
            id: checkContainer
            visible: false
            color: palette.HighlightColor
            radius: defaultSpacing.halfRadius
            anchors.fill: parent
            anchors.margins: 1

            Text {
                id : tickMark
                color : palette.HighlightTextColor
                font.family : "Marlett"
                font.pixelSize: checkboxFrame.height + defaultSpacing.standardRadius
                renderType: Text.NativeRendering
                text : "\uF061"
                visible : false
                anchors.fill: parent
                anchors.leftMargin: defaultSpacing.halfRadius
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }
}
