import QtQuick 2.3
import BWControls 1.0

BWCheckBox{
	id: checkbox
	anchors.left: parent.left
	anchors.right: parent.right
	height: childrenRect.height
	checked: itemData.Value
	
	Binding {
		target: itemData
		property: "Value"
		value: checkbox.checked
	}
}
