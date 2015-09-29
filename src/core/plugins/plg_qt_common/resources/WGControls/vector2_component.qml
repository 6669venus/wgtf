import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

//TODO:
// Would be nice if this just inherited vector4_component and turned off the last two value
// but doing so caused a major crash.
// Need to add in proper min/max values and required decimal points.

WGExpandingRowLayout {
	anchors.fill: parent
	height: parent.height

	WGExpandingRowLayout{
		Layout.fillWidth: true
		Layout.minimumWidth: 60
		Layout.preferredHeight: parent.height
		WGLabel{
			Layout.preferredWidth: 16
			Layout.preferredHeight: parent.height
			Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
			text: "X: "
			horizontalAlignment: Text.AlignRight
			verticalAlignment: Text.AlignVCenter
		}
		WGNumberBox {
			Layout.fillWidth: true
			Layout.preferredHeight: parent.height
			number: itemData.Value.x
			maximumValue: 2147483647
			minimumValue: -2147483647
			stepSize: 0.1
			decimals: 3

			onNumberChanged: {
				itemData.Value.x = number
			}
		}
	}
	WGExpandingRowLayout{
		Layout.fillWidth: true
		Layout.minimumWidth: 60
		Layout.preferredHeight: parent.height
		WGLabel{
			Layout.preferredWidth: 16
			Layout.preferredHeight: parent.height
			Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
			text: "Y: "
			horizontalAlignment: Text.AlignRight
			verticalAlignment: Text.AlignVCenter
		}
		WGNumberBox {
			Layout.fillWidth: true
			Layout.preferredHeight: parent.height
			number: itemData.Value.y
			maximumValue: 2147483647
			minimumValue: -2147483647
			stepSize: 0.1
			decimals: 3

			onNumberChanged: {
				itemData.Value.y = number
			}
		}
	}
}
