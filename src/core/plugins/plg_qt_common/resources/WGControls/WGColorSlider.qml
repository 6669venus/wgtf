import QtQuick 2.3
import QtQuick.Controls 1.2



/*!
    \brief Slider with a coloured gradient bar and arrow drag handle.
    this is a temporary control, do not use... yet
    The slider to which this provides styling is only used in WGColorPicker

\code{.js}
WGScrollPanel{
    childObject_:
        WGColumnLayout {
            WGInternalPanel{
                text: "SubPanel"
                clipContents_: true
                expanded_: true
                childObject_ :
                    WGColumnLayout{
                        WGTextBox {
                            width: 150
                            placeholderText: "Text Field"
                        }
                    }
            }
        }
}
\endcode
*/

Item {
    id: sliderFrame

    /*! This property
        The default value is \c
    */
    property alias style: slider.style

    /*! This property
        The default value is \c
    */
    property alias value: slider.value

    /*! This property
        The default value is \c
    */
    property alias text: sliderLabel.text


    /*! This property
        The default value is \c
    */
    property alias minimumValue: slider.minimumValue
    /*! This property
        The default value is \c
    */
    property alias maximumValue: slider.maximumValue


    /*! This property
        The default value is \c
    */
    property color color_: "#999999"


    /*! This property
        The default value is \c
    */
    property int red_: Math.round(color_.r * 255)

    /*! This property
        The default value is \c
    */
    property int green_: Math.round(color_.g * 255)

    /*! This property
        The default value is \c
    */
    property int blue_: Math.round(color_.b * 255)


    /*! This property
        The default value is \c
    */
    property int hue_: 0

    /*! This property
        The default value is \c
    */

    property int sat_: 0
    /*! This property
        The default value is \c
    */
    property int light_: 0


    /*! This property
        The default value is \c
    */
    property string channel_: ""


    /*! This property
        The default value is \c
    */
    property int valueBoxWidth: 60

    /*! This property
        The default value is \c
    */
    property int labelBoxWidth: 60

    implicitHeight: parent.rowHeight_ ? parent.rowHeight_ : 22

    Row {
        spacing: defaultSpacing.standardMargin

        WGLabel {
            id: sliderLabel
            formLabel_: false
        }

        Slider {
            id: slider
            minimumValue: 0
            maximumValue: 255

            value: {
                if (channel_ == "r") {
                    red_
                } else if (channel_ == "g") {
                    green_
                } else if (channel_ == "b") {
                    blue_
                } else if (channel_ == "h") {
                    hue_
                } else if (channel_ == "s") {
                    sat_
                } else if (channel_ == "l") {
                    light_
                }
            }

            orientation: Qt.Horizontal
            stepSize: 1
            updateValueWhileDragging: true
            width: sliderFrame.width - valueBoxWidth - sliderLabel.width - (defaultSpacing.standardMargin * 2)
            height: sliderFrame.height
            style : WGColorSliderStyle{
                baseColor_: color_
                colorChannel_: channel_
                hueValue_: hue_
                satValue_: sat_
                lightValue_: light_
            }

            onValueChanged: {
                sliderValue.value = value;
            }
        }

        WGNumberBox {
            id: sliderValue
            value: slider.value
            height: sliderFrame.height
            width: valueBoxWidth
            minimumValue: slider.minimumValue
            maximumValue: slider.maximumValue
            stepSize: slider.stepSize

            onValueChanged: {
                slider.value = value
            }
        }

    }
}
