import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle {
	property var title: "List Test"
	property var layoutHints: { 'test': 0.1 }
	property var sourceModel: source
	color: palette.MainWindowColor

	WGListModel {
		id: listModel
		source: sourceModel

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: listModelSelection
			multiSelect: true
		}
	}

	WGListView {
		id: testListView
		anchors.fill: parent
		anchors.margins: 5
		model: listModel
		selectionExtension: listModelSelection
		columnDelegates: [columnDelegate1, columnDelegate2]

		Component {
			id: columnDelegate1
			
			Item {
				Layout.fillWidth: true
				Layout.preferredHeight: testListView.panelProps.rowHeight_

				Text {
					text: itemData.Value
					color: palette.TextColor
				}
			}
		}

		Component {
			id: columnDelegate2

			Item {
				Layout.fillWidth: true
				Layout.preferredHeight: testListView.panelProps.rowHeight_
				
				Rectangle {
					anchors.fill: parent
					visible: typeof itemData.Value != "string"
					color: {
						if (typeof itemData.Value == "string")
						{
							return "#000000";
						}
						
						var colour = itemData.Value;
						var r = colour > 9999 ? (colour / 10000) % 100 + 156 : 0;
						var g = colour > 99 ? (colour / 100) % 100 + 156 : 0;
						var b = colour % 100 + 156;
						
						return Qt.rgba(r / 255, g / 255, b / 255, 1);
					}
				}

				Text {
					visible: typeof itemData.Value == "string"
					text: typeof itemData.Value == "string" ? itemData.Value : ""
					color: palette.TextColor
				}
			}
		}
	}
}
