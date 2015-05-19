import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import "qrc:/controls"


// Default component for displaying each column
Item {
	Layout.fillWidth: true
	Layout.preferredHeight: panelProps.rowHeight_

	Image {
		id: thumbnail
		source: typeof itemData.decoration != "undefined" ?
			itemData.decoration : ""
	}

	Text {
		id: value
		x: thumbnail.x + thumbnail.width
		clip: false
		text: typeof itemData.Value == "string" ?
			itemData.Value : typeof itemData.Value
		color: palette.TextColor
	}
}

