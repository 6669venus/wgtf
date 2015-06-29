import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

Item {
	id: rowDelegate
	height: panelProps.rowHeight_
	anchors.left: parent.left
	anchors.right: parent.right
	property int rowIndex: index

	ListView {
		id: row
		model: ColumnModel
		anchors.fill: parent
		orientation: Qt.Horizontal

		delegate: Loader {
			id: columnDelegate

			anchors.top: parent.top
			anchors.bottom: parent.bottom
			
			property var itemData: model
			property int rowIndex: rowDelegate.rowIndex
			property int columnIndex: index

			sourceComponent:
				columnIndex < columnDelegates.length ? columnDelegates[columnIndex] :
				defaultColumnDelegate

			onLoaded: {
				var widthFunction = function()
				{
					return Math.ceil((row.width - 1) / row.count);
				}
				
				item.width = Qt.binding(widthFunction);
				rowDelegate.height = height;
			}
		}
		
		Component {
			id: defaultColumnDelegate
			Loader {
				source: "WGListViewColumnDelegate.qml"
			}
		}
	}

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
		propagateComposedEvents: true

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
