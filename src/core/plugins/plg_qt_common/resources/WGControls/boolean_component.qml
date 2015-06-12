import QtQuick 2.3
import BWControls 1.0

BWCheckBox{
	id: checkbox
	anchors.left: parent.left
	anchors.right: parent.right
	height: childrenRect.height
	checked: itemData_.Value
	Binding {
		target: itemData_
		property: "Value"
		value: checkbox.checked
	}
}
