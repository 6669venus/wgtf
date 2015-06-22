import QtQuick 2.3
import QtQuick.Controls.Styles 1.2


MenuStyle {

	frame: Rectangle {
		color: palette.DarkHeaderColor
		WGSeparator {
			vertical_: true
			anchors.verticalCenter: parent.verticalCenter
			anchors.left: parent.left
			anchors.leftMargin: panelProps.rowHeight_
			height: parent.height - panelProps.doubleBorder_
		}
	}

	itemDelegate.background: Rectangle {
		color: "transparent"
		height: panelProps.rowHeight_
		WGHighlightFrame {
			anchors.fill: parent
			visible: styleData.selected
		}
	}

	itemDelegate.label: WGLabel {
		text: "   " + styleData.text
		color: {
			if (styleData.enabled && styleData.selected){
				palette.HighlightTextColor
			} else if (styleData.enabled && !styleData.selected){
				palette.TextColor
			} else if (!styleData.enabled){
				palette.DisabledTextColor
			}
		}
	}

	scrollIndicator: Text {
		text: styleData.scrollerDirection ==  Qt.DownArrow ? "6" : "5"
		font.family : "Marlett"
		color: palette.TextColor
		renderType: Text.NativeRendering
		verticalAlignment: Text.AlignVCenter
		horizontalAlignment: Text.AlignHCenter
	}

	itemDelegate.submenuIndicator: Text {
		text: "8"
		font.family : "Marlett"
		color: {
			if (styleData.enabled && styleData.selected){
				palette.HighlightTextColor
			} else if (styleData.enabled && !styleData.selected){
				palette.TextColor
			} else if (!styleData.enabled){
				palette.DisabledTextColor
			}
		}
		renderType: Text.NativeRendering
		verticalAlignment: Text.AlignVCenter
		y: 3
	}

	itemDelegate.checkmarkIndicator: WGTextBoxFrame {
		id: checkboxFrame
		implicitWidth: 14
		implicitHeight: 14

		color: {
			if (styleData.enabled){
				palette.TextBoxColor
			} else {
				"transparent"
			}
		}

		border.color: {
			if (styleData.enabled) {
				palette.DarkestShade
			} else {
				palette.DarkerShade
			}
		}

		Rectangle {
			visible: styleData.checked
			color: {
				if (styleData.enabled && styleData.checked){
					palette.HighlightColor
				} else if (styleData.enabled && !styleData.checked){
					palette.HighlightShade
				} else if (!styleData.enabled){
					palette.LightShade
				}
			}
			radius: panelProps.halfRadius_
			anchors.fill: parent
			anchors.margins: styleData.checked ? 1 : 2 //smaller dull blue square if partially checked

			Text {
				id : tickMark
				color : styleData.enabled ? palette.HighlightTextColor : palette.LightestShade
				font.family : "Marlett"
				font.pixelSize: checkboxFrame.height + panelProps.standardRadius_
				renderType: Text.NativeRendering
				text : "a"
				visible : styleData.checked //invisible if partially checked
				anchors.fill: parent
				anchors.leftMargin: panelProps.halfRadius_
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignHCenter
			}
		}
	}

	separator: Rectangle {
		color: "transparent"
		height: panelProps.separatorWidth_ + panelProps.doubleBorder_

		WGSeparator {
			vertical_: false
			anchors.verticalCenter: parent.verticalCenter
			anchors.right: parent.right
			anchors.left: parent.left
			anchors.rightMargin: panelProps.borderWidth_
			anchors.leftMargin: panelProps.rowHeight_ + (panelProps.separatorWidth_ / 2)
		}
	}
}

