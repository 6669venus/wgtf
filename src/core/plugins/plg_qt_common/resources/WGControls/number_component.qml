import QtQuick 2.3
import QtQuick.Controls 1.2

WGNumberBox {
    id: numberBox
    objectName:  itemData != null ? itemData.IndexPath : "number_component"
    anchors.left: parent.left

    number: itemData.Value
    minimumValue: itemData.MinValue
    maximumValue: itemData.MaxValue
    stepSize: itemData.StepSize
    decimals: itemData.Decimals
    readOnly: itemData.IsReadOnly

    Binding {
        target: itemData
        property: "Value"
        value: numberBox.number
    }

}

