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

	WGColumnLayout {
		id: levelColumn
		anchors.fill: parent

		Row {
			id: levelRow
			Layout.fillWidth: true
			Layout.fillHeight: false

			Text {
				id: listText
				text: "Levels " + value.list
				color: palette.NeutralTextColor
			}

			WGSeparator {
				id: separator
				vertical_: true
				anchors.top: parent.top
				anchors.bottom: parent.bottom
			}

			Text {
				id: rateText
				text: " Rate " + value.rate
				color: palette.NeutralTextColor
			}
		}
	}
}

