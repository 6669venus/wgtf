import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

ListView {
    id: listView
    currentIndex: -1
	clip: true
	leftMargin: 2
	rightMargin: 2
	topMargin: 2
	bottomMargin: 2
	spacing: 1
	
	//TODO: Move WGPanelProperties into C++ as a context property.
    property QtObject panelProps: WGPanelProperties{}
	property var selectionExtension: null
	property var columnDelegates: []
	property real minimumRowHeight: panelProps.rowHeight_
	property real columnSpacing: 1
	property real selectionMargin: 0
	
	delegate: Loader {
		id: rowDelegateLoader
		source: "WGListViewRowDelegate.qml"
		anchors.left: parent.left
		width: parent.width - leftMargin - rightMargin - verticalScrollBar.collapsedWidth - 1
	}

	WGScrollBar {
		id: verticalScrollBar
		width: panelProps.rightMargin_
		anchors.top: listView.top
		anchors.right: listView.right
		anchors.bottom: listView.bottom
		orientation: Qt.Vertical
		position: listView.visibleArea.yPosition
		pageSize: listView.visibleArea.heightRatio
		scrollFlickable: listView
		visible: listView.contentHeight > listView.height
	}
}