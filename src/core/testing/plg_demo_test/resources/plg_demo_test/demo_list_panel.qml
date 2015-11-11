import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

Rectangle {
	property var title: "SenceBrowser"
	property var layoutHints: { 'scenebrowser': 0.1 }
	property var sourceModel: listSource
	color: palette.MainWindowColor
	
	Label {
		id: searchBoxLabel
		x: testListView.leftMargin
		y: 2
		text: "Search:"
	}

	WGTextBox {
		id: searchBox
		y: 2
		anchors.left: searchBoxLabel.right
		anchors.right: parent.right
	}

	BWDataChangeNotifier {
        id: objectSelection
        source: CurrentIndexSource
        onDataChanged: {
            filteredListModelSelection.selectedIndex = demoListView.model.index(rootObjectIndex());
        }
    }
	
	WGFilteredListModel {
		id: filteredListModel
		source: sourceModel

		filter: WGTokenizedStringFilter {
			id: stringFilter
			filterText: searchBox.text
			splitterChar: " "
			itemRole: "Value"
		}

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: filteredListModelSelection
			multiSelect: false
		}
	}

	WGListView {
		id: demoListView
		anchors.top: searchBox.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		model: filteredListModel
		selectionExtension: filteredListModelSelection
		columnDelegates: [columnDelegate]
		Component {
			id: columnDelegate

			Item {
				Layout.fillWidth: true
				Layout.preferredHeight: demoListView.minimumRowHeight

				Text {
					clip: true
					anchors.left: parent.left
					anchors.top: parent.top
					anchors.bottom: parent.bottom
					anchors.margins: 4
					verticalAlignment: Text.AlignVCenter
					visible: true
					text: itemData != null ? itemData.Value.name : ""
					color: palette.TextColor
				}
				Connections {
					target: filteredListModelSelection
					onSelectionChanged: {
						if(itemData.Selected)
						{
							var modelIndex = filteredListModel.find( itemData.Value, "Value" );
							updateRootObject(filteredListModel.indexRow( modelIndex ));
						}
					}
				}
			}
		}
	}
}
