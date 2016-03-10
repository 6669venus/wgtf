import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import QtQuick.Window 2.1
import BWControls 1.0
import WGControls 1.0


Rectangle{
	id: root
	property var title: "Change Root Object Test"
	property var layoutHints: { 'test': 0.1 }
	color: palette.mainWindowColor
	Layout.fillWidth: true
	Layout.fillHeight: true
	property var testListModel: listSource
	property var testTreeModel: treeSource
	property variant selectedItemData: null
	signal selectItemData();

	WGListModel {
		id: listModel
		source: root.testListModel

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: listModelSelection
			multiSelect: false
		}
	}

	WGTreeModel {
		id: treeModel
		source: root.testTreeModel

		ValueExtension {}
		ColumnExtension {}
		ComponentExtension {}
		TreeExtension {}
		ThumbnailExtension {}
		SelectionExtension {
			id: treeModelSelection
			multiSelect: true
		}
	}

	SplitView{
		id: spliter
		anchors.fill: parent
		orientation: Qt.Horizontal

		handleDelegate: Rectangle {
			color: "transparent"
			width: defaultSpacing.doubleMargin
			WGSeparator {
				vertical_: true
				width: 2
				anchors.horizontalCenter: parent.horizontalCenter
				anchors.top: parent.top
				anchors.bottom: parent.bottom
			}
		}

		WGListView {
			id: testListView
			Layout.minimumWidth: 100
			Layout.fillHeight: true
			model: listModel
			selectionExtension: listModelSelection
			columnDelegates: [columnDelegate]
			Component.onCompleted: {
				listModelSelection.selectedIndex = model.index(0, 0);
			}
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
						visible: true
						text: itemData != null ? itemData.display : ""
						color: palette.textColor
					}

					Connections {
						target: listModelSelection
						onSelectionChanged: {
							if(itemData.Selected)
							{
								selectedItemData = itemData.Value;
								root.selectItemData();
							}
						}
					}

				}
			}
		}

		WGTreeView {
			id: testTreeView
			Layout.fillHeight: true
			Layout.fillWidth: true
			model: treeModel
			columnDelegates: [treeColumnDelegate, propertyDelegate]
			selectionExtension: treeModelSelection
			indentation: 4
			spacing: 1

            autoUpdateLabelWidths: true

			Component {
				id: treeColumnDelegate

				Item {
					Layout.fillWidth: true
					Layout.preferredHeight: testTreeView.minimumRowHeight

					Text {
						clip: true
						anchors.left: parent.left
						anchors.top: parent.top
						anchors.bottom: parent.bottom
						anchors.margins: 4
						verticalAlignment: Text.AlignVCenter
						visible: true
						text: itemData != null ? itemData.display : ""
						color: palette.textColor
					}

					Connections
					{
						target: root
						onSelectItemData:
						{
							if(selectedItemData != null)
							{
								itemData.RootValue = selectedItemData;
							}
						}
					}
				}
			}
		
			property Component propertyDelegate: Loader {
				clip: true
				sourceComponent: itemData != null ? itemData.Component : null
			}	
		}
	}
}
