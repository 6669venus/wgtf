import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import BWControls 1.0


// Default row in WGMultiColumnListView
Item {
	id: rowDelegate

	// -- Begin Interface
	property int rowIndex: parent.rowIndex

	property bool isCurrentItem: parent.isCurrentItem
	property int lastIndexClicked: parent.lastIndexClicked
	// -- End Interface

	// Width set by parent
	// Height set by columnDelegateLoader
	// Choose a default height until it has loaded
	height: panelProps.rowHeight_

	// One whole row in the list
	ListView {
		id: row

		// Adapt from single row to columns within row
		model: ColumnModel

		anchors.left: parent.left
		anchors.right: parent.right
		orientation: Qt.Horizontal

		// Each column in row
		delegate: Loader {
			id: columnDelegateLoader

			// -- Begin Interface
			property variant itemData: model

			property int rowIndex: rowDelegate.rowIndex
			property int columnIndex: index

			property bool isCurrentItem: rowDelegate.isCurrentItem
			// -- End Interface

			source: columnIndex < columnDelegates.length ?
				columnDelegates[ columnIndex ] :
				defaultColumnDelegate

			onLoaded: {
				// Fill the width of the column
				// Because this is a one column, we cannot use anchors here
				item.width = Qt.binding( function() {
					return Math.ceil( row.width / row.count );
				} );
				// When we know the height of the child, set it on the row
				rowDelegate.height = Qt.binding( function(){ return height; } );
			}
		}
	}


	// TODO move mouse highlight out of row delegate
	property bool isHighlighted: false

	Rectangle {
		id: clickedHighlight

		anchors.fill: itemMouseArea

		color: isHighlighted ? palette.HighlightShade : "transparent"
		border.width: rowIndex == lastIndexClicked ? 1 : 0
		border.color: rowIndex == lastIndexClicked ? palette.HighlightColor : "transparent"
	}


	Rectangle {
		id: mouseOverHighlight 
		anchors.fill: itemMouseArea
		visible: false
		color: "#10FFFFFF"
	}


	MouseArea {
		id: itemMouseArea
		anchors.fill: parent
		hoverEnabled: true

		//Clicking this won't activate the Flickable
		preventStealing: true

		z: -1

		onPressed: {

			//control click toggles single row on or off
			if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ControlModifier)){
				if (isHighlighted){
					isHighlighted = false
				} else {
					isHighlighted = true
				}
			//Shift click multi selects.
			//Picking which index is kinda ugly. listView.contentItem.children[i].item is not great at all.
			} else if ((mouse.button == Qt.LeftButton) && (mouse.modifiers & Qt.ShiftModifier)){
				if (lastIndexClicked < rowIndex){
					for(var i = parent.lastIndexClicked + 1; i <= rowIndex; i++ ){
						if (typeof listView.contentItem.children[i].item == typeof rowDelegate) {
							listView.contentItem.children[i].item.isHighlighted = true
						}
					}
				} else {
					for(var i = rowIndex + 1; i <= lastIndexClicked; i++ ){
						if (typeof listView.contentItem.children[i].item == typeof rowDelegate) {
							listView.contentItem.children[i].item.isHighlighted = true
						}
					}
				}
				parent.isHighlighted = true
			} else {
				for(var i = 0; i < listView.contentItem.children.length; i++ ){
					if (typeof listView.contentItem.children[i].item == typeof rowDelegate) {
						listView.contentItem.children[i].item.isHighlighted = false
					}
				}
				parent.isHighlighted = true
			}
			listView.lastIndexClicked = rowIndex
		}

		//double clicking sets the current item.
		onDoubleClicked: {
			listView.currentIndex = rowIndex;
			//Model will de-select old one automatically
			model.Selected = true;
			if(!isHighlighted){
				for(var i = 0; i < listView.contentItem.children.length; i++ ){
					if (typeof listView.contentItem.children[i].item == typeof rowDelegate) {
						listView.contentItem.children[i].item.isHighlighted = false
					}
				}
				isHighlighted = true
			}
		}

		onEntered: {
			mouseOverHighlight.visible = true;
		}

		onExited: {
			mouseOverHighlight.visible = false;
		}
	}
}

