import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import "qrc:/controls"
import "itemCreation.js" as Code


// Component for dislaying command instances
Rectangle {
	id: commandInstance


	// -- Begin Interface
	// Only access DisplayObject once because it is generated every time in C++
	property variant displayObject: parent.itemData.Value.DisplayObject

	property int rowIndex: parent.rowIndex
	property int columnIndex: parent.columnIndex

	property bool isCurrentItem: parent.isCurrentItem
	property int lastIndexClicked: parent.lastIndexClicked
	property bool isHighlighted: parent.isHighlighted
	// -- End Interface

	color: isHighlighted ? palette.HighlightShade : "transparent"
	border.width: rowIndex == lastIndexClicked ? 1 : 0
	border.color: rowIndex == lastIndexClicked ? palette.HighlightColor : "transparent"

	WGExpandingRowLayout {
		id: commandInstanceRow

		// -- Begin Interface
		property int columnIndex: parent.columnIndex
		property variant displayObject: parent.displayObject
		property bool isApplied: (rowIndex <= history.currentIndex)
		// -- End Interface

		//Current item column & arrow
		Item {
			id: timeline

			Layout.preferredHeight: panelProps.rowHeight_
			Layout.preferredWidth: panelProps.rowHeight_

			Image {
				id: currentArrow
				anchors.centerIn: parent
				source: "qrc:///icons/marker_right_16x16"
				visible: rowIndex == history.currentIndex || arrowButton.containsMouse
				opacity: arrowButton.containsMouse && rowIndex != history.currentIndex ? 0.3 : 1
			}

			MouseArea {
				id: arrowButton
				anchors.fill: parent
				hoverEnabled: true

				cursorShape: Qt.PointingHandCursor

				onClicked: {
					history.currentIndex = rowIndex;
				}
			}
		}

		WGSeparator {
			vertical_: true
		}

		Component.onCompleted: Code.createObjects( commandInstanceRow )
	}
}

