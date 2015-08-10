import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

WGExpandingRowLayout {
	anchors.left: parent.left
	anchors.right: parent.right

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
			value: itemData.Value.x
			maximumValue: 2147483647
			minimumValue: -2147483647

			onValueChanged: {
				itemData.Value.x = value
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
			value: itemData.Value.y
			maximumValue: 2147483647
			minimumValue: -2147483647

			onValueChanged: {
				itemData.Value.y = value
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
			text: "Z: "
			horizontalAlignment: Text.AlignRight
			verticalAlignment: Text.AlignVCenter
		}
		WGNumberBox {
			Layout.fillWidth: true
			Layout.preferredHeight: parent.height
			value: itemData.Value.z
			maximumValue: 2147483647
			minimumValue: -2147483647

			onValueChanged: {
				itemData.Value.z = value
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
			text: "W: "
			horizontalAlignment: Text.AlignRight
			verticalAlignment: Text.AlignVCenter
		}
		WGNumberBox {
			Layout.fillWidth: true
			Layout.preferredHeight: parent.height
			value: itemData.Value.w
			maximumValue: 2147483647
			minimumValue: -2147483647

			onValueChanged: {
				itemData.Value.w = value
			}
		}
	}
}
