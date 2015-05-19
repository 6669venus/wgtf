import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0


// Default row in WGMultiColumnListView
Item {
	id: rowDelegate

	// -- Begin Interface
	property int rowIndex: parent.rowIndex

	property bool isCurrentItem: parent.isCurrentItem
	property int lastIndexClicked: parent.lastIndexClicked
	// -- End Interface

	property bool isHighlighted: false

	// One whole row in the list
	Repeater {
		id: columnRepeater

		// Adapt from single row to columns within row
		model: ColumnModel
		anchors.fill: parent

		// One square within a row in the list
		delegate: Loader {
			id: columnDelegateLoader

			// -- Begin Interface
			property variant itemData: model

			property int rowIndex: rowDelegate.rowIndex
			property int columnIndex: index

			property bool isCurrentItem: rowDelegate.isCurrentItem
			property int lastIndexClicked: rowDelegate.lastIndexClicked
			property bool isHighlighted: rowDelegate.isHighlighted
			// -- End Interface

			anchors.fill: parent

			source: columnIndex < columnDelegates.length ?
				columnDelegates[ columnIndex ] :
				defaultColumnDelegate

		}
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

	Rectangle {
		id: mouseOverHighlight 
		anchors.fill: parent
		visible: false
		color: "#10FFFFFF"
	}
}

