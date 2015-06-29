import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

Item {
	Layout.fillWidth: true
	Layout.preferredHeight: panelProps.rowHeight_

	Text {
		id: value
		clip: false
		text: typeof itemData.Value == "string" ? itemData.Value : typeof itemData.Value
		color: palette.TextColor
	}
}
