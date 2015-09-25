import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2
import BWControls 1.0

// TODO:
// * Labels should probably be set via C++/data somehow
// * Fix magic width numbers & alignments
// * Assign proper border/rectangle colours from palette
// * Vertically align label text with number text boxes.
// * Number text boxes should be sized for 3-digit values but clip decimal places unless editing
// * Fix tab focus

WGExpandingRowLayout {
	id: colorLayout
	anchors.left: parent.left
	anchors.right: parent.right

	property bool showAlpha: true

	function getColor(vectorColor) {
		if(showAlpha)
		{
			return Qt.rgba(vectorColor.x / 255, vectorColor.y / 255, vectorColor.z / 255, vectorColor.w / 255);
		}
		else
		{
			return Qt.rgba(vectorColor.x / 255, vectorColor.y / 255, vectorColor.z / 255, 1);
		}
	}

	function getVector(color) {
		if(showAlpha)
		{
			return Qt.vector4d(color.r * 255, color.g * 255, color.b * 255, color.a * 255);
		}
		else
		{
			return Qt.vector3d(color.r * 255, color.g * 255, color.b * 255)
		}
	}

	Item {
		Layout.preferredHeight: parent.height
		Layout.minimumWidth: 80
		Layout.maximumWidth: 80

		WGColorButton {
			id: colButton
			color: getColor(itemData.Value)
			anchors.left: parent.left
			width: 40
			height: parent.height
			defaultColorDialog: false

			onClicked: {
				reflectColorDialog.visible = true
			}

		}

		WGLabel {
			anchors.left: colButton.right
			width: 40
			height: parent.height
			horizontalAlignment: Text.AlignRight
			verticalAlignment: Text.AlignVCenter

			text: {
				if(showAlpha)
				{
					"RGBA:"
				}
				else
				{
					"RGB:"
				}
			}
		}

		/*
		 * don't use modal dialog since On MacOS the color dialog is only allowed to be non-modal.
		 * see http://doc.qt.io/qt-5/qml-qtquick-dialogs-colordialog.html#modality-prop for detail info
		*/
		ColorDialog {
			id: reflectColorDialog
			title: "Please choose a color"
			showAlphaChannel: showAlpha
			color: getColor(itemData.Value)

			onAccepted: {
				var vector = getVector(reflectColorDialog.color);
				itemData.Value = vector;
			}
		}
	}
	Item {
		Layout.preferredHeight: parent.height
		Layout.minimumWidth: 160 + defaultSpacing.rowSpacing
		Layout.maximumWidth: 160 + defaultSpacing.rowSpacing

		WGSplitTextFrame {
			id: splitTextFrame
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			height: parent.height
			width: 120
			boxList: [
				WGNumberBox {
					id: boxX
					number: itemData.Value.x
					minimumValue: 0
					maximumValue: 255

					onNumberChanged: {
						itemData.Value.x = number
					}
				},
				WGNumberBox {
					id: boxY
					number: itemData.Value.y
					minimumValue: 0
					maximumValue: 255

					onNumberChanged: {
						itemData.Value.y = number
					}
				},
				WGNumberBox {
					id: boxZ
					number: itemData.Value.z
					minimumValue: 0
					maximumValue: 255

					onNumberChanged: {
						itemData.Value.z = number
					}
				}
			]
		}

		WGNumberBox {
			id: boxW

			anchors.verticalCenter: parent.verticalCenter
			anchors.left: splitTextFrame.right
			anchors.leftMargin: defaultSpacing.rowSpacing
			height: parent.height
			width: 40

			visible: showAlpha

			number: showAlpha ? itemData.Value.w : 255
			minimumValue: 0
			maximumValue: 255
			noArrows_: true
			horizontalAlignment: Text.AlignHCenter

			onNumberChanged: {
				if(showAlpha)
				{
					itemData.Value.w = number
				}
			}
		}
	}

	Rectangle{
		color: "transparent"
		Layout.fillWidth: true
	}
}
