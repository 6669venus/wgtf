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
	textRole: "display"

	Component.onCompleted: {
		var modelIndex = enumModel.find( itemData.Value, "Value" );
		currentIndex = enumModel.indexRow( modelIndex );
	}

	Connections {
		target: combobox
		onCurrentIndexChanged: {
			var modelIndex = enumModel.index( currentIndex );
			itemData.Value = enumModel.data( modelIndex, "Value" );
		}
	}
}
