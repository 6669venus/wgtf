import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import QtQuick.Layouts 1.1
import WGControls 2.0

/*!
 \brief Slider with a large bar and invisible handles

\code{.js}

**** TODO **** {
    Layout.fillWidth: true
    minimumValue: 0
    maximumValue: 100
    lowerValue: 25
    upperValue: 75
    stepSize: 1
    enabled: true
}

\endcode
*/


WGSlider {
    objectName: "slider"
    id: slider

    activeFocusOnPress: true

    style: WGTimelineBarSliderStyle{}

    property real startFrame: 0
    property real endFrame: 1

    grooveClickable: false

    handleClamp: false

    clickAreaBuffer: 50

    property Component barContent: Rectangle {
        color: slider.barColor
    }

    WGTimelineBarSliderHandle {
        id: sliderMinHandle
        minimumValue: slider.minimumValue
        maximumValue: sliderMaxHandle.value
        showBar: false
        rangePartnerHandle: sliderMaxHandle
        value: slider.startFrame
        maxHandle: false

        onValueChanged: {
            slider.startFrame = value
        }
    }

    WGTimelineBarSliderHandle {
        id: sliderMaxHandle
        minimumValue: sliderMinHandle.value
        maximumValue: slider.maximumValue
        showBar: true
        barMinPos: (sliderMinHandle.value * (parentSlider.__clampedLength / (parentSlider.maximumValue - parentSlider.minimumValue))) + parentSlider.__visualMinPos
        rangePartnerHandle: sliderMinHandle
        value: slider.endFrame
        maxHandle: true

        onValueChanged: {
            slider.endFrame = value
        }
    }
}
