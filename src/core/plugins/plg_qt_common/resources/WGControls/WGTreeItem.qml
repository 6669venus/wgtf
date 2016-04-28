import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQml.Models 2.2
import WGControls 2.0

ListView {
	id: treeItem

	property real depth: 0

	property var columnDelegates: []
	property var columnSequence: []
	property var columnWidths: []
	property real columnSpacing: 0

	signal itemPressed(var mouse, var itemIndex)
	signal itemClicked(var mouse, var itemIndex)
    signal itemDoubleClicked(var mouse, var itemIndex)

	interactive: false

    delegate: Item {
		height: childrenRect.height
		width: childrenRect.width

		WGItemRow {
			id: itemRow
			columnDelegates: treeItem.columnDelegates
			columnSequence: treeItem.columnSequence
			columnWidths: treeItem.columnWidths
			columnSpacing: treeItem.columnSpacing
			selected: false

			onItemPressed: treeItem.itemPressed(mouse, itemIndex)
			onItemClicked: treeItem.itemClicked(mouse, itemIndex)
			onItemDoubleClicked: treeItem.itemDoubleClicked(mouse, itemIndex)
		}

		Item {
			id: childItems
			anchors.top: itemRow.bottom

			Loader {
				active: hasChildren && expanded

				Component.onCompleted: {
					setSource("WGTreeItem.qml", {
						"width": Qt.binding( function() { return treeItem.width - childItems.x } ),
						"height": Qt.binding( function() { return treeItem.height - childItems.y + treeItem.contentY } ),
						"depth": Qt.binding( function() { return treeItem.depth + 1 } ),
						"testY": Qt.binding( function() { return testY } ),

						"model": Qt.binding( function() { return childModel } ),
						"columnDelegates": Qt.binding( function() { return treeItem.columnDelegates } ),
						"columnSequence": Qt.binding( function() { return treeItem.columnSequence } ),
						"columnWidths": Qt.binding( function() { return treeItem.columnWidths } ),
						"columnSpacing": Qt.binding( function() { return treeItem.columnSpacing } ),

						"onItemPressed": Qt.binding( function(mouse, itemIndex) { treeItem.itemPressed(mouse, itemIndex) } ),
						"onItemClicked": Qt.binding( function(mouse, itemIndex) { treeItem.itemClicked(mouse, itemIndex) } ),
						"onItemDoubleClicked": Qt.binding( function(mouse, itemIndex) { treeItem.itemDoubleClicked(mouse, itemIndex) } )
					})

					childItems.width = Qt.binding( function() { return active ? item.contentWidth : 0 } )
					childItems.height = Qt.binding( function() { return active ? item.contentHeight : 0 } )
				}
			}
		}
	}
}
