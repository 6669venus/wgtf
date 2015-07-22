import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0

Item {
	id: rowDelegate
	height: minimumRowHeight
	clip: true
	
	property int indentation: 0
	property int rowIndex: index
	property var defaultColumnDelegate: null
	property var columnDelegates: []
	property var selectionExtension: null

	signal doubleClicked()

	BWCopyable {
		id: copyableObject

		onDataCopied : {
			setValue( Value )
		}

		onDataPasted : {
			Value = data
		}
	}

	MouseArea {
		id: itemMouseArea
		anchors.fill: parent
		hoverEnabled: true

		onPressed: {
			if (mouse.button === Qt.LeftButton && selectionExtension != null)
			{
				var multiSelect = selectionExtension.multiSelect;
				
				if (mouse.modifiers & Qt.ControlModifier)
				{
					Selected = !Selected;
				}
				else if (mouse.modifiers & Qt.ShiftModifier)
				{
					if (multiSelect)
					{
						selectionExtension.prepareRangeSelect();
						Selected = true;
					}
				}
				else
				{
					if (multiSelect)
					{
						selectionExtension.clearOnNextSelect();
					}
					
					Selected = true;
				}
				
				if (Selected)
				{
					selectControl(copyableObject, multiSelect)
				}
				else
				{
					deselectControl(copyableObject, !multiSelect)
				}
			}
		}

		onDoubleClicked: {
			rowDelegate.doubleClicked();
		}

		Rectangle {
			id: selectionHighlight
			color: palette.HighlightShade
			anchors.fill: itemMouseArea
			anchors.margins: selectionMargin
			visible: selectionExtension != null && Selected
		}

		Rectangle {
			id: mouseOverHighlight
			anchors.fill: itemMouseArea
			visible: itemMouseArea.containsMouse
			color: palette.LighterShade
		}
		
		ListView {
			id: columns
			model: ColumnModel
			x: indentation
			width: parent.width - indentation
			anchors.top: parent.top
			anchors.bottom: parent.bottom
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
						return Math.ceil((columns.width - columnSpacing) / columns.count);
					}
					
					item.width = Qt.binding(widthFunction);
					rowDelegate.height = Math.max(height, minimumRowHeight);
				}
			}
		}
	}
}
