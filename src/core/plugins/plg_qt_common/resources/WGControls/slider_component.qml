import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

//TODO: WGSlider needs to be modified so that it can use much larger range, negating the need for min/maximumValue calculations

WGSliderControl {
    id: reflectedSlider
    anchors.fill: parent

    value: itemData.Value

    //When metadata does not define a min and max value the itemData.MinValue and
    //MaxValue get set to -2147483648, and 2147483647 respectively.
    //Im not sure where this number comes from because you can represent larger numbers in QML.
    //The slider can only represent a total of maxQMLValue positions (-ve, +ve, and 0 counting as positions)
    //Minimum and maximum values outside of the maxQMLValue range set are valid.
    //Possible bug if itemData.Value is outside of maxQMLValue and metadata for min/max is not defined

    // Is there some way of deriving the following nubmers? I dislike using this magic number
    property real maxQMLValue: 2147483647
    property real minQMLValue: -2147483648

    //If more than maxQMLValue positions are detected we clamp the max min between Math.ceil(-itemData.MaxValue / 2)
    //MaxValue is used in both because MinValue is 1 higher than Max which causes slider to break.
    //Maybe I should use maxQMLValue either side of itemData.value instead?

    minimumValue: ((itemData.MaxValue - itemData.MinValue + 1) > maxQMLValue ) ? Math.ceil(-itemData.MaxValue / 2) : itemData.MinValue
    maximumValue: ((itemData.MaxValue - itemData.MinValue + 1) > maxQMLValue ) ? Math.floor(itemData.MaxValue / 2) : itemData.MaxValue

    Binding {
        target: itemData
        property: "Value"
        value: reflectedSlider.value
    }
}
