import QtQuick 2.1
import QtQuick.Controls 1.0

Row
{
	property var source;

	Text {
		text: "Numeric Value: " + source.Numeric
		color: palette.TextColor
		width: 100
	}

	Text {
		text: "String Value: " + source.String
		color: palette.TextColor
		width: 100
	}
}