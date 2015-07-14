import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Slider {
	id: slider
	minimumValue: itemData.MinValue
	maximumValue: itemData.MaxValue
	updateValueWhileDragging: true
	readonly property alias pressed: mouseArea.pressed

	MouseArea {
		id: mouseArea

		hoverEnabled: true
		anchors.centerIn: slider

		preventStealing: true

		width: slider.width
		height: slider.height

		property int clickOffset: 0

		function clamp (val) {
			return Math.max(slider.minimumValue, Math.min(slider.maximumValue, val));
		}

		onMouseXChanged: {
			if (pressed) {
				var pos = clamp(slider.minimumValue + (mouse.x * (slider.maximumValue - slider.minimumValue) / slider.width));
				slider.value = pos;
			}
		}

		onPressed: {
			beginUndoFrame();

			if (slider.activeFocusOnPress)
			{
				slider.forceActiveFocus();
			}
		}

		onReleased: {
			endUndoFrame();
		}
	}

	Binding {
		target: itemData
		property: "Value"
		value: slider.value
	}

	// seems value : itemData.Value would lost binding sometimes
	// change by using Binding QML type to binding two values
	Binding {
		target: slider
		property: "value"
		value: itemData.Value
	}

	style: SliderStyle {
		groove: Rectangle {
			implicitHeight: slider.height / 4
			color: "#383838"
			border.color: "#2b2b2b"
			border.width: 1
			radius: 4

			Rectangle {
				id: blueBar
				height: parent.height
				border.color: "#282828"
				border.width: 1
				width: styleData.handlePosition
				anchors.margins: 1
				radius: 3
				color: palette.HighlightColor
			}
		}

		handle: Rectangle {
			id: slideHandle
			anchors.centerIn: parent
			color: "#484848"
			border.color: "#2b2b2b"
			border.width: 2
			implicitWidth: slider.height * 0.8
			implicitHeight: slider.height * 0.8
			radius: 4

			Rectangle {
				anchors.centerIn: parent
				anchors.fill: parent
				anchors.margins: 1
				border.color: "#676767"
				border.width: 1
				radius: 3

				gradient: Gradient {
					GradientStop { position: 0.0; color: "#525252" }
					GradientStop { position: 1.0; color: "#454545" }
				}
			}
		}
	}
}
