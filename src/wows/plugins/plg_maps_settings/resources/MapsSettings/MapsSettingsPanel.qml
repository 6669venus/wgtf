import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
    color: palette.MainWindowColor
    property var title: "Maps Settings"
    property var layoutHints: { 'test': 0.1 }
    property var topControlsHeight: 20

	// -- Data models

	// Python objects provide the data model for the list view
    WGListModel {
        id: mapIdsModel
        source: mapIds

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        SelectionExtension {
            id: mapIdsModelSelection
			multiSelect: false
        }
    }

	// Python objects provide the data model for the tree view
    //WGListModel {
    //    id: xmlDataModel
    //    source: mapsSettingsXMLData

    //    ValueExtension {}
    //    ColumnExtension {}
    //    ComponentExtension {}
    //    SelectionExtension {
    //        id: xmlDataModelSelection
	//		multiSelect: false
    //    }
    //}
    WGTreeModel {
        id: xmlDataModel
        source: mapsSettingsXMLData

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: xmlDataModelExtension            
            selectionExtension: xmlDataModelSelection
        }
        SelectionExtension {
            id: xmlDataModelSelection
        }
    }

	// Python objects provide the data model for the tree view
    WGTreeModel {
        id: rootObjectModel
        source: rootPythonObject

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: rootObjectModelExtension            
            selectionExtension: rootObjectModelSelection
        }
        SelectionExtension {
            id: rootObjectModelSelection
        }
    }


	// -- View

	WGColumnLayout {
		id: mainColumnLayout
		anchors.fill: parent

		// Refresh button for debugging - refreshes entire tree
		WGPushButton {
			id: refreshButton
			height: topControlsHeight
			text: "Refresh"

			onClicked: {
				// Fire signal to update UI
				mapIdsChanged( mapIds );
				mapsSettingsXMLDataChanged( mapsSettingsXMLData );
				rootPythonObjectChanged( rootPythonObject );
			}
		}

		Text {
			text: "Map Ids"
            color : palette.NeutralTextColor
		}
		WGListView {
			id: mapIdsView

			model: mapIdsModel
			selectionExtension: mapIdsModelSelection
			// Implement a delegate for custom items in each column
			columnDelegates: [defaultColumnDelegate, propertyDelegate]

			Layout.minimumHeight: 50
			Layout.minimumWidth: 100
			Layout.fillHeight: true
			Layout.fillWidth: true

			// Delegate to use Reflected components for the second column.
			property Component propertyDelegate: Loader {
				clip: true
				sourceComponent: itemData != null ? itemData.Component : null
			}
		}

		Text {
			text: "Map XML Data"
            color : palette.NeutralTextColor
		}
		//WGListView {
		//	id: mapsSettingsXMLDataListView

		//	model: xmlDataModel
		//	selectionExtension: xmlDataModelSelection
		//	// Implement a delegate for custom items in each column
		//	columnDelegates: [defaultColumnDelegate, propertyDelegate]

		//	Layout.minimumHeight: 50
		//	Layout.minimumWidth: 100
		//	Layout.fillHeight: true
		//	Layout.fillWidth: true

		//	// Delegate to use Reflected components for the second column.
		//	property Component propertyDelegate: Loader {
		//		clip: true
		//		sourceComponent: itemData != null ? itemData.Component : null
		//	}
		//}

		WGTreeView {
			id: mapsSettingsXMLDataTreeView

			model: xmlDataModel
			selectionExtension: xmlDataModelSelection
			treeExtension: xmlDataModelExtension
			// Implement a delegate for custom items in each column
			//columnDelegates: [defaultColumnDelegate, propertyDelegate]

			rightMargin: 8
			childRowMargin: 2
			columnSpacing: 4
			lineSeparator: false
			autoUpdateLabelWidths: false // TODO bugged if true
			flatColourisation: false
			depthColourisation: 5

			Layout.minimumHeight: 50
			Layout.minimumWidth: 100
			Layout.fillHeight: true
			Layout.fillWidth: true

			// Delegate to use Reflected components for the second column.
			property Component propertyDelegate: Loader {
				clip: true
				sourceComponent: itemData != null ? itemData.Component : null
			}
		}

		Text {
			text: "Root Python Object"
            color : palette.NeutralTextColor
		}
		WGTreeView {
			id: rootObjectTreeView

			model: rootObjectModel
			selectionExtension: rootObjectModelSelection
			treeExtension: rootObjectModelExtension
			// Implement a delegate for custom items in each column
			columnDelegates: [defaultColumnDelegate, propertyDelegate]

			rightMargin: 8
			childRowMargin: 2
			columnSpacing: 4
			lineSeparator: false
			autoUpdateLabelWidths: false // TODO bugged if true
			flatColourisation: false
			depthColourisation: 5

			Layout.minimumHeight: 50
			Layout.minimumWidth: 100
			Layout.fillHeight: true
			Layout.fillWidth: true

			// Delegate to use Reflected components for the second column.
			property Component propertyDelegate: Loader {
				clip: true
				sourceComponent: itemData != null ? itemData.Component : null
			}
		}

	}
}
