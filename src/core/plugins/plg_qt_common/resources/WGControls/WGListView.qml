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

	property alias internalModel: listView.model
	property alias model: itemView.model

	internalModel: itemView.extendedModel

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
                // Variable exists for the entire function scope
                var selection;
                if ((mouse.modifiers & Qt.ShiftModifier) && (mouse.modifiers & Qt.ControlModifier)) {
                    selection = listExtension.itemSelection(itemView.selectionModel.currentIndex, rowIndex);
                    itemView.selectionModel.select(selection,
                        ItemSelectionModel.Select);
                }
                else if (mouse.modifiers & Qt.ShiftModifier) {
                    selection = listExtension.itemSelection(itemView.selectionModel.currentIndex, rowIndex)
                    itemView.selectionModel.select(selection,
                        itemView.selectionModel.Clear | ItemSelectionModel.Select);
                }
                else if (mouse.modifiers & Qt.ControlModifier) {
                    itemView.selectionModel.setCurrentIndex(rowIndex,
                        ItemSelectionModel.Toggle);
                }
                else {
                    itemView.selectionModel.setCurrentIndex(rowIndex,
                        ItemSelectionModel.Clear | ItemSelectionModel.Select);
                }
                listView.currentModelIndex = rowIndex;
                listView.currentIndex = listExtension.indexToRow(rowIndex);
            }
        }
    }

    /*! Stores which item is currently in focus by the keyboard.
        Often this will correspond to the selected item, but not always.
        E.g. pressing ctrl+up will move the current index, but not the selected index.
        The default value is the same as the selection (modelIndex).
    */
    property var currentModelIndex: itemView.selectionModel.currentIndex

    Keys.onUpPressed: {
        // Move keyboard highlight
        currentModelIndex = listExtension.getPreviousIndex(currentModelIndex);

        // Synchronize with the index in Qt's ListView
        currentIndex = listExtension.indexToRow(currentModelIndex);

        // When Shift is pressed, selection area increases with the keyboard highlight
        if (event.modifiers & Qt.ShiftModifier) {
            var selection = listExtension.itemSelection(itemView.selectionModel.currentIndex, currentModelIndex);
            itemView.selectionModel.select(selection,
                itemView.selectionModel.Clear | ItemSelectionModel.Select);
        }

        // When Ctrl is not pressed, selection moves with the keyboard highlight
        else if (!(event.modifiers & Qt.ControlModifier)) {
            itemView.selectionModel.setCurrentIndex(currentModelIndex,
                ItemSelectionModel.Clear | ItemSelectionModel.Select);
        }
    }

    Keys.onDownPressed: {
        // Move keyboard highlight
        currentModelIndex = listExtension.getNextIndex(currentModelIndex);

        // Synchronize with the index in Qt's ListView
        currentIndex = listExtension.indexToRow(currentModelIndex);

        // When Shift is pressed, selection area increases with the keyboard highlight
        if (event.modifiers & Qt.ShiftModifier) {
            var selection = listExtension.itemSelection(itemView.selectionModel.currentIndex, currentModelIndex);
            itemView.selectionModel.select(selection,
                itemView.selectionModel.Clear | ItemSelectionModel.Select);
        }

        // When Ctrl is not pressed, selection moves with the keyboard highlight
        else if (!(event.modifiers & Qt.ControlModifier)) {
            itemView.selectionModel.setCurrentIndex(currentModelIndex,
                ItemSelectionModel.Clear | ItemSelectionModel.Select);
        }
    }
}

