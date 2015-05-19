import QtQuick 2.2
import QtQuick.Controls.Styles 1.2

//Slider style based on the height of the parent control

SliderStyle {
    property real lowerPos: 0
    property real upperPos: 0

        groove: WGTextBoxFrame {
            height: control.height * 0.25
            radius: panelProps.standardRadius_

            color: {
                if (control.enabled){
                    palette.TextBoxColor
                } else {
                    "transparent"
                }
            }

            border.color: {
                if (control.enabled) {
                    palette.DarkestShade
                } else if (!control.enabled){
                    palette.DarkerShade
                }
            }

            Rectangle {
                id: colouredBar
                height: {
                     if(control.orientation == Qt.Horizontal){
                         parent.height - panelProps.doubleBorder_
                     } else if (control.rangeSlider_){
                         upperPos - lowerPos
                     } else {
                         styleData.handlePosition
                     }
                }

				// x offset is control.height * 0.75 / 2 i.e. half the handle width which can't be gotten directly.

				x: control.rangeSlider_ ? lowerPos + (control.height * 0.375) : 0
                y: control.rangeSlider_ ? lowerPos + radius : 0

                width:{
                    if(control.orientation != Qt.Horizontal){
                        parent.width - panelProps.doubleBorder_
                    } else if (control.rangeSlider_){
						upperPos - lowerPos
                    } else {
                        styleData.handlePosition
                    }
               }

                anchors.verticalCenter: control.orientation == Qt.Horizontal ? parent.verticalCenter : undefined
                anchors.horizontalCenter: control.orientation != Qt.Horizontal ? parent.horizontalCenter : undefined

                radius: panelProps.standardRadius_
                color: {
                    if(control.enabled){
                        control.barColor_
                    } else {
                        palette.DarkShade
                    }
                }
            }
        }

        handle: Rectangle {
            id: slideHandle
            anchors.centerIn: parent
            color: palette.LightPanelColor
            border.width: panelProps.standardBorder_
            border.color: palette.DarkColor

            height: control.height * 0.75
            width: height
            radius: panelProps.standardRadius_

            activeFocusOnTab: true

            visible: control.enabled && !control.rangeSlider_

            //Coloured border when the slider has focus
            Rectangle {
                anchors.fill: parent
                anchors.margins: panelProps.standardBorder_
                radius: panelProps.standardRadius_
                color: "transparent"
                border.width: panelProps.standardBorder_
                border.color: {
                    if (control.enabled && control.activeFocus){
                        palette.HighlightShade
                    } else if (control.enabled && !control.activeFocus) {
                        palette.LighterShade
                    } else if (!control.enabled){
                        "transparent"
                    }
                }

            }
        }
}
