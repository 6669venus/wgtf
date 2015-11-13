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
                        model: control.colorBarModel

                        Rectangle
                        {
                            id: colorBar

                            property real minimumBlockValue: minValue

                            property real maximumBlockValue: maxValue

                            Layout.fillWidth: true
                            Layout.preferredHeight: Math.round((maximumBlockValue - minimumBlockValue) * (gradientFrame.height / (control.maximumValue - control.minimumValue)))

                            Text {
                                anchors.centerIn: parent
                                text: index
                                color: "white"
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
