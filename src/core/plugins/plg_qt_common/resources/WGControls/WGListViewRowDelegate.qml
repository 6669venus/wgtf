import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0

Item {
	id: rowDelegate
	height: panelProps.rowHeight_

	property string defaultColumnDelegateFileName: "WGListViewColumnDelegate.qml"

	Loader {
		id: columnLoader
		source: defaultColumnDelegateFileName

		anchors.left: parent.left
		anchors.right: parent.right
		
		property variant itemData: model
		property int columnIndex: 0
	}

	// TODO move mouse highlight out of row delegate
	property bool isHighlighted: false

	WGHighlightFrame { 
		anchors.fill: itemMouseArea
		visible: Selected 
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
		preventStealing: true
		z: -1

		onPressed: {
			if (mouse.button == Qt.LeftButton)
			{
				var multiSelect = listView.selectionExtension != null && listView.selectionExtension.multiSelect;
				
				if (mouse.modifiers & Qt.ControlModifier)
				{
					Selected = !Selected;
				}
				else if (mouse.modifiers & Qt.ShiftModifier)
				{
					if (multiSelect)
					{
						listView.selectionExtension.prepareRangeSelect();
						Selected = true;
					}
				}
				else
				{
					Selected = true;
					
					if (multiSelect)
					{
						listView.selectionExtension.clearSelection(true);
					}
				}
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

