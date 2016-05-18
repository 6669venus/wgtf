import QtQuick 2.4
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
    property alias showColumnHeaders: itemView.showColumnHeaders
    property alias showColumnFooters: itemView.showColumnFooters

	property alias internalModel: listView.model
	property alias model: itemView.model

	internalModel: itemView.extendedModel
    header: itemView.header
    footer: itemView.footer

	property var extensions: []

	WGItemViewCommon {
		id: itemView

		ListExtension {
			id: listExtension
		}

		property var listExtensions: listView.extensions.concat(commonExtensions.concat([listExtension]))
		extensions: listExtensions

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
