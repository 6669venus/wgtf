import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle {
	property var title: "List Test"
	property var layoutHints: { 'test': 0.1 }
	property var sourceModel: source
	color: palette.DarkColor

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
		model: listModel
		selectionExtension: listModelSelection
		columnDelegates: [columnDelegate, columnDelegate]

		Component {
			id: columnDelegate

			Item {
				Layout.fillWidth: true
				Layout.preferredHeight: testListView.panelProps.rowHeight_
				
				Rectangle {
					anchors.fill: parent
					color: {
						if (typeof itemData.Value == "string")
						{
							return palette.MainWindowColor;
						}
						
						var colour = itemData.Value;
						var r = colour > 9999 ? (colour / 10000) % 100 + 156 : 0;
						var g = colour > 99 ? (colour / 100) % 100 + 156 : 0;
						var b = colour % 100 + 156;
						
						return Qt.rgba(r / 255, g / 255, b / 255, 1);
					}
				}

				Text {
					clip: true
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.margins: 4
					verticalAlignment: Text.AlignVCenter
					visible: typeof itemData.Value == "string"
					text: typeof itemData.Value == "string" ? itemData.Value : ""
					color: palette.TextColor
				}
			}
		}
	}
}