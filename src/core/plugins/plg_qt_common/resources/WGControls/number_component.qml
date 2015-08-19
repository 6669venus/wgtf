import QtQuick 2.3
import QtQuick.Controls 1.2

WGNumberBox {
	id: numberBox
	anchors.left: parent.left

	number: itemData.Value
	minimumValue: itemData.MinValue
	maximumValue: itemData.MaxValue

	Binding {
		target: itemData
		property: "Value"
		value: numberBox.number
	}

}

