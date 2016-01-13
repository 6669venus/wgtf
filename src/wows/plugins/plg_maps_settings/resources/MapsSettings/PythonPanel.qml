import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

// WGPanel triggers auto-reloading when QML file is saved
WGPanel {
    color: palette.MainWindowColor
    property var title: "Python Test"
    property var layoutHints: { 'test': 0.1 }
    property var topControlsHeight: 20

	// Python objects provide the data model for the tree view
    WGTreeModel {
        id: testModel
        source: pythonObjects

        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: treeModelExtension            
            selectionExtension: treeModelSelection
        }
        SelectionExtension {
            id: treeModelSelection
        }
    }

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
				pythonObjectsChanged( pythonObjects );
			}
		}

		WGTreeView {
			id: testTreeView

			model: testModel
			selectionExtension: treeModelSelection
			treeExtension: treeModelExtension
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
