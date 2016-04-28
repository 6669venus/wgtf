import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

ListView {
	id: listViewBase

	property var view

	signal itemPressed(var mouse, var itemIndex, var rowIndex)
	signal itemClicked(var mouse, var itemIndex, var rowIndex)
    signal itemDoubleClicked(var mouse, var itemIndex, var rowIndex)

    delegate: WGItemRow {
		id: itemRow
        columnDelegates: view.columnDelegates
        columnSequence: view.columnSequence
		columnWidths: view.columnWidths
        columnSpacing: view.columnSpacing
		selected: view.selectionModel.isSelected(modelIndex)

		Connections {
			target: view.selectionModel
			onSelectionChanged: {
				itemRow.selected = view.selectionModel.isSelected(modelIndex)
			}
		}

        onItemPressed: listViewBase.itemPressed(mouse, itemIndex, modelIndex)
		onItemClicked: listViewBase.itemClicked(mouse, itemIndex, modelIndex)
        onItemDoubleClicked: listViewBase.itemDoubleClicked(mouse, itemIndex, modelIndex)
    }
}
