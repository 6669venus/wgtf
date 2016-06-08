.pragma library
.import QtQml.Models 2.2 as QtQmlModels

/*! \file Common functions for WGListView and WGTreeView.
 */

/*! Update selection when the keyboard highlight moves.
 */
function updateKeyboardSelection(event, newIndex, selectionModel, extension) {
    // When Shift is pressed, the selected area increases with the keyboard highlight
    if (event.modifiers & Qt.ShiftModifier) {
        // Add new item to selection
        var selection = extension.itemSelection(selectionModel.currentIndex, newIndex);
        selectionModel.select(selection,
            QtQmlModels.ItemSelectionModel.Select);

        // Move keyboard highlight to the item selected last
        selectionModel.setCurrentIndex(newIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }

    // When Ctrl is pressed, move keyboard highlight, but do not modify selection
    else if (event.modifiers & Qt.ControlModifier) {
        selectionModel.setCurrentIndex(newIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }

    // When no modifiers are pressed, selection moves with the keyboard highlight
    else {
        selectionModel.setCurrentIndex(newIndex,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
    }

}


/*! Update selection when the mouse is clicked.
 */
function itemPressed(mouse, selectionModel, extension, rowIndex) {
    var selection;
    if ((mouse.modifiers & Qt.ShiftModifier) && (mouse.modifiers & Qt.ControlModifier)) {
        selection = extension.itemSelection(selectionModel.currentIndex, rowIndex);
        selectionModel.select(selection, ItemSelectionModel.Select);
        selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }
    else if (mouse.modifiers & Qt.ShiftModifier) {
        selection = extension.itemSelection(selectionModel.currentIndex, rowIndex)
        selectionModel.select(selection,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
        selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.NoUpdate);
    }
    else if (mouse.modifiers & Qt.ControlModifier) {
        selectionModel.setCurrentIndex(rowIndex, QtQmlModels.ItemSelectionModel.Toggle);
    }
    else {
        selectionModel.setCurrentIndex(rowIndex,
            QtQmlModels.ItemSelectionModel.Clear | QtQmlModels.ItemSelectionModel.Select);
    }
}
