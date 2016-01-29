import QtQuick 2.1
import QtQuick.Controls 1.0

Row
{
	property var source;

	TextField {
		id: numeric
		text: source.numeric
		width: 100
		Binding {
			target: source
			property: "numeric"
			value: numeric.text
		}
	}

	TextField {
		text: source.string
		width: 250
		onTextChanged: {
			source.string = text;
		}
	}

	Button {
		text: "Increment!"
		onClicked: {
			source.incrementNumeric(parseInt(incrementValue.text));
		}
	}

	TextField {
		id: incrementValue
		text: "1"
	}
	
	Connections {
		target: source

		onIncrementNumericInvoked: {
			source.numericChanged(0);
		}
	}
}