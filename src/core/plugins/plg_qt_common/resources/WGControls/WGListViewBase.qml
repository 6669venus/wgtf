import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

/*!
 \brief Base class matches WGTreeViewBase's structure.
*/
ListView {
    id: listViewBase

    property var view

    /*! Propogates events from children to parents.
     */
    signal itemPressed(var mouse, var itemIndex, var rowIndex)
    signal itemClicked(var mouse, var itemIndex, var rowIndex)
    signal itemDoubleClicked(var mouse, var itemIndex, var rowIndex)

    /*! Stores which item is currently in focus by the keyboard.
        Often this will correspond to the selected item, but not always.
        E.g. pressing ctrl+up will move the current index, but not the selected index.
        The default value is the same as the selection (modelIndex).
    */
    property var currentModelIndex: view.selectionModel.currentIndex

    delegate: WGItemRow {
        id: itemRow
        columnDelegates: view.columnDelegates
        columnSequence: view.columnSequence
        columnWidths: view.columnWidths
        columnSpacing: view.columnSpacing
        selected: view.selectionModel.isSelected(modelIndex)
        isCurrent: ListView.isCurrentItem

        Connections {
            target: view.selectionModel
            onSelectionChanged: {
                itemRow.selected = view.selectionModel.isSelected(modelIndex)
            }
        }

        // mouse and modelIndex are passed as an argument by WGItemRow
        // itemIndex comes from ?
        onItemPressed: listViewBase.itemPressed(mouse, itemIndex, modelIndex)
        onItemClicked: listViewBase.itemClicked(mouse, itemIndex, modelIndex)
        onItemDoubleClicked: listViewBase.itemDoubleClicked(mouse, itemIndex, modelIndex)
    }
}
