import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0
import WGControls 1.0

Rectangle {

	property int itemIndex: index

	// implicit height gives it a fallback if this isn't set.
	// if we give it a set height it won't expand depending on the size of the message.
	// We can use childrenRect.height to get the height of all its children
	// This can cause binding loops very easily though so we have to be careful that none of the children look to the parent for their height

	implicitHeight: childrenRect.height
	radius: panelProps.standardRadius_
	border.width: 1
	border.color: palette.DarkestShade
	color: palette.LightShade

	// Remove alert when the timer is triggered

	// Moved the timer here so it's not taking up a space in the layout
	// Might be unnecessary but is neater

	Timer {
		id: removeAlertTimer
		interval: 10000
		running: true
		onTriggered: {
			removeAlertAt( index )
		}
	}

	// Row layout here allows the icon and text message to be split into separate areas inside the alert frame

	WGExpandingRowLayout {
		// the anchors will work for width but we'll need to keep using childrenRect.height or we'll get loops. In this case with some extra padding from panelProps.doubleMargin_
		anchors.left: parent.left
		anchors.right: parent.right
		height: childrenRect.height + panelProps.doubleMargin_
		anchors.margins: panelProps.standardMargin_

		// As this is written the icon will be centred inside the alert. This looks fine if the message is only one line but odd if it gets larger.
		// Changing the Layout alignment for the icon to Qt::AlignTop might be better.

		Rectangle {
			id: info
			Layout.preferredWidth: 32
			Layout.preferredHeight: 32
			color: "#ffffff"
			radius: 16

			Rectangle {
				id: info2
				anchors.centerIn: parent
				width: 26
				height: 26
				color: "#4295fd"
				radius: 13

				Text {
					id: text1
					anchors.centerIn: parent
					color: "#ffffff"
					text: qsTr("i")
					font.family: "Courier"
					font.pixelSize: 24
					font.bold: true
				}
			}
		}

		// Changed this to a WGMultiLineText so it will word wrap if the message is too long.
		// Text is an odd exception where it doesn't need a specific height or width - it gets this from the font.pixelSize and the text itself.
		WGMultiLineText {
			id: message
			text: Value.message
			font.pixelSize: 22
			horizontalAlignment: Text.AlignRight
			verticalAlignment: Text.AlignBottom
			Layout.fillWidth: true
		}
	}

	// Remove the alert on mouse click
	// MouseArea is also outside the Layout and at the bottom so it is highest in the Zorder.
	MouseArea {
		anchors.fill: parent
		onClicked: {
			removeAlertAt( index )
		}
	}
}
