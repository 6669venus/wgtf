import QtQuick 2.1
import QtQuick.Controls 1.0

Row
{
	property var source;

	TextField {
		id: numeric
		text: source.Numeric
		width: 100
		Binding {
			target: source
			property: "Numeric"
			value: numeric.text
		}
	}

	TextField {
		text: source.String
		width: 250
		onTextChanged: {
			source.String = text;
		}
	}
}