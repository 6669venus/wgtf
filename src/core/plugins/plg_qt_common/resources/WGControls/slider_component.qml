import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

WGSliderControl {
	id: reflectedSlider

	anchors.left: parent.left
	anchors.right: parent.right
	anchors.verticalCenter: parent.verticalCenter
	anchors.verticalCenterOffset: 1
	height: parent.height

	minimumValue: itemData.MinValue
	maximumValue: itemData.MaxValue

	Binding {
		target: itemData
		property: "Value"
		value: reflectedSlider.value
	}

	Binding {
		target: reflectedSlider
		property: "value"
		value: itemData.Value
	}
}
