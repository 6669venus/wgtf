import QtQuick 2.3
import QtQuick.Window 2.1
import QtQuick.Controls 1.2
import BWControls 1.0

BWComboBox{
	id: combobox
	anchors.left: parent.left
	anchors.right: parent.right

	comboModel: itemData_.DefinitionModel
	chosenItem: itemData_.Definition
	Binding {
		target: itemData_
		property: "Definition"
		value: combobox.chosenItem
	}
}
