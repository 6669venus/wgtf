import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

Item {
	id: rowDelegate
	height: minimumRowHeight
	anchors.left: parent.left
	anchors.right: parent.right
	property int rowIndex: index
	clip: true

	MouseArea {
		id: itemMouseArea
		anchors.fill: parent
		hoverEnabled: true

		onPressed: {
			if (mouse.button === Qt.LeftButton && listView.selectionExtension !== null)
			{
				var multiSelect = listView.selectionExtension.multiSelect;
				
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
		
		ListView {
			id: row
			model: ColumnModel
			anchors.fill: parent
			orientation: Qt.Horizontal
			interactive: false
			spacing: columnSpacing

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
						return Math.ceil((row.width - columnSpacing) / row.count);
					}
					
					item.width = Qt.binding(widthFunction);
					rowDelegate.height = height < minimumRowHeight ? minimumRowHeight : height;
				}
			}
			
			Component {
				id: defaultColumnDelegate
				Loader {
					source: "WGListViewColumnDelegate.qml"
				}
			}
		}
	}

	WGHighlightFrame { 
		anchors.fill: itemMouseArea
		anchors.margins: selectionMargin
		visible: listView.selectionExtension !== null && Selected
	}

	Rectangle {
		id: mouseOverHighlight
		anchors.fill: itemMouseArea
		visible: itemMouseArea.containsMouse
		color: "#10FFFFFF"
	}
}
