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

	// Internal bool to track what view was clicked for a selection
	property bool wasTreeClick: false

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
			onSelectionChanged: {				

				// Fetch the selected QModelIndex and then use the TreeExtension to get
				// the child model (Note: this doesn't enumerate, but could, if necessary)
				var selectedIndexValue = treeModelSelection.selectedIndex;
				var childModel = treeModelExtension.getChildModelFromIndex(selectedIndexValue);

				// Take the child model and set it as the model property on the WGListView
				// Note: Cannot put it as source to WGListModel, because it only accepts
				//       an IListModel as its source.
				testListView.model = childModel;
			}
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
				model: null
				Layout.fillHeight: true
				Layout.fillWidth: true
				selectionExtension: testListModelSelection
				columnDelegates: [defaultColumnDelegate, columnDelegate]

				Component {
					id: columnDelegate

					Item {
						Layout.fillWidth: true
						Layout.preferredHeight: testListView.minimumRowHeight

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
