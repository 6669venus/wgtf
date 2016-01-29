import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

//TODO: WGSlider needs to be modified so that it can use much larger range, negating the need for min/maximumValue calculations
//TODO: How can we warn the user that no metaData has been set. A range of 2 billion is not a usable UI.

WGSliderControl {
    id: reflectedSlider
    anchors.fill: parent

    value: itemData.Value

    //When metadata MetaMinMax has not been defined the itemData.MinValue and
    //MaxValue get set to -2147483648, and 2147483647 respectively.
    //This might be due to an int or double validator.

    property bool maxSliderRangeExceeded: ((itemData.MaxValue - itemData.MinValue + 1) > 2147483647 )

    // Slider will only allow 2147483647 values of range.
    // If your slider is using these values then you need to set metadata MetaMinMax values in your .mpp files
    minimumValue: maxSliderRangeExceeded ? itemData.Value - Math.floor(maxSliderRange/2) : itemData.MinValue
    maximumValue: maxSliderRangeExceeded ? itemData.Value + Math.floor(maxSliderRange/2) : itemData.MaxValue

    Binding {
        target: itemData
        property: "Value"
        value: reflectedSlider.value
    }
}
