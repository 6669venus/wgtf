import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

Rectangle {
	id: rootFrame
	property var title: "WGContextMenu Test"
	property var layoutHints: { 'test': 0.1 }
	property var sourceModel: source
	color: palette.MainWindowColor
	z: 1

	WGExpandingRowLayout {
		anchors.fill: parent

		WGLabel {
			text: "Test the context menu by right-clicking on this label."

			MouseArea {
				anchors.fill: parent
				acceptedButtons: Qt.AllButtons;
				onClicked: {
					if (mouse.button == Qt.RightButton) {
						testContextMenu.show();
					}
				}
			}
		}

		WGContextMenu {
			id: testContextMenu
			path: "ContextMenu"
			windowId: ""
			contextManager: componentContext
		}
	}
}
