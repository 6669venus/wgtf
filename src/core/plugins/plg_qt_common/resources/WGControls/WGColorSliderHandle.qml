import QtQuick 2.2
import QtQuick.Controls 1.2
import QtQuick.Controls.Private 1.0
import BWControls 1.0

/*!
 \A Slider handle intended for the WGColorSlider Control

 Generated dynamically by WGColorSlider. Don't use separately.
*/

WGSliderHandle {
    id: sliderHandle
    Binding {
        target: parentSlider
        property: "positionData[handleIndex]"
        value: sliderHandle.value
        when: parentSlider.linkColorsToHandles
    }
}
