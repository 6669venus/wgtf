import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle {
    color: palette.MainWindowColor
	property var title: "Python Test"
    property var layoutHints: { 'test': 0.1 }
	property var topControlsHeight: 20

	Button {
		id: randomizeButton
		anchors.top: parent.top
		anchors.left: parent.left
		anchors.right: parent.right
		height: topControlsHeight
		text: "Update Values"

		onClicked: {
			updateValues();
			// Fire signal to update UI
			pythonObjectsChanged(pythonObjects);
		}
	}

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

    WGTreeView {
        id: testTreeView

		anchors.top: randomizeButton.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom

        model: testModel
        selectionExtension: treeModelSelection
        treeExtension: treeModelExtension
        columnDelegates: [defaultColumnDelegate, propertyDelegate]

        rightMargin: 8
        childRowMargin: 2
        columnSpacing: 4
        lineSeparator: false
        autoUpdateLabelWidths: true
        flatColourisation: false
        depthColourisation: 5

		// Delegate to use Reflected components for the second column.
        property Component propertyDelegate: Loader {
            clip: true
            sourceComponent: itemData != null ? itemData.Component : null
        }
    }
}
