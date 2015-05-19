import QtQuick 2.1
import QtQuick.Window 2.1
import QtQuick.Controls 1.0
import BWControls 1.0

Item{
	id : baseContainer
	anchors.left : parent.left
	anchors.right : parent.right
	height: childrenRect.height

	function expandComboBox() {
		if( dropDownListWindow.visible )
		{
			dropDownListWindow.close()
			return;
		}
		dropDownListWindow.anchor = baseContainer
		dropDownListWindow.width = parent.width
		dropDownListWindow.height = 200
		dropDownListWindow.show()
	}

	function rowColor( index ) {
		var colors = [ palette.LightPanelColor, palette.MidLightColor ]
		if( index < 0 )
		{
			//Not sure why this happens
			index = 0;
		}
		var colorIndex = index % 2
		return colors[ colorIndex ]
	}

	PopupWindow {
		id : dropDownListWindow
		flags: Qt.Popup | Qt.FramelessWindowHint
		minimumHeight : 10
		maximumHeight : 150
		color: palette.MainWindowColor

		function menuSelect()
		{
			// Always use setChosenItem, and not chosenItem = value.
			// Because it will break the binding in polystruct_component.qml.
			baseContainer.parent.setChosenItem( dropDownList.currentItem.value );
			dropDownListWindow.close()
			baseContainer.parent.forceActiveFocus();
		}

		Component {
			id: listDelegate

			Rectangle {
				anchors.left : parent.left
				anchors.right : parent.right
				color: ListView.isCurrentItem ? palette.HighlightColor : rowColor( index )
				height: childrenRect.height
				property var value: Value

				Text {
					anchors.left : parent.left
					anchors.right : parent.right
					text: display
					color: dropDownList.currentIndex == index ? palette.HighlightTextColor : palette.TextColor

					MouseArea{
						anchors.fill: parent
						hoverEnabled: true
						onPositionChanged:{
							dropDownList.currentIndex = index
						}
					}
				}
			}
		}

		ScrollView{
			anchors.fill: parent
			frameVisible: true
			ListView {
				id: dropDownList
				objectName: "dropDownList"
				width : baseContainer.width
				delegate: listDelegate
				highlightMoveDuration : 0
				boundsBehavior: Flickable.StopAtBounds

			}
		}
		onVisibleChanged:{
			if( visible )
			{
				dropDownList.model = baseContainer.parent.comboModel
				return
			}
		}
	}

	MouseArea {
		id : mouseArea
		anchors.left : parent.left
		anchors.right : parent.right
		hoverEnabled: true
		height : 18

		Text{
			id: comboText
			objectName: "comboText"
			anchors.left : parent.left
			anchors.right : comboExpandButton.left
			color: palette.TextColor
		}

		Text{
			id: comboExpandButton
			anchors.right : parent.right
			height : 18
			width : 18
			font.family : "Marlett"
			font.pointSize : 11
			text : "7"
			color: palette.TextColor
		}

		onPressed:{
			expandComboBox()
		}
	}
}

