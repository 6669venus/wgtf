import QtQuick 2.1
import BWControls 1.0

BWTextField{
	id: textField
	anchors.left: parent.left
	anchors.right: parent.right
	text: itemData_.Value
	Binding {
		target: itemData_
		property: "Value"
		value: textField.text
	}
}

