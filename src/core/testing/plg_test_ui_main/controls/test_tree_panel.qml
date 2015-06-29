import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

Rectangle {
	property var title: "Test Panel"
	property var layoutHints: { 'test': 0.1 }

	property variant source_ : source

	color: palette.MainWindowColor

	WGTreeFilter {
		id: filter
		source: source_
		filter: searchBox.text
	}

	Label {
		id: searchBoxLabel
		x: 0
		y: 2
		text: "Search:"
	}

	BWTextField {
		id: searchBox
		y: 2
		anchors.left: searchBoxLabel.right
		anchors.right: parent.right
	}

	WGTreeModel {
		id : model

		source : filter.filteredSource

		ValueExtension {}
		ColumnExtension {}
		ComponentExtension {}
		TreeExtension {}
		ThumbnailExtension {}
		SelectionExtension {}
	}

	Item {
		anchors.top : searchBox.bottom
		anchors.left : parent.left
		anchors.right : parent.right
		anchors.bottom : parent.bottom
		TreeView {
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
}

