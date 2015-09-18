import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

Rectangle {
	property var title: "List Test"
	property var layoutHints: { 'test': 0.1 }
	property var sourceModel: source
	color: palette.MainWindowColor
	
	Label {
		id: searchBoxLabel
		x: testListView.leftMargin
		y: 2
		text: "Search:"
	}

	BWTextField {
		id: searchBox
		y: 2
		anchors.left: searchBoxLabel.right
		anchors.right: parent.right
	}

	WGTokenizedStringFilter {
		id: stringFilter
		filterText: searchBox.text
		splitterChar: " "
	}

	WGFilteredListModel {
		id: filteredListModel
		source: sourceModel
		filter: stringFilter

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: filteredListModelSelection
			multiSelect: true
		}
	}

	WGListView {
		id: testListView
		anchors.top: searchBox.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		model: filteredListModel
		selectionExtension: filteredListModelSelection
		columnDelegates: [defaultColumnDelegate, columnDelegate]

		Component {
			id: columnDelegate

			Item {
				Layout.fillWidth: true
				Layout.preferredHeight: testListView.minimumRowHeight
				
				Rectangle {
					anchors.fill: parent
					anchors.margins: 1
					color: {
						if (typeof itemData.Value === "string")
						{
							return "transparent";
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
					visible: typeof itemData.Value === "string"
					text: typeof itemData.Value === "string" ? itemData.Value : ""
					color: palette.TextColor
				}
			}
		}
	}
}
