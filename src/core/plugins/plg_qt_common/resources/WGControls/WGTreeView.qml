import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

WGTreeViewBase {
	id: treeView
	clip: true
	view: itemView

	property alias roles: itemView.roles

	property alias columnDelegate: itemView.columnDelegate
	property alias columnDelegates: itemView.columnDelegates
	property alias columnSequence: itemView.columnSequence
	property alias columnWidth: itemView.columnWidth
	property alias columnWidths: itemView.columnWidths
	property alias columnSpacing: itemView.columnSpacing

	property alias internalModel: treeView.model
	property alias model: itemView.model

	internalModel: itemView.extendedModel

	property var extensions: []

	WGItemViewCommon {
		id: itemView

		TreeExtension {
			id: treeExtension
		}

		property var treeExtensions: treeView.extensions.concat(commonExtensions.concat([treeExtension]))
		extensions: treeExtensions

		Connections {
			target: treeView
			onItemPressed: {
				if ((mouse.modifiers & Qt.ShiftModifier) && (mouse.modifiers & Qt.ControlModifier)) {
					var selection = treeExtension.itemSelection(itemView.selectionModel.currentIndex, rowIndex)
					itemView.selectionModel.select(selection, 0x0002) // Select
				}
				else if (mouse.modifiers & Qt.ShiftModifier) {
					var selection = treeExtension.itemSelection(itemView.selectionModel.currentIndex, rowIndex)
					itemView.selectionModel.select(selection, 0x0001 | 0x0002) // Clear || Select
				}
				else if (mouse.modifiers & Qt.ControlModifier) {
					itemView.selectionModel.setCurrentIndex(rowIndex, 0x0008) // Toggle
				}
				else {
					itemView.selectionModel.setCurrentIndex(rowIndex, 0x0001 | 0x0002) // Clear | Select
				}
			}
		}
	}
}
