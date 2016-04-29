import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

WGListViewBase {
	id: listView
	clip: true
	view: itemView

	property alias roles: itemView.roles

	property alias columnDelegate: itemView.columnDelegate
	property alias columnDelegates: itemView.columnDelegates
	property alias columnSequence: itemView.columnSequence
	property alias columnWidth: itemView.columnWidth
	property alias columnWidths: itemView.columnWidths
	property alias columnSpacing: itemView.columnSpacing

	property alias __internalModel: listView.model
	property alias model: itemView.model

	__internalModel: itemView.extendedModel

	WGItemViewCommon {
		id: itemView

		extensions: [listExtension, columnExtension, imageExtension]

		ListExtension {
			id: listExtension
		}

		ColumnExtension {
			id: columnExtension
		}

		ImageExtension {
			id: imageExtension
		}

		Connections {
			target: listView
			onItemPressed: {
				if ((mouse.modifiers & Qt.ShiftModifier) && (mouse.modifiers & Qt.ControlModifier)) {
					var selection = listExtension.itemSelection(itemView.selectionModel.currentIndex, rowIndex)
					itemView.selectionModel.select(selection, 0x0002) // Select
				}
				else if (mouse.modifiers & Qt.ShiftModifier) {
					var selection = listExtension.itemSelection(itemView.selectionModel.currentIndex, rowIndex)
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
