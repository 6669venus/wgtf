import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*! \brief Provides custom styling for WGSliderControl based on the height of the parent control.*/

SliderStyle {
    objectName: "WGSliderControlStyle"

    property real lowerPos: 0
    property real upperPos: 0

        groove: WGTextBoxFrame {
            height: control.height * 0.25
            radius: defaultSpacing.standardRadius

            color: control.enabled ? palette.TextBoxColor : "transparent"

            border.color: {
                if (control.enabled)
                {
                    palette.DarkestShade
                }
                else if (!control.enabled)
                {
                    palette.DarkerShade
                }
            }

            Rectangle {
                id: colouredBar
                height: {
                     if (control.orientation == Qt.Horizontal)
                     {
                         parent.height - defaultSpacing.doubleBorderSize
                     }
                     else if (control.rangeSlider_)
                     {
                         upperPos - lowerPos
                     }
                     else
                     {
                         styleData.handlePosition
                     }
                }

                // x offset is control.height * 0.75 / 2 i.e. half the handle width which can't be gotten directly.
                x: control.rangeSlider_ ? lowerPos + (control.height * 0.375) : 0
                y: control.rangeSlider_ ? lowerPos + radius : 0

                width:{
                    if (control.orientation != Qt.Horizontal)
                    {
                        parent.width - defaultSpacing.doubleBorderSize
                    }
                    else if (control.rangeSlider_)
                    {
                        upperPos - lowerPos
                    } else
                    {
                        styleData.handlePosition
                    }
               }

                anchors.verticalCenter: control.orientation == Qt.Horizontal ? parent.verticalCenter : undefined
                anchors.horizontalCenter: control.orientation != Qt.Horizontal ? parent.horizontalCenter : undefined

                radius: defaultSpacing.standardRadius
                color: control.enabled ? control.barColor_ : palette.DarkShade
            }
        }

        handle: Rectangle {
            id: slideHandle
            anchors.centerIn: parent
            color: palette.LightPanelColor
            border.width: defaultSpacing.standardBorderSize
            border.color: palette.DarkColor

            height: control.height * 0.75
            width: height
            radius: defaultSpacing.standardRadius

            activeFocusOnTab: true

            visible: control.enabled && !control.rangeSlider_

            //Coloured border when the slider has focus
            Rectangle {
                anchors.fill: parent
                anchors.margins: defaultSpacing.standardBorderSize
                radius: defaultSpacing.standardRadius
                color: "transparent"
                border.width: defaultSpacing.standardBorderSize
                border.color: {
                    if (control.enabled && control.activeFocus)
                    {
                        palette.HighlightShade
                    }
                    else if (control.enabled && !control.activeFocus)
                    {
                        palette.LighterShade
                    }
                    else if (!control.enabled)
                    {
                        "transparent"
                    }
                }
            }
        }
}
