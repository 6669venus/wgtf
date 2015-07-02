import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import WGControls 1.0

Rectangle {
	property variant source_ : source

	color: palette.MainWindowColor

	WGTreeModel {
		id : model

		source : source_

		ValueExtension {}
		ColumnExtension {}
		ComponentExtension {}
		TreeExtension {}
		ThumbnailExtension {}
		SelectionExtension {}
	}

	TreeView {
		anchors.fill : parent

		model_ : model
		columnCount_ : 2
		property Component propertyDelegate : Loader {
			clip : true
			sourceComponent : itemData_ != null ? itemData_.Component : null
		}
		columnDelegates_ : [ columnDelegate_, propertyDelegate ]
		clampWidth_ : true
	}
}
