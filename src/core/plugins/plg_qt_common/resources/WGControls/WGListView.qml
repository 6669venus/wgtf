import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

ListView {
    id: listView
    currentIndex: -1
	
	//TODO: Move into C++ as a context property.
    property QtObject panelProps: WGPanelProperties{}
	
	property var selectionExtension: null
	
	delegate: Loader {
		id: rowDelegateLoader
		source: "WGListViewRowDelegate.qml"
		anchors.left: parent.left
		anchors.right: parent.right
	}

	WGScrollBar {
		 id: verticalScrollBar
		 width: panelProps.rightMargin_
		 height: listView.height
		 anchors.right: listView.right
		 anchors.bottom: listView.bottom
		 orientation: Qt.Vertical
		 position: listView.visibleArea.yPosition
		 pageSize: listView.visibleArea.heightRatio

		 scrollFlickable: listView

		 visible: listView.contentHeight > listView.height
	 }
}
