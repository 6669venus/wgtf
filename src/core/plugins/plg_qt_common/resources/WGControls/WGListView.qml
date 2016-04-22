import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQml.Models 2.2
import WGControls 2.0

ListView {
	id: listView
	clip: true
	contentWidth: columnsFrame.width

	property Component columnDelegate: Text {
		text: itemData.display
		color: palette.textColor
	}
	property var columnDelegates: []
	property var columnSequence: []
	property real columnWidth: 1
	property var columnWidths: []
	property real columnSpacing: 0

	property alias roles: itemView.roles

	signal itemPressed(var mouse, var itemIndex)
	signal itemClicked(var mouse, var itemIndex)
    signal itemDoubleClicked(var mouse, var itemIndex)

	Component.onCompleted: {
		var tmp = columnDelegates
		while (tmp.length < columnCount()) {
			tmp.push(columnDelegate)
		}
		columnDelegates = tmp
	}

    delegate: WGItemRow {
		id: itemRow
        columnDelegate: listView.columnDelegate
        columnDelegates: listView.columnDelegates
        columnSequence: listView.columnSequence
		columnWidths: columnsFrame.columnWidths
        columnSpacing: listView.columnSpacing
		selected: selectionModel.isSelected(modelIndex)

		Connections {
			target: selectionModel
			onSelectionChanged: {
				itemRow.selected = selectionModel.isSelected(modelIndex)
			}
		}

        onItemPressed: {
			if ((mouse.modifiers & Qt.ShiftModifier) && (mouse.modifiers & Qt.ControlModifier)) {
				var selection = listExtension.itemSelection(selectionModel.currentIndex, modelIndex)
				selectionModel.select(selection, 0x0002) // Select
			}
			else if (mouse.modifiers & Qt.ShiftModifier) {
				var selection = listExtension.itemSelection(selectionModel.currentIndex, modelIndex)
				selectionModel.select(selection, 0x0001 | 0x0002) // Clear || Select
			}
			else if (mouse.modifiers & Qt.ControlModifier) {
				selectionModel.setCurrentIndex(modelIndex, 0x0008) // Toggle
			}
			else {
				selectionModel.setCurrentIndex(modelIndex, 0x0001 | 0x0002) // Clear | Select
			}
			listView.itemPressed(mouse, itemIndex)
		}
		onItemClicked: listView.itemClicked(mouse, itemIndex)
        onItemDoubleClicked: listView.itemDoubleClicked(mouse, itemIndex)
    }

	WGColumnsFrame {
		id: columnsFrame
		x: listView.contentItem.x + listView.originX
		y: listView.contentItem.y + listView.originY
		height: listView.contentItem.height
		columnCount: listView.columnCount()
		columnWidth: listView.columnWidth
		columnWidths: listView.columnWidths
		columnSpacing: listView.columnSpacing
		availableWidth: listView.width - Math.max(contentItem.x, 0)
	}

	WGItemView {
		id: itemView
		extensions: [listExtension, columnExtension]
	}

	ListExtension {
		id: listExtension
	}

	ColumnExtension {
		id: columnExtension
	}

	ItemSelectionModel {
		id: selectionModel
		model: listView.model
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

	property alias __internalModel: listView.model
	property alias model: itemView.model

	__internalModel: itemView.extendedModel
}
