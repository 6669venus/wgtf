import QtQuick 2.3
import QtQuick.Controls.Styles 1.2

/* TODO: This was marked as WIP. What needs to be done?
Lots of invisible Rectangles being used which may be excessive?
*/

/*!
    \brief Provides custom styling for WGColorSlider
    Allows for gradients to be changed as the value changes
*/

SliderStyle {
    objectName: "WGColorSliderStyle"

    property color baseColor_: "#999999"

    property int hueValue_ : 0
    property int satValue_ : 0
    property int lightValue_ : 0

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
                if (control.enabled){
                    if (control.activeFocus) {
                        palette.HighlightColor
                    } else {
                        palette.NeutralTextColor
                    }
                } else {
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
