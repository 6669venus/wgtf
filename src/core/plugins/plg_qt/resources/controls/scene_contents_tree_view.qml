import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0

Rectangle {
	property variant source_ : source

	color: palette.MainWindowColor

	BWTreeModel {
		id : model

		source : source_

		ValueExtension {}
		ColumnExtension {}
		TreeExtension {}
		SelectionExtension {}
	}

	TreeView {
		anchors.fill : parent
		property Component textPropertyDelegate : Text {
			clip : true
			text : itemData_.Value
			color: palette.TextColor
		}
		property Component booleanPropertyDelegate : BWCheckBox {
		}
		property Component sizePropertyDelegate : Text {
			text : itemData_.Value
			color: palette.TextColor
		}

		model_ : model
		columnCount_ : 10
		columnDelegates_ : [
			textPropertyDelegate, // Asset Name
			booleanPropertyDelegate, // Visible
			booleanPropertyDelegate, // Locked
			textPropertyDelegate, // Chunk Identifier
			textPropertyDelegate, // Asset Type
			textPropertyDelegate, // Resource ID
			sizePropertyDelegate, // Triangle Count
			sizePropertyDelegate, // Texture Memory
			sizePropertyDelegate, // Vertex Memory
			sizePropertyDelegate // Primitive Count
		]
		columnWidths_ : [
			110, // Asset Name
			50, // Visible
			50, // Locked
			75, // Chunk Identifier
			50, // Asset Type
			200, // Resource ID
			20, // Triangle Count
			20, // Texture Memory
			20, // Vertex Memory
			20 // Primitive Count
		]
		clampWidth_ : false
	}
}

