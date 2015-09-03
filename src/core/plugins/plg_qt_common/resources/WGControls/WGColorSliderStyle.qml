import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/*  TODO: This was marked as WIP. What needs to be done?
    The slider to which this provides styling is only used in WGColorPicker
    Lots of invisible Rectangles being used which may be excessive?
*/

/*!
    \brief Provides custom styling for WGColorSlider
    Allows for gradients to be changed as the value changes

\code{.js}
style : WGColorSliderStyle{
    baseColor_: color_
    colorChannel_: channel_
    hueValue_: hue_
    satValue_: sat_
    lightValue_: light_
}
\endcode
*/

SliderStyle {
    objectName: "WGColorSliderStyle"

    /*! This property defines the starting colour to be used in the color slider
        The default value is \c "#999999"
    */
    property color baseColor_: "#999999"

    /*! This property defines the starting hue to be used in the color slider
        The default value is \c 0
    */
    property int hueValue_ : 0

    /*! This property defines the starting saturation to be used in the color slider
        The default value is \c 0
    */
    property int satValue_ : 0

    /*! This property defines the starting value to be used in the color slider
        The default value is \c 0
    */
    property int lightValue_ : 0

    /*! This property is used to provide different styling to different slider types
        The default value is an empty string
    */
    property string colorChannel_: ""

        groove: Item {
            height: control.height

            WGTextBoxFrame {
                height: 10
                width: parent.width - 14
                anchors.centerIn: parent
                radius: defaultSpacing.halfRadius
            }

            //QML only supports vertical gradients. Horizontal gradients need to be rotated.
            //Rotation + rounded corners causes problems when the width expands so radius = 0

            Rectangle {
                id: redGrad
                height: parent.width - 16
                width: 8
                anchors.centerIn: parent
                rotation: 90
                border.width: 0

                visible: colorChannel_ == "r" ? true : false

                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: Qt.rgba(1,baseColor_.g,baseColor_.b,1);
                    }
                    GradientStop {
                        position: 1.0
                        color: Qt.rgba(0,baseColor_.g,baseColor_.b,1);
                    }
                }
            }
            Rectangle {
                id: greenGrad
                height: parent.width - 16
                width: 8
                anchors.centerIn: parent
                rotation: 90

                border.width: 0

                visible: colorChannel_ == "g" ? true : false

                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: Qt.rgba(baseColor_.r,1,baseColor_.b,1);
                    }
                    GradientStop {
                        position: 1.0
                        color: Qt.rgba(baseColor_.r,0,baseColor_.b,1);
                    }
                }
            }
            Rectangle {
                id: blueGrad
                height: parent.width - 16
                width: 8
                anchors.centerIn: parent
                rotation: 90

                border.width: 0

                visible: colorChannel_ == "b" ? true : false

                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: Qt.rgba(baseColor_.r,baseColor_.g,1,1);
                    }
                    GradientStop {
                        position: 1.0
                        color: Qt.rgba(baseColor_.r,baseColor_.g,0,1);
                    }
                }
            }

            Rectangle {
                id: hueGrad
                height: parent.width - 16
                width: 8
                anchors.centerIn: parent
                rotation: 90

                border.width: 0

                visible: colorChannel_ == "h" ? true : false

                // hue rainbow gradient does not update depending on the value
                gradient: Gradient {
                    GradientStop {
                        position: 0.1
                        color: Qt.hsla(1,1,0.5,1);
                    }
                    GradientStop {
                        position: 0.2
                        color: Qt.hsla(0.8,1,0.5,1);
                    }
                    GradientStop {
                        position: 0.4
                        color: Qt.hsla(0.6,1,0.5,1);
                    }
                    GradientStop {
                        position: 0.6
                        color: Qt.hsla(0.4,1,0.5,1);
                    }
                    GradientStop {
                        position: 0.8
                        color: Qt.hsla(0.2,1,0.5,1);
                    }
                    GradientStop {
                        position: 1.0
                        color: Qt.hsla(0,1,0.5,1);
                    }
                }
            }
            Rectangle {
                id: satGrad
                height: parent.width - 16
                width: 8
                anchors.centerIn: parent
                rotation: 90

                border.width: 0

                visible: colorChannel_ == "s" ? true : false

                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: Qt.hsla(hueValue_/360,1,lightValue_/100,1);
                    }
                    GradientStop {
                        position: 1.0
                        color: Qt.hsla(hueValue_/360,0,lightValue_/100,1);
                    }
                }
            }
            Rectangle {
                id: lightGrad
                height: parent.width - 16
                width: 8
                anchors.centerIn: parent
                rotation: 90

                border.width: 0

                visible: colorChannel_ == "l" ? true : false

                gradient: Gradient {
                    GradientStop {
                        position: 0.0
                        color: Qt.hsla(hueValue_/360,satValue_/100,1,1);
                    }
                    GradientStop {
                        position: 0.5
                        color: Qt.hsla(hueValue_/360,satValue_/100,0.5,1);
                    }
                    GradientStop {
                        position: 1.0
                        color: Qt.hsla(hueValue_/360,satValue_/100,0,1);
                    }
                }
            }
        }
        //arrow slider
        handle: Text {
            anchors.centerIn: parent
            anchors.verticalCenterOffset:10
            color: {
                if (control.enabled)
                {
                    if (control.activeFocus)
                    {
                        palette.HighlightColor
                    }
                    else
                    {
                        palette.NeutralTextColor
                    }
                }
                else
                {
                    palette.DisabledTextColor
                }
            }

            style: Text.Outline
            styleColor: palette.DarkerShade

            font.family : "Marlett"
            font.pixelSize: 18

            renderType: Text.NativeRendering
            text : "t"

            visible: control.enabled
        }
}
