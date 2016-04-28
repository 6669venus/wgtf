import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

WGTreeItem {
	id: treeView
	clip: true
	contentWidth: itemView.width

	property Component columnDelegate: Text {
		text: itemData.display
		color: palette.textColor
	}
	property real columnWidth: 0

	property alias roles: itemView.roles

	Component.onCompleted: {
		var tmp = columnDelegates
		while (tmp.length < columnCount()) {
			tmp.push(columnDelegate)
		}
		columnDelegates = tmp

		var tmp = columnWidths
		while (tmp.length < columnCount()) {
			tmp.push(Math.max(columnWidth, 1))
		}
		columnWidths = tmp
	}

	WGItemView {
		id: itemView
		extensions: [treeExtension, columnExtension]

		width: columnsFrame.width

		WGColumnsFrame {
			id: columnsFrame
			x: treeView.contentItem.x + treeView.originX
			y: treeView.contentItem.y + treeView.originY
			height: treeView.contentItem.height
			columnWidths: treeView.columnWidths
			columnSpacing: treeView.columnSpacing
			availableWidth: treeView.width - Math.max(contentItem.x, 0)

			Component.onCompleted: {
				treeView.columnWidths = Qt.binding( function() { return columnsFrame.columnWidths } )
			}
		}
	}

	TreeExtension {
		id: treeExtension
	}

	ColumnExtension {
		id: columnExtension
	}

	ItemSelectionModel {
		id: selectionModel
		model: treeView.model
	}

	function rowCount() {
		return __internalModel.rowCount()
	}

	function columnCount() {
		var count = columnSequence.length
		if (count == 0) {
			count = __internalModel.columnCount()
		}
		return count
	}

	property alias __internalModel: treeView.model
	property alias model: itemView.model

	__internalModel: itemView.extendedModel
}
