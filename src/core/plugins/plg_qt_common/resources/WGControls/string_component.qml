import QtQuick 2.3
import BWControls 1.0

WGTextBox {
	id: textField
	anchors.left: parent.left
	anchors.right: parent.right
	text: itemData.Value

	onEditAccepted: {
		itemData.Value = text;
	}
}

