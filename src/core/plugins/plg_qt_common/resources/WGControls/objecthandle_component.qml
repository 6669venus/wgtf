import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0
import BWControls 1.0

Item {
	id: objectHandle
	anchors.left: parent.left
	anchors.right: parent.right
 
	WGTreeModel {
		id: objectHandleModel
		source: itemData.DefinitionModel
 
        ValueExtension {}
        ColumnExtension {}
        ComponentExtension {}
        TreeExtension {
            id: objectHandleModelExtension            
            selectionExtension: objectHandleModelSelection
        }
        SelectionExtension {
            id: objectHandleModelSelection
        }
	}
 
	WGTreeView {
		anchors.fill : parent

		model: objectHandleModel
		selectionExtension: objectHandleModelSelection
		treeExtension: objectHandleModelExtension
		// Implement a delegate for custom items in each column
		columnDelegates: [defaultColumnDelegate, propertyDelegate]

		rightMargin: 8
		childRowMargin: 2
		columnSpacing: 4
		lineSeparator: false
		autoUpdateLabelWidths: false // TODO bugged if true
		flatColourisation: false
		depthColourisation: 5

		//Layout.minimumHeight: 50
		//Layout.minimumWidth: 100
		//Layout.fillHeight: true
		//Layout.fillWidth: true

		// Delegate to use Reflected components for the second column.
		property Component propertyDelegate: Loader {
			clip: true
			sourceComponent: itemData != null ? itemData.Component : null
		}
	}
}
