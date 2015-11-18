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

    //a large rectangle handle that fills the entire gradient groove
    property Component defaultHandle: WGButtonFrame {
        id: defaultHandleFrame
        implicitHeight: __horizontal ? control.height - 2 : 8
        implicitWidth: __horizontal ? 8 : control.width - 2
        color: control.__hoveredHandle == buttonid ? "white" : palette.OverlayLighterShade
        borderColor: palette.OverlayDarkerShade
        innerBorderColor: control.__activeHandle == buttonid && control.activeFocus ? palette.HighlightShade : "transparent"

        radius: defaultSpacing.halfRadius
    }

    //a small arrow handle that is offset below the gradient groove. It also contains a color swatch.
    //lots of magic numbers here as needed to use an icon to get the triangle shape.
    property Component arrowHandle: Item {
        implicitHeight: __horizontal ? control.height - 2 : 11
        implicitWidth: __horizontal ? 11 : control.width - 2
            Image {
                id: arrowHandleFrame
                source: "icons/arrow_handle.png"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom

                Rectangle {
                    id: colorSquare
                    height: parent.width - 4
                    width: parent.width - 4
                    anchors.bottom: parent.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.bottomMargin: 2

                    color: control.colorData[buttonid]

                    Connections {
                        target: control
                        onUpdateColorBars : {
                            colorSquare.color = control.colorData[buttonid]
                        }
                    }
                }
        }
    }

    handle: control.offsetArrowHandles ? arrowHandle : defaultHandle

    groove: Item {

        anchors.verticalCenter: __horizontal ? parent.verticalCenter : undefined
        anchors.horizontalCenter: !__horizontal ? parent.horizontalCenter : undefined

        //changing between odd and even values causes pixel 'wiggling' as the center anchors move around.
        //and can't use anchors.fill because the gradients need rotating
        height: control.height - control.height % 2
        width: control.width - control.width % 2

        WGTextBoxFrame {
            radius: defaultSpacing.halfRadius

            anchors.top: __horizontal ? parent.top : undefined
            anchors.left: !__horizontal ? parent.left : undefined

            width: {
                if (control.offsetArrowHandles)
                {
                    __horizontal ? parent.width : parent.width - 4
                }
                else
                {
                    parent.width
                }
            }
            height: {
                if (control.offsetArrowHandles)
                {
                    __horizontal ? parent.height - 4 : parent.height
                }
                else
                {
                    parent.height
                }
            }
            color: control.enabled ? palette.TextBoxColor : "transparent"

            //Item that holds the gradient
            //QML can't make horizontal gradients so this is always vertical, then possibly rotated.

            Item {
                id: gradientFrame
                anchors.centerIn: parent

                height:__horizontal ? parent.width - 2 : parent.height - 2
                width: __horizontal ? parent.height - 2 : parent.width - 2

                rotation: __horizontal ? -90 : 0

                clip: true

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
                            Layout.preferredHeight: (maximumBlockValue - minimumBlockValue) * (gradientFrame.height / (control.maximumValue - control.minimumValue))

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
