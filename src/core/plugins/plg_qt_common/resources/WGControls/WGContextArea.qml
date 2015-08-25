import QtQuick 2.3
import QtQuick.Controls 1.2

//Context sensitive Right Click Menu

MouseArea {
	anchors.fill: parent
	propagateComposedEvents: true
	preventStealing: true

	acceptedButtons: Qt.RightButton

	onClicked: {
		if(mouse.button == Qt.RightButton){
			contextMenu.popup()
			mouse.accepted = false
		} else {
			mouse.accepted = false
		}
	}
}

