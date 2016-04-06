import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

WGPanel {
	title: "MultiColumn List Test"
	layoutHints: { 'test': 0.1 }

	property var sourceModel: source
	color: palette.mainWindowColor
	
    WGListModel {
		id: listModel
		source: sourceModel

        HeaderFooterTextExtension {}
        ValueExtension {}
		ColumnExtension {}
		SelectionExtension {
			id: listModelSelection
			multiSelect: true
		}
	}	

	WGListView {
		id: testListView
		anchors.fill: parent
		backgroundColourMode: alternatingRowBackgroundColours
	    showColumnHeaders: true
	    showColumnFooters: true
	    model: listModel
        selectionExtension: listModelSelection
		columnDelegates: [columnDelegate]
		columnSequence: [0, 0, 0]

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
					text: itemData.Value
					color: palette.textColor
				}
			}
		}
	}
}
