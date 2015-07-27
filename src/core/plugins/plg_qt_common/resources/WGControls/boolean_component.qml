import QtQuick 2.3
import BWControls 1.0

WGCheckBox{
	id: checkbox
	anchors.left: parent.left
	checked: itemData.Value

	Binding {
		target: itemData
		property: "Value"
		value: checkbox.checked
	}
}
