import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0
import BWControls 1.0

WGDropDownBox {
	id: combobox
	anchors.left: parent.left
	anchors.right: parent.right
 
	WGListModel {
		id: polyModel
		source: itemData.DefinitionModel
 
		ValueExtension {}
	}
 
	model: polyModel
	textRole: "display"
 
	Component.onCompleted: {
		var modelIndex = polyModel.find( itemData.Definition, "Value" );
		currentIndex = polyModel.indexRow( modelIndex );
	}

	Connections {
		target: combobox
		onCurrentIndexChanged: {
			var modelIndex = polyModel.index( currentIndex );
			itemData.Definition = polyModel.data( modelIndex, "Value" );
		}
	}
}