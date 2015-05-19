import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import BWControls 1.0

BWComboBox{
	id: combobox
	anchors.left: parent.left
	anchors.right: parent.right

	comboModel: itemData_.EnumModel
	chosenItem: itemData_.Value
	Binding {
		target: itemData_
		property: "Value"
		value: combobox.chosenItem
	}
}
