import QtQuick 2.1
import QtQuick.Controls 1.0
import WGControls 1.0

WGPanel {
	title: "Custom Model Interface"
	layoutHints: { 'test': 0.1 }

	color: palette.MainWindowColor

	Column {
		id: original
		CustomModelInterfaceTestControl {
			source: Implementation1
		}

		CustomModelInterfaceTestControl {
			source: Implementation2
		}

		CustomModelInterfaceTestControl {
			source: Implementation3
		}
	}


	Column {
		id: clones
		y: original.y + original.height + 20
		CustomModelInterfaceTestControl {
			source: Implementation1
		}

		CustomModelInterfaceTestControl {
			source: Implementation2
		}

		CustomModelInterfaceTestControl {
			source: Implementation3
		}
	}


	WGTreeModel {
		id: fileModel
		source: fileSystemModel
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

		anchors.top: clones.bottom
		anchors.left: parent.left
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		model: fileModel
		selectionExtension: treeModelSelection
		treeExtension: treeModelExtension
		columnDelegates: [defaultColumnDelegate]

		rightMargin: 8
		childRowMargin: 2
		columnSpacing: 4
		lineSeparator: false
		autoUpdateLabelWidths: true
		backgroundColourMode: incrementalGroupBackgroundColours
		backgroundColourIncrements: 5
	}
}
