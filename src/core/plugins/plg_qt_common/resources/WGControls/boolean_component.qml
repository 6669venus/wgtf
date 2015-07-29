import QtQuick 2.3
import BWControls 1.0

WGCheckBox{
	id: checkbox
	anchors.left: parent.left
	//click on the checkbox will break the "checked: itemData.Value" binding
	// see: https://bugreports.qt.io/browse/QTBUG-42505 for reference
	onClicked: {
		itemData.Value = (checkedState === Qt.Checked) ? true : false;
	}
	Component.onCompleted: {
		checked = itemData.Value;
	}
	Binding {
		target: checkbox
		property: "checked"
		value: itemData.Value
	}
}
