import QtQuick 2.3
import QtQuick.Controls.Styles 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0

/*! \brief A button style for use with WGComboBoxImage*/

ButtonStyle {
    id: baseStyle
    objectName: "WGButtonStyle"

    /*! \internal */
    // helper property for text color so states can all be in the background object
    property color __textColor: palette.neutralTextColor

    /*! \internal */
    property int __menuPadding:
    {
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
        objectName: "buttonLabel"

        implicitWidth: labelFrame.implicitWidth
        implicitHeight: labelFrame.implicitHeight
        baselineOffset: labelFrame.y + labelText.y + labelText.baselineOffset

        RowLayout {
            id: labelFrame
            height: control.height - padding.top - padding.bottom
            spacing: defaultSpacing.standardMargin

            //Disabled icons are desaturated and faded.
            //For some reason having the opacity change in the image didn't work with Desaturate so added parent Item

            // Image container
            Item {
                opacity: control.enabled ? 1 : 0.35

                width: control.iconSource ? control.height : 0
                height: control.iconSource ? parent.height : 0

                Image {
                    id: icon
                    objectName: "icon"
                    source: control.iconSource
                    visible: control.enabled
                    height: parent.height
                    width: height
                }

                Desaturate {
                    anchors.fill: icon
                    cached: true
                    source: icon
                    desaturation: control.enabled ? 0 : 1
                }
            }

            Text {
                id: labelText
                text: StyleHelpers.stylizeMnemonics(control.text)

                verticalAlignment: Text.AlignVCenter

                renderType: globalSettings.wgNativeRendering ? Text.NativeRendering : Text.QtRendering

                //icon only buttons are offcentre without this
                visible: control.text

                color: __textColor

                Image{
                    id: textDropDownArrow
                    anchors.horizontalCenter: parent.right
                    source: "icons/drop_down_arrow_16x16.png"
                    z: 1
                    visible: true // TODO... IF MORE ITEMS.. GREY control.menu != null && control.iconSource == "" && control.showMenuIndicator
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
                PropertyChanges {target: buttonFrame; color: palette.darkShade}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "CHECKED"
                when: control.checked && !control.pressed && !control.hovered && control.enabled && !control.activeFocus
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "HOVERED"
                when: control.hovered && control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "HOVERED CHECKED"
                when: control.hovered && control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: buttonFrame; highlightColor: palette.lighterShade}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
            },
            State {
                name: "DISABLED"
                when: !control.enabled && !control.checked
                PropertyChanges {target: buttonFrame; color: "transparent"}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.disabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "DISABLED CHECKED"
                when: !control.enabled && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightShade}
                PropertyChanges {target: buttonFrame; borderColor: palette.darkShade}
                PropertyChanges {target: baseStyle; __textColor: palette.disabledTextColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: "transparent"}
            },
            State {
                name: "ACTIVE FOCUS"
                when: control.enabled && control.activeFocus && !control.checked
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.lightestShade}
            },
            State {
                name: "ACTIVE FOCUS CHECKED"
                when: control.enabled && control.activeFocus && control.checked
                PropertyChanges {target: buttonFrame; color: palette.highlightColor}
                PropertyChanges {target: baseStyle; __textColor: palette.textColor}
                PropertyChanges {target: buttonFrame; innerBorderColor: palette.darkerShade}
            }
        ]
    }
}
