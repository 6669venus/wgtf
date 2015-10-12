import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

/*! \brief Button style intended to be used over a 3D vieport with pressed, hovered, focused and checked states.*/

ButtonStyle {
    id: baseStyle
    objectName: "WGOverlayButtonStyle"

    property int pushOffset: 0

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.OverlayDarkerShade

    /*! \internal */
    // Ideally a button should not have a menu and no icon, a WGDropDownBox is preferrable in that instance
    // If it does this extra padding fixes it.
    property int __menuPadding:{
        if(control.menu !== null && control.iconSource == "")
        {
            5
        }
        else
        {
            0
        }
    }

    padding {
        top: defaultSpacing.standardRadius
        left: defaultSpacing.standardRadius + (control.iconSource != "" ? 0 : defaultSpacing.standardRadius)
        right: defaultSpacing.standardRadius + (control.text != "" ? defaultSpacing.standardRadius : 0) + __menuPadding
        bottom: defaultSpacing.standardRadius
    }

    label:
        Item {
        id: buttonLabel

        implicitWidth: labelFrame.implicitWidth
        implicitHeight: labelFrame.implicitHeight
        baselineOffset: labelFrame.y + labelText.y + labelText.baselineOffset

            Row {
            id: labelFrame
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: defaultSpacing.standardMargin

            //Disabled icons are desaturated and faded.
            //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

            anchors.verticalCenterOffset: 0 + pushOffset

            Item {
                anchors.verticalCenter: parent.verticalCenter
                opacity: control.enabled ? 1 : 0.35

                width: {
                    if (control.iconSource)
                    {
                        icon.width
                    } else
                    {
                        0
                    }
                }

                height: {
                    if (control.iconSource)
                    {
                        icon.height
                    } else
                    {
                        0
                    }
                }

                Image{
                    id: icon
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.verticalCenterOffset: __yPos
                    source: control.iconSource
                    visible: control.enabled
                }

                Desaturate {
                    anchors.fill: icon
                    cached: true
                    source: icon
                    desaturation: control.enabled ? 0 : 1
                }

                Image{
                    id: dropDownArrow
                    anchors.verticalCenter: parent.verticalCenter
                    source: "qrc:///icons/drop_down_arrow_16x16"
                    z: 1
                    visible: control.enabled && (control.menu != null) && control.iconSource != ""
                }
            }

            Text {
                id: labelText
                text: StyleHelpers.stylizeMnemonics(control.text)

                //horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter

                renderType: Text.NativeRendering
                color: __textColor
                font.bold: true

                //icon only buttons are offcentre without this
                visible: control.text

                // Ideally a button should not have a menu and no icon, a WGDropDownBox is preferrable in that instance
                // If it does... this arrow is displayed here.
                Image{
                    id: textDropDownArrow
                    anchors.horizontalCenter: parent.right
                    source: "qrc:///icons/drop_down_arrow_16x16"
                    z: 1
                    visible: control.menu != null && control.iconSource == ""
                }
            }
        }
    }

    background: WGButtonFrame{
        id: buttonFrame

        radius: control.radius
        color: palette.OverlayLightShade
        border.color: palette.OverlayDarkerShade

        states: [
            State {
                name: "PRESSED"
                when: control.pressed && control.enabled && !control.checked
                PropertyChanges {target: baseStyle; pushOffset: 1}
                PropertyChanges {target: buttonFrame; color: palette.DarkestShade}
            },

            State {
                name: "PRESSED CHECKED"
                when: control.pressed && control.enabled && control.checked
                PropertyChanges {target: baseStyle; pushOffset: 1}
                PropertyChanges {target: buttonFrame; color: palette.HighlightShade}
            },

            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.HighlightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.HighlightTextColor}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: palette.OverlayLighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.OverlayTextColor}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.LighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.HighlightTextColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: palette.LightestShade }
                PropertyChanges {target: baseStyle; __textColor: palette.DarkestShade}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.DarkestShade }
                PropertyChanges {target: baseStyle; __textColor: palette.LightestShade}
            },
            //TODO There is nothing visual that illustrates an active focus button.
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus && !control.checked
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.HighlightShade}
            },
            State {
                name: "ACTIVE FOCUS CHECKED"
                when: control.enabled && control.activeFocus && control.checked
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.HighlightColor}
                PropertyChanges {target: buttonFrame; color: palette.HighlightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.HighlightTextColor}
            }

        ]
    }
}