import QtQuick 2.3
import QtQuick.Controls 1.2

Item{
	anchors.left : parent.left
	anchors.right : parent.right
	height: 18

	property vector2d activatePosition

	Component{
		id: textBoxComponent
		Text{
			text: boundValue
			color: selected ? palette.HighlightTextColor : palette.TextColor
			MouseArea{
				id: textBoxArea
				anchors.fill: parent
				onPressed:{
					activatePosition.x = mouse.x
					activatePosition.y = mouse.y
					textBoxLoader.sourceComponent = textFieldComponent
				}
			}
		}
	}

	Component{
		id: textFieldComponent
		TextField{
			id : textField
			text: boundValue
			onActiveFocusChanged:{
				if(activeFocus == false)
				{
					sourceComponent = textBoxComponent
				}
			} 
			Keys.onEscapePressed:{
				textBoxLoader.parent.forceActiveFocus()	
			}
			Keys.onReturnPressed:{
				boundValue = text
				textBoxLoader.parent.forceActiveFocus()
			}
		}
	}

	Loader{
		id: textBoxLoader
		anchors.left : parent.left
		anchors.right : parent.right
		sourceComponent: textBoxComponent
		onLoaded:{
			if (sourceComponent == textFieldComponent)
			{
				var textField =
					actionHandler.findChildItem( textBoxLoader, "TextField" );
				textField.forceActiveFocus()
				var textInput =
					actionHandler.findChildItem( textField, "TextInput" );
				textInput.autoScroll = false
				var mappedMouse =
					mapToItem(
						textInput, activatePosition.x, activatePosition.y );
				textField.cursorPosition =
					textInput.positionAt(
						mappedMouse.x, mappedMouse.y )
				textInput.autoScroll = true
			}
		}
	}
}

