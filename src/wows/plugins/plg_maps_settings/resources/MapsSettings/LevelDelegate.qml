import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0

// Component for dislaying a Level class from Python
Rectangle {
	id: levelEntry
    color: Qt.darker( palette.MainWindowColor, 1.2 )

	property variant value: itemData.Value

	Row {
		id: scenarioRow
		anchors.fill: parent

		Text {
			id: rateText
			text: typeof value.rate == "string" ?
				value.rate :
				typeof value.rate
			color: palette.NeutralTextColor
		}

		WGSeparator {
			id: timelineSeparator
			vertical_: true
			anchors.top: parent.top
			anchors.bottom: parent.bottom
		}

		Text {
			id: listText
			text: typeof value.list == "string" ?
				value.list :
				typeof value.list
			color: palette.NeutralTextColor
		}
	}
}

