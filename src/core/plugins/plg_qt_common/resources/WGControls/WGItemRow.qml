import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import BWControls 1.0

Item {
	id: itemRow
    objectName: "WGItemRow"

	width: childrenRect.width
	height: childrenRect.height

	property Component columnDelegate
	property var columnDelegates: []
    property var columnSequence: []
	property var columnWidths: []
	property alias columnSpacing: row.spacing

	property bool selected: false

	signal itemPressed(var mouse, var itemIndex)
    signal itemClicked(var mouse, var itemIndex)
    signal itemDoubleClicked(var mouse, var itemIndex)

	/* MOVE INTO STYLE*/
	Rectangle {
		id: backgroundArea
		anchors.fill: row
        color: palette.highlightShade
		opacity: selected ? 1 : 0.5 
        visible: hoverArea.containsMouse || selected
    }

	MouseArea {
		id: hoverArea
		anchors.fill: backgroundArea
		hoverEnabled: true
	}
	/**/

	Row {
		id: row

		Repeater {
			model: SequenceList {
				model: columnModel
				sequence: columnSequence
			}

			Item {
				width: columnWidths[index]
				height: childrenRect.height
				clip: true

				MouseArea {
					width: columnWidths[index]
					height: row.height
					acceptedButtons: Qt.RightButton | Qt.LeftButton;

					onPressed: itemPressed(mouse, modelIndex)
					onClicked: itemClicked(mouse, modelIndex)
					onDoubleClicked: itemDoubleClicked(mouse, modelIndex)
				}

				Loader {
					property var itemData: model
					property var itemWidth: columnWidths[index]
					sourceComponent: itemRow.columnDelegates[index]
				}
			}
		}
    }
}
