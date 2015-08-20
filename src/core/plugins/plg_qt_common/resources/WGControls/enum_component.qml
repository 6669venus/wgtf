import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0
import BWControls 1.0

WGDropDownBox {
	id: combobox
	anchors.left: parent.left
	anchors.right: parent.right
 
	WGListModel {
		id: enumModel
		source: itemData.EnumModel
 
		ValueExtension {}
	}
 
	model: enumModel
	currentIndex: itemData.Value
	textRole: "display"
 
	
	Binding {
		target: itemData
		property: "Value"
		value: combobox.currentIndex
	}
}
