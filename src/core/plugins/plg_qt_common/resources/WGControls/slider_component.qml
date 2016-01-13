import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

WGSliderControl {
    id: reflectedSlider

    anchors.fill: parent

    //When metadata does not define a min and max value the itemData.MinValue and
    //MaxValue get set to -2147483648, and 2147483647 respectively. This is the
    //upper limit of the qml real basic type.
    //There are issues with comparing <= or >= these max values.
    minimumValue: itemData.MinValue == -2147483648 ? -1000 : itemData.minimumValue
    maximumValue: itemData.MaxValue == 2147483647 ? 1000 : itemData.maximumValue
    value: itemData.Value

    Binding {
        target: itemData
        property: "Value"
        value: reflectedSlider.value
    }
}
