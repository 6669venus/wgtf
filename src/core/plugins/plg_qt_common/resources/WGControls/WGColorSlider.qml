import QtQuick 2.3
import QtQuick.Controls 1.2

/*!
    \brief Slider with a coloured gradient bar and arrow drag handle.
    this is a temporary control, do not use... yet
    The slider to which this provides styling is only used in WGColorPicker

\code{.js}
WGColorSlider {
    id: redSlider
    Layout.fillWidth: true
    labelBoxWidth: 10
    text: "R"
    color_: combinedColor
    channel_: "r"
}
\endcode
*/

Item {
    id: sliderFrame

    /*! This property determines the sliderStyle used by the slider
    */
    property alias style: slider.style

    /*! This property defines what sliderstyle styling component to use for this control
    */
    property alias value: slider.value

    /*! This property defines the text used within the slider label
    */
    property alias text: sliderLabel.text


    /*! This property sets the minimum value of the slider
        The default value is \c 0
    */
    property alias minimumValue: slider.minimumValue

    /*! This property sets the maximum value of the slider
        The default value is \c 255
    */
    property alias maximumValue: slider.maximumValue


    /*! This property defines the starting colour to be used in the color slider
        The default value is \c "#999999"
    */
    property color color_: "#999999"

    /*! This property defines the starting value of the red slider
    */
    property int red_: Math.round(color_.r * 255)

    /*! This property defines the starting value of the green slider
    */
    property int green_: Math.round(color_.g * 255)

    /*! This property defines the starting value of the blue slider
    */
    property int blue_: Math.round(color_.b * 255)


    /*! This property defines the starting value of the hue slider
        The default value is \c 0
    */
    property int hue_: 0

    /*! This property defines the starting value of the hue slider
        The default value is \c 0
    */
    property int sat_: 0

    /*! This property defines the starting value of the hue slider
        The default value is \c 0
    */
    property int light_: 0

    /*! This property is used as a reference to the different slider types.
        The default value is an empty string
    */
    property string channel_: ""


    /*! This property determines the width of the WGNumberBox component of the color slider
        The default value is \c 60
    */
    property int valueBoxWidth: 60

    //TODO: I dont think this property is used by anything
    /*! This property determines the width of the slider label
        The default value is \c 60
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
