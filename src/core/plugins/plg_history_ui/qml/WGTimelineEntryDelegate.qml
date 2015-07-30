import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

// Component for dislaying an entry in the history timeline
Item {
	id: timelineEntry

	// Only access DisplayObject once because it is generated every time in C++
	property variant displayObject: itemData.Value.DisplayObject
	property int parentColumnIndex: columnIndex
	property bool isCurrentItem: history.currentIndex == rowIndex

	// Width set by parent
	// Height determined by the batch command
	height: expandingTimelineRow.height

	// Keep timeline arrow a fixed size
	Row {
		id: fixedTimelineRow

		anchors.left: parent.left
		anchors.top: expandingTimelineRow.top
		anchors.bottom: expandingTimelineRow.bottom

		//Current item column & arrow
		Item {
			id: timeline

			width: defaultSpacing.minimumRowHeight
			anchors.top: parent.top
			anchors.bottom: parent.bottom

			Image {
				id: currentArrow
				anchors.centerIn: parent
				source: "qrc:///icons/marker_right_16x16"
				visible: isCurrentItem || arrowButton.containsMouse
				opacity: arrowButton.containsMouse && !isCurrentItem ? 0.3 : 1
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
			id: timelineSeparator
			vertical_: true
			anchors.top: parent.top
			anchors.bottom: parent.bottom
		}
	}

	RowLayout {
		id: expandingTimelineRow
		anchors.left: fixedTimelineRow.right
		anchors.right: parent.right

		property bool isApplied: rowIndex <= history.currentIndex

		WGBatchCommand {
			id: timelineCommand

			Layout.fillWidth: true
			Layout.preferredHeight: height

			displayObject: timelineEntry.displayObject
			isCurrentItem: timelineEntry.isCurrentItem
			isApplied: expandingTimelineRow.isApplied
			columnIndex: parentColumnIndex
		}
	}
}

