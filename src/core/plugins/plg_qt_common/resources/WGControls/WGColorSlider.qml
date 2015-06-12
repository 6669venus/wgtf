import QtQuick 2.3
import QtQuick.Controls 1.2

//slider with a coloured gradient bar and arrow drag handle.
//this is a temporary control, do not use... yet

Item {
    id: sliderFrame
    property alias style: slider.style
    property alias value: slider.value
    property alias text: sliderLabel.text

    property alias minimumValue: slider.minimumValue
    property alias maximumValue: slider.maximumValue

    property color color_: "#999999"

    property int red_: Math.round(color_.r * 255)
    property int green_: Math.round(color_.g * 255)
    property int blue_: Math.round(color_.b * 255)

    property int hue_: 0
    property int sat_: 0
    property int light_: 0

    property string channel_: ""

    property int valueBoxWidth: 60
    property int labelBoxWidth: 60

    implicitHeight: {
        if (parent.rowHeight_){
            parent.rowHeight_
        } else {
            22
        }
    }

    Row {
        spacing: panelProps.standardMargin_

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
                } else if (channel_ == "b"){
                    blue_
                } else if (channel_ == "h"){
                    hue_
                } else if (channel_ == "s"){
                    sat_
                } else if (channel_ == "l"){
                    light_
                }
            }

            orientation: Qt.Horizontal
            stepSize: 1
            updateValueWhileDragging: true
            width: sliderFrame.width - valueBoxWidth - sliderLabel.width - (panelProps.standardMargin_ * 2)
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
