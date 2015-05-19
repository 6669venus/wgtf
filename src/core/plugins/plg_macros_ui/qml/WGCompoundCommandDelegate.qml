import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import "qrc:/controls"


// Component for displaying compound commands
Rectangle {

	color: isHighlighted ? palette.HighlightShade : "transparent"

	border.width: rowIndex == macros.lastIndexClicked ? 1 : 0
	border.color: rowIndex == macros.lastIndexClicked  ? palette.HighlightColor : "transparent"

	property QtObject macroWindow: WGMacroEditWindow{}
	function closeHandler() {
		if(macroWindow.accepted)
		{
			itemData.Value.DisplayObject.UpdateMacroData
		}
	}

	Component.onCompleted: {
		macroWindow.closing.connect( closeHandler )
	}
	WGExpandingRowLayout {

		anchors.fill: parent

					//Current item column & arrow
		Rectangle {
			id: expandArrow
			Layout.preferredHeight: panelProps.rowHeight_
			Layout.preferredWidth: panelProps.rowHeight_

			property bool expanded: false

			color: "transparent"

			Image {
				source: expandArrow.expanded ? "qrc:///icons/arrow_down_16x16" : "qrc:///icons/arrow_right_16x16"
				anchors.centerIn: parent
			}

			MouseArea {
				anchors.fill: parent
				z: 10
				cursorShape: Qt.PointingHandCursor
				onClicked: {
					
					expandArrow.expanded = !expandArrow.expanded
					//expand the macro and show child actions.
					//this should really be handled by a TreeView of some kind.
				}
			}
		}

		Text{
			id: textField
			Layout.minimumWidth: paintedWidth
			clip: false
			text: itemData.Value.DisplayObject.DisplayName
			color: palette.TextColor
		}

		WGToolButton {
			iconSource: "qrc:///icons/edit_16x16"
			onClicked: {
				macroWindow.sourceData = itemData.Value.DisplayObject.EditData
				macroWindow.accepted = false
				macroWindow.title = "Edit " + itemData.Value.DisplayObject.DisplayName
				macroWindow.show()
			}
		}

		WGToolButton {
			iconSource: checked ? "qrc:///icons/dialogue_on_16x16" : "qrc:///icons/dialogue_off_16x16"
			checkable: true
			checked: false
			onClicked: {
				//turn on show a dialogue box on this step
			}
		}

		Rectangle {
			Layout.fillWidth: true
			color: "transparent"
		}


		Text{
			id: label
			Layout.minimumWidth: paintedWidth
			clip: false
			text: "SelectContextObject:"
			color: palette.TextColor
		}

		BWComboBox{
			id: contextObject
			Layout.fillWidth: true
			comboModel: itemData.Value.DisplayObject.ContextObjects
			chosenItem: itemData.Value.DisplayObject.ContextObject
			Binding {
				target: itemData.Value.DisplayObject
				property: "ContextObject"
				value: contextObject.chosenItem
			}
		}
	}
}

