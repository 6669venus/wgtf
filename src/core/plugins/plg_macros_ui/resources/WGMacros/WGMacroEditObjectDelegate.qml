import QtQuick 2.1
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Window 2.2
import BWControls 1.0
import WGControls 1.0


Rectangle {
	id: macroEditObject
	color: palette.MainWindowColor
	width: 200
	height: 20
	property variant path;
	property variant value;
	property variant oldPath;
	property variant oldValue;

	function onOkHandler() {
		if((oldPath == path) && (oldValue == value))
		{
			return;
		}

		itemData.Value.PropertyPath = path
		// TODO how to convert value from string to PropertyValue's type
		itemData.Value.PropertyValue = value;
		oldPath = path;
		oldValue = value;
		root.accepted = true
	}
	function onCancelHandler() {
		if(oldPath == null)
		{
			text1.text = ""
		}
		else
		{
			text1.text = oldPath
		}

		if(oldValue == null)
		{
			text2.text = ""
		}
		else
		{
			text2.text = oldValue.toString()
		}
	}
	Component.onCompleted: {
		oldPath = itemData.Value.PropertyPath
		oldValue = itemData.Value.PropertyValue
		okButton.onOk.connect( onOkHandler )
		cancelButton.onCancel.connect( onCancelHandler )
	}
	
	WGExpandingRowLayout {
		id: macroEditObjectRow
		anchors.fill: parent
		Text {
			id: label1
			width: paintedWidth
			clip: false
			text: "Property:"
			color: palette.TextColor
		}

		WGTextBox {
			id: text1
			clip: false
			text: itemData.Value.PropertyPath
			onTextChanged: {
				path = text
			}
		}

		WGSeparator {
			vertical_: true
		}

		Text {
			id: label2
			width: paintedWidth
			clip: false
			text: "Value:"
			color: palette.TextColor
		}

	
		WGTextBox {
			id: text2
			clip: false
			text: itemData.Value.PropertyValue.toString()
			onTextChanged: {
				value = text
			}
		}
	}
}

