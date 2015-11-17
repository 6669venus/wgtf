import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import BWControls 1.0

/*!
    A slider style that contains a color gradient as the background
*/
WGSliderStyle {
    id: sliderStyle
    objectName: "WGColorSliderStyle"

    handle: WGButtonFrame {
            id: handleFrame
            implicitHeight: __horizontal ? control.height - 2 : 8
            implicitWidth: __horizontal ? 8 : control.width - 2
            color: control.__hoveredHandle == buttonid ? "white" : palette.OverlayLighterShade
            borderColor: palette.OverlayDarkerShade
            innerBorderColor: control.__activeHandle == buttonid && control.activeFocus ? palette.HighlightShade : "transparent"

            radius: defaultSpacing.halfRadius
    }

    groove: Item {

        anchors.verticalCenter: __horizontal ? parent.verticalCenter : undefined
        anchors.horizontalCenter: !__horizontal ? parent.horizontalCenter : undefined

        height: __horizontal ? control.height : parent.width
        width: __horizontal ? parent.width : control.width

        WGTextBoxFrame {
            radius: defaultSpacing.halfRadius
            anchors.fill: parent
            color: control.enabled ? palette.TextBoxColor : "transparent"

            //Item that holds the gradient
            //QML can't make horizontal gradients so this is always vertical, then possibly rotated.

            Item {
                id: gradientFrame
                anchors.centerIn: parent

                height: (__horizontal ? parent.width : parent.height) - 2
                width: (__horizontal ? parent.height : parent.width) - 2

                rotation: __horizontal ? -90 : 0

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0
                    Repeater {
                        model: control.__colorBarModel

                        Rectangle
                        {
                            id: colorBar

                            property real minimumBlockValue: minValue
                            property real maximumBlockValue: maxValue

                            Layout.fillWidth: true
                            Layout.preferredHeight: Math.round((maximumBlockValue - minimumBlockValue) * (gradientFrame.height / (control.maximumValue - control.minimumValue)))

                            MouseArea
                            {
                                anchors.fill: parent
                                propagateComposedEvents: true

                                onPressed: {
                                    //adds handles when bar is Shift Clicked
                                    if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier) && control.addDeleteHandles)
                                    {
                                        //get the position of the mouse inside the current bar
                                        var mousePos = mapToItem(gradientFrame, mouseX, mouseY)

                                        var newPos = mousePos.y / (gradientFrame.height / (control.maximumValue - control.minimumValue))

                                        //convert hex to rgb, find the mid point, keep within RGB color values
                                        var rgbMin = control.hexToRgb(minColor)
                                        var rgbMax = control.hexToRgb(maxColor)
                                        var midColor = Qt.rgba(((rgbMin.r + rgbMax.r)/2),((rgbMin.g + rgbMax.g)/2),((rgbMin.b + rgbMax.b)/2),((rgbMin.a + rgbMax.a)/2))

                                        midColor.r = Math.min (1,midColor.r)
                                        midColor.g = Math.min (1,midColor.g)
                                        midColor.b = Math.min (1,midColor.b)
                                        midColor.a = Math.min (1,midColor.a)

                                        //add a new point to the data
                                        control.addData(index, newPos, control.rgbToHex(midColor.r,midColor.g,midColor.b,midColor.a))
                                    }
                                    else
                                    {
                                        mouse.accepted = false
                                    }
                                }
                            }

                            gradient: Gradient {
                                GradientStop {
                                    position: 0
                                    color: minColor
                                }
                                GradientStop {
                                    position: 1
                                    color: maxColor
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    bar: null
}
