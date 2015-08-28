import QtQuick 2.3
import QtQuick.Layouts 1.1
import BWControls 1.0

RowLayout
{
	id: vector2Layout
	anchors.fill: parent
	spacing: 0

	// properties to change the field label.	
	property string xCaption: "X"
	property string yCaption: "Y"

	Rectangle 
	{
		height: parent.height
		width: parent.width / 2
		Layout.fillHeight: true
		Layout.fillWidth: true
		color: palette.MidLightColor

		RowLayout
		{
			anchors.fill: parent
			spacing: 0
		
			Rectangle 
			{
				Layout.fillWidth: true
				Layout.minimumWidth: 16
				Layout.preferredWidth: 16
				Layout.maximumWidth: 16
				Layout.minimumHeight: parent.height
				color: palette.MidLightColor
		
				Text 
				{
					height: parent.height
					width: parent.width
					verticalAlignment: Text.AlignVCenter
					text: xCaption + ":"
					color: palette.TextColor
					font.bold: true
				}
			}
		
			Rectangle 
			{
				Layout.fillWidth: true
				Layout.minimumHeight: parent.height
				color: palette.MidLightColor
		
				BWTextField
				{
					height: parent.height
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter
					text: itemData.Value.x
					onTextChanged: 
					{
						itemData.Value.x = text
					}
				}
			}
		}
	}

	Rectangle 
	{
		height: parent.height
		width: parent.width / 2
		Layout.fillHeight: true
		Layout.fillWidth: true
		color: palette.MidLightColor

		RowLayout
		{
			anchors.fill: parent
			spacing: 0

			Rectangle 
			{
				Layout.fillWidth: true
				Layout.minimumWidth: 16
				Layout.preferredWidth: 16
				Layout.maximumWidth: 16
				Layout.minimumHeight: parent.height
				color: palette.MidLightColor

				Text 
				{
					height: parent.height
					width: parent.width
					verticalAlignment: Text.AlignVCenter
					text: yCaption + ":"
					color: palette.TextColor
					font.bold: true
				}
			}

			Rectangle 
			{
				Layout.fillWidth: true
				Layout.minimumHeight: parent.height
				color: palette.MidLightColor

				BWTextField
				{
					height: parent.height
					width: parent.width
					anchors.verticalCenter: parent.verticalCenter
					text: itemData.Value.y
					onTextChanged: 
					{
						itemData.Value.y = text
					}
				}
			}
		}
	}
}
