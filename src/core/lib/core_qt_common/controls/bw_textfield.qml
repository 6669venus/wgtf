import QtQuick 2.1
import QtQuick.Controls 1.0

Item{
	id: baseContainer
	anchors.left : parent.left
	anchors.right : parent.right
	height: childrenRect.height

	property vector2d activatePosition

	// This component is for display when not entering text
	Component{
		id: textBoxComponent
		Text{
			// Bind text read/display to BWTextField.text
			text: baseContainer.parent.text
			color: palette.TextColor
			MouseArea{
				id: textBoxArea
				anchors.fill : parent
				onPressed:{
					activatePosition.x = mouse.x
					activatePosition.y = mouse.y
					textBoxLoader.sourceComponent = textFieldComponent
				}
			}
		}
	}

	// This component is for display when entering text
	Component{
		id: textFieldComponent
		TextField{
			id : textField
			// Bind text read/display to BWTextField.text
			text: baseContainer.parent.text
			onActiveFocusChanged:{
				if(activeFocus == false)
				{
					textBoxLoader.sourceComponent = textBoxComponent
				}
			}
			Keys.onUpPressed:{
				var result = baseContainer.parent.upPressed( textField, cursorPosition )	
				// Must call setText. Do not use "=text", e.g.
				// baseContainer.parent.text = text
				// because it is not valid to have two bindings to write data.
				// See the binding on string_component.qml.
				baseContainer.parent.setText( baseContainer.text )

				cursorPosition = result.cursorPosition;
			}
			Keys.onDownPressed:{
				var result = baseContainer.parent.downPressed( textField, cursorPosition )
				// Must call setText. Do not use "=text", e.g.
				// baseContainer.parent.text = text
				// because it is not valid to have two bindings to write data.
				// See the binding on string_component.qml.
				baseContainer.parent.setText( baseContainer.text )

				cursorPosition = result.cursorPosition;
			}
			Keys.onEscapePressed:{
				baseContainer.parent.forceActiveFocus()	
			}
			Keys.onEnterPressed:{
				// Must call setText. Do not use "=text", e.g.
				// baseContainer.parent.text = text
				// because it is not valid to have two bindings to write data.
				// See the binding on string_component.qml.
				baseContainer.parent.setText( text )

				baseContainer.parent.forceActiveFocus()
			}
			Keys.onReturnPressed:{

				// Must call setText. Do not use "=text", e.g.
				// baseContainer.parent.text = text
				// because it is not valid to have two bindings to write data.
				// See the binding on string_component.qml.
				baseContainer.parent.setText( text )

				baseContainer.parent.forceActiveFocus()
			}
		}
	}

	Loader{
		id: textBoxLoader
		anchors.left : parent.left
		anchors.right : parent.right
		sourceComponent: textBoxComponent
		height:18
		onLoaded:{
			if (sourceComponent == textFieldComponent)
			{
				var textField = textBoxLoader.children[0];
				textField.forceActiveFocus();
				var textInput = textField.children[3];
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

