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

	BWListModel {
		id: listModel
		source: sourceModel

		ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: listModelSelection
			multiSelect: true
		}
	}

	WGListView {
		anchors.fill: parent
		anchors.margins: 5
		model: listModel
		selectionExtension: listModelSelection
	}
}
