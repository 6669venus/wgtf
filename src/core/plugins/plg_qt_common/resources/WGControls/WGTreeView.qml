import QtQuick 2.4
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

    property alias showColumnHeaders: itemView.showColumnHeaders
    property alias showColumnFooters: itemView.showColumnFooters
    /*! A list of components to be used for each header/footer column.
        Item 0 for column 0, item 1 for column 1 etc.
        If a column is not in the list, then it will default to columnHeaderDelegate/columnFooterDelegate.
        The default value is an empty list.
    */
    property alias columnHeaderDelegates: itemView.columnHeaderDelegates
    property alias columnFooterDelegates: itemView.columnFooterDelegates
    /*! The default component to be used for header/footer columns that are not specified
        by columnHeaderDelegates/columnFooterDelegates.
    */
    property alias columnHeaderDelegate: itemView.columnHeaderDelegate
    property alias columnFooterDelegate: itemView.columnFooterDelegate
    header: itemView.header
    footer: itemView.footer

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
