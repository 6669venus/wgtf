import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import BWControls 1.0
import WGControls 1.0


// Component for displaying compound commands
Item {

	height: Math.max( defaultSpacing.minimumRowHeight, macroRow.height )

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
		id: macroRow
		anchors.left: parent.left
		anchors.right: parent.right

		//Current item column & arrow
		Item {
			id: expandArrow

			Layout.minimumWidth: expandArrowImage.width
			Layout.minimumHeight: expandArrowImage.height
			
			Layout.preferredWidth: defaultSpacing.minimumRowHeight
			Layout.preferredHeight: defaultSpacing.minimumRowHeight

			property bool expanded: false

			Image {
				id: expandArrowImage
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

		WGDropDownBox {
			id: contextObject
			Layout.fillWidth: true
 
			WGListModel {
				id: contextObjects
				source: itemData.Value.DisplayObject.ContextObjects
 
				ValueExtension {}
			}
 
			model: contextObjects
			textRole: "display"

			Component.onCompleted: {
				currentIndex = Qt.binding( function() { 
					var modelIndex = contextObjects.find( itemData.Value.DisplayObject.ContextObject, "Value" );
					return contextObjects.indexRow( modelIndex ); } )
			}

			Connections {
				target: contextObject
				onCurrentIndexChanged: {
					if (contextObject.currentIndex < 0) {
						return;
					}
					var modelIndex = contextObjects.index( contextObject.currentIndex );
					itemData.Value.DisplayObject.ContextObject = contextObjects.data( modelIndex, "Value" );
				}
			}
		}
	}
}

