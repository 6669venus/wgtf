import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

Item {
	id: treeView

	//TODO: Move WGPanelProperties into C++ as a context property.
    property QtObject panelProps: WGPanelProperties{}
	property var model
	property real leftMargin: 2
	property real rightMargin: 2
	property real topMargin: 2
	property real bottomMargin: 2
	property real spacing: 1
	property real columnSpacing: 1
	property real selectionMargin: 0
	property real expandIconMargin: 3
	property real childListMargin: 0
	property real footerSpacing: 0
	property real headerRowMargin: 0
	property real childRowMargin: 0
	property real minimumRowHeight: panelProps.rowHeight_
	property int indentation: 8
	property var selectionExtension: null
	property var columnDelegates: []

	property Component defaultColumnDelegate: Text {
		property var itemData: null

		color: palette.TextColor
		clip: itemData != null && itemData.Component != null
		text: itemData != null ? itemData.display : ""
		font.bold: itemData != null && itemData.HasChildren
		verticalAlignment: Text.AlignVCenter
	}

	WGTreeItem {
		id: rootItem
		leftMargin: treeView.leftMargin
		rightMargin: treeView.rightMargin
		topMargin: treeView.topMargin
		bottomMargin: treeView.bottomMargin
		spacing: treeView.spacing
		childListMargin: treeView.childListMargin
		model: treeView.model
		verticalScrollBar: true
		width: treeView.width
		height: treeView.height
	}
}
