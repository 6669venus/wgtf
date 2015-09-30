import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

/*! \brief Default button style with pressed, hovered, focused and checked states.*/

ButtonStyle {
    id: baseStyle
    objectName: "WGButtonStyle"

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.NeutralTextColor

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
            anchors.centerIn: parent
            spacing: defaultSpacing.standardMargin

            //Disabled icons are desaturated and faded.
            //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

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

                //icon only buttons are offcentre without this
                visible: control.text

                color: __textColor

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

        states: [
            State {
                name: "PRESSED"
                when: control.pressed && control.enabled
                PropertyChanges {target: buttonFrame; color: palette.DarkShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; highlightColor: palette.LighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.LighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: "transparent"}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.DarkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
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
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.DarkerShade}
            }

        ]

    }
}

/*
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
*/
