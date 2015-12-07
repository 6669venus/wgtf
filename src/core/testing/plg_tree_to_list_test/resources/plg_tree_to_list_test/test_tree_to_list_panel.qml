import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

WGPanel {
	id: rootFrame
	title: "Tree to List Test"
	layoutHints: { 'test': 0.1 }
	property var sourceModel: source
	property var topControlsHeight: 20
	color: palette.MainWindowColor
	z: 1

	// Data Models
	WGFilteredTreeModel {
		id: testTreeModel
		source: sourceModel

		filter: WGTokenizedStringFilter {
			id: stringFilter			
			filterText: searchBox.text
			splitterChar: " "
		}

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
			id: treeModelExtension            
			selectionExtension: treeModelSelection
		}
        ThumbnailExtension {}
        SelectionExtension {
            id: treeModelSelection
        }
	}

	WGListModel {
		id: testListModel
		source: null

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: testListModelSelection
		}
	}

	// Main Layout
	ColumnLayout {
        id: viewsColumnLayout
        anchors.fill: parent
		

		WGExpandingRowLayout {
			id: searchRowLayout
			anchors.fill: parent
		
			Label {
				id: searchBoxLabel
				width: 100
				height: topControlsHeight
				text: "Search:"
			}

			WGTextBox {
				id: searchBox
				Layout.fillWidth: true
				height: topControlsHeight
			}
		}//searchRowLayout

		WGExpandingRowLayout {
			id: mainRowLayout
			width: 300
			height: 300
			WGTreeView {
				id: testTreeView
				model: testTreeModel
				Layout.fillHeight: true
				Layout.fillWidth: true
				rightMargin: 8 // leaves just enought space for conventional slider
				columnDelegates: [defaultColumnDelegate]
				selectionExtension: treeModelSelection
				treeExtension: treeModelExtension
				childRowMargin: 2
				columnSpacing: 4
				lineSeparator: false

				autoUpdateLabelWidths: true

				flatColourisation: false
				depthColourisation: 5
			}//WGTreeView

			WGListView {
				id: testListView
				model: testListModel
				Layout.fillHeight: true
				Layout.fillWidth: true
				selectionExtension: testListModelSelection
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
			}//WGListView
		}//mainRowLayout
	}//ColumnLayout
}
