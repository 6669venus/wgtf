import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

Item {
	Layout.fillWidth: true
	Layout.preferredHeight: minimumRowHeight

	Text {
		id: value
		clip: true
		anchors.left: parent.left
		anchors.top: parent.top
		anchors.bottom: parent.bottom
		anchors.margins: 4
		verticalAlignment: Text.AlignVCenter
		text: typeof itemData.Value == "string" ? itemData.Value : typeof itemData.Value
		color: palette.TextColor
	}
}
