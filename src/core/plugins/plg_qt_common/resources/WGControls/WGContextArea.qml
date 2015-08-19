import QtQuick 2.3
import QtQuick.Controls 1.2
import WGControls 1.0

//Context sensitive Right Click Menu

MouseArea {
	anchors.fill: parent
	propagateComposedEvents: true
	preventStealing: true

	acceptedButtons: Qt.RightButton

	default property var contextMenu
	
	onClicked: {
		if(mouse.button == Qt.RightButton){
			if(contextMenu && contextMenu.items.length > 0){
				mouse.accepted = true
				contextMenu.popup()
			}
		} else {
			mouse.accepted = false
		}
	}
}

