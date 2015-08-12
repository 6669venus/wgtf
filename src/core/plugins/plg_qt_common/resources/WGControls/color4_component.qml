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
				beginUndoFrame();
				reflectColorDialog.visible = true
			}

			onColorChanged: {
				itemData.Value = getVector(colButton.color)
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

		ColorDialog {
			id: reflectColorDialog
			title: "Please choose a color"
			showAlphaChannel: showAlpha
			modality: Qt.ApplicationModal
			color: getColor(itemData.Value)

			onAccepted: {
				var vector = getVector(reflectColorDialog.color);
				itemData.Value = vector;
				endUndoFrame();
			}

			onRejected: {
				abortUndoFrame();
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
					value: itemData.Value.x
					minimumValue: 0
					maximumValue: 255

					onValueChanged: {
						itemData.Value.x = value
					}
				},
				WGNumberBox {
					id: boxY
					value: itemData.Value.y
					minimumValue: 0
					maximumValue: 255

					onValueChanged: {
						itemData.Value.y = value
					}
				},
				WGNumberBox {
					id: boxZ
					value: itemData.Value.z
					minimumValue: 0
					maximumValue: 255

					onValueChanged: {
						itemData.Value.z = value
					}
				}
			]
		}

		WGNumberBox {
			id: boxW

			//Lots of hassle getting this box to lineup correctly.
			//No clue as to why it is this hard but it was ALWAYS off by 1 no matter where it was.

			//Alpha box is separate from RGB boxes to avoid confusion.

			anchors.verticalCenter: parent.verticalCenter
			anchors.verticalCenterOffset: 1
			anchors.left: splitTextFrame.right
			anchors.leftMargin: defaultSpacing.rowSpacing
			height: parent.height
			width: 40

			visible: showAlpha

			value: showAlpha ? itemData.Value.w : 255
			minimumValue: 0
			maximumValue: 255
			noArrows_: true
			horizontalAlignment: Text.AlignHCenter

			onValueChanged: {
				if(showAlpha)
				{
					itemData.Value.w = value
				}
			}
		}
	}

	Rectangle{
		color: "transparent"
		Layout.fillWidth: true
	}
}
