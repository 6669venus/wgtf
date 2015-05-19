import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

// Multi-column ListView
ListView {
	id: listView

	// Default component for displaying each row
	property string defaultRowDelegate: "WGDefaultListRowDelegate.qml"

	// Default component for displaying each column
	// Override it to display custom items
	property string defaultColumnDelegate: "WGDefaultListColumnDelegate.qml"

	// Fill this list with custom components for displaying each column
	// columnDelegates will override defaultDelegate
	property variant columnDelegates: []

	//Previously clicked row for multi-select
	property int lastIndexClicked: -1

	delegate: Loader {
		id: rowDelegateLoader

		// -- Begin Interface
		property bool isCurrentItem: ListView.isCurrentItem
		property int rowIndex: index
		property int lastIndexClicked: listView.lastIndexClicked
		// -- End Interface

		anchors.left: parent.left
		anchors.right: parent.right
		height: panelProps.rowHeight_

		source: defaultRowDelegate
	}
}

