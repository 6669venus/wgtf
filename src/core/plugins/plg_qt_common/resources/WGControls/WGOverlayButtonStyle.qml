import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

//Todo needs testing with an icon in the button

/*! \brief Default button style with pressed, hovered, focused and checked states.*/

ButtonStyle {
    id: baseStyle
    objectName: "WGOverlayButtonStyle"

    readonly property real opacityOff: 0.65
    readonly property real opacityHalf: 0.7
    readonly property real opacityOn: 0.85

    property real __opacity: opacityOff

    readonly property int pushOffset: 1
    property real __pushOffset: 0


    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: "black"

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
                opacity: __opacity

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
                    anchors.verticalCenterOffset: __pushOffset
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
                color: control.enabled ? "black" : "#303030"
                font.bold: true

                //icon only buttons are offcentre without this
                visible: control.text

                opacity: __opacity
                y: __pushOffset

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
        color: "white" //palette.LightestShade
        border.color: "black"
        opacity: __opacity

        states: [
            State {
                name: "PRESSED CHECKED"
                when: control.pressed && control.enabled && control.checkable && !control.checked
                PropertyChanges {target: baseStyle; __pushOffset: pushOffset}
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: baseStyle; __opacity: opacityOn}
            },

            State {
                name: "PRESSED"
                when: control.pressed && control.enabled
                PropertyChanges {target: baseStyle; __opacity: opacityOn}
                PropertyChanges {target: baseStyle; __pushOffset: pushOffset}
            },

            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
                PropertyChanges {target: baseStyle; __opacity: opacityHalf}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: baseStyle; __opacity: opacityOn}
                PropertyChanges {target: buttonFrame; opacity: opacityOn}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.LighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
                PropertyChanges {target: baseStyle; __opacity: opacityOn}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: "grey" }
                PropertyChanges {target: buttonFrame; color: "grey" }
                PropertyChanges {target: baseStyle; __textColor: "grey"}
                PropertyChanges {target: baseStyle; __opacity: opacityOff}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightShade}
                PropertyChanges {target: baseStyle; __textColor: palette.DisabledTextColor}
                PropertyChanges {target: baseStyle; __opacity: opacityHalf}
            },
            //TODO There is nothing visual that illustrates an active focus button.
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus && !control.checked
                PropertyChanges {target: buttonFrame; color: "white"}
            },
            State {
                name: "ACTIVE FOCUS CHECKED"
                when: control.enabled && control.activeFocus && control.checked
                PropertyChanges {target: buttonFrame; color: palette.HighlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.TextColor}
            }

        ]
    }
}
